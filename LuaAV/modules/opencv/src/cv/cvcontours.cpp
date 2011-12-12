/*M///////////////////////////////////////////////////////////////////////////////////////
//
//  IMPORTANT: READ BEFORE DOWNLOADING, COPYING, INSTALLING OR USING.
//
//  By downloading, copying, installing or using the software you agree to this license.
//  If you do not agree to this license, do not download, install,
//  copy or use the software.
//
//
//                        Intel License Agreement
//                For Open Source Computer Vision Library
//
// Copyright (C) 2000, Intel Corporation, all rights reserved.
// Third party copyrights are property of their respective owners.
//
// Redistribution and use in source and binary forms, with or without modification,
// are permitted provided that the following conditions are met:
//
//   * Redistribution's of source code must retain the above copyright notice,
//     this list of conditions and the following disclaimer.
//
//   * Redistribution's in binary form must reproduce the above copyright notice,
//     this list of conditions and the following disclaimer in the documentation
//     and/or other materials provided with the distribution.
//
//   * The name of Intel Corporation may not be used to endorse or promote products
//     derived from this software without specific prior written permission.
//
// This software is provided by the copyright holders and contributors "as is" and
// any express or implied warranties, including, but not limited to, the implied
// warranties of merchantability and fitness for a particular purpose are disclaimed.
// In no event shall the Intel Corporation or contributors be liable for any direct,
// indirect, incidental, special, exemplary, or consequential damages
// (including, but not limited to, procurement of substitute goods or services;
// loss of use, data, or profits; or business interruption) however caused
// and on any theory of liability, whether in contract, strict liability,
// or tort (including negligence or otherwise) arising in any way out of
// the use of this software, even if advised of the possibility of such damage.
//
//M*/
#include "_cv.h"

/* initializes 8-element array for fast access to 3x3 neighborhood of a pixel */
#define  CV_INIT_3X3_DELTAS( deltas, step, nch )            \
    ((deltas)[0] =  (nch),  (deltas)[1] = -(step) + (nch),  \
     (deltas)[2] = -(step), (deltas)[3] = -(step) - (nch),  \
     (deltas)[4] = -(nch),  (deltas)[5] =  (step) - (nch),  \
     (deltas)[6] =  (step), (deltas)[7] =  (step) + (nch))

static const CvPoint icvCodeDeltas[8] =
    { {1, 0}, {1, -1}, {0, -1}, {-1, -1}, {-1, 0}, {-1, 1}, {0, 1}, {1, 1} };

CV_IMPL void
cvStartReadChainPoints( CvChain * chain, CvChainPtReader * reader )
{
    int i;

    if( !chain || !reader )
        CV_Error( CV_StsNullPtr, "" );

    if( chain->elem_size != 1 || chain->header_size < (int)sizeof(CvChain))
        CV_Error( CV_StsBadSize, "" );

    cvStartReadSeq( (CvSeq *) chain, (CvSeqReader *) reader, 0 );

    reader->pt = chain->origin;
    for( i = 0; i < 8; i++ )
    {
        reader->deltas[i][0] = (schar) icvCodeDeltas[i].x;
        reader->deltas[i][1] = (schar) icvCodeDeltas[i].y;
    }
}


/* retrieves next point of the chain curve and updates reader */
CV_IMPL CvPoint
cvReadChainPoint( CvChainPtReader * reader )
{
    schar *ptr;
    int code;
    CvPoint pt = { 0, 0 };

    if( !reader )
        CV_Error( CV_StsNullPtr, "" );

    pt = reader->pt;

    ptr = reader->ptr;
    if( ptr )
    {
        code = *ptr++;

        if( ptr >= reader->block_max )
        {
            cvChangeSeqBlock( (CvSeqReader *) reader, 1 );
            ptr = reader->ptr;
        }

        reader->ptr = ptr;
        reader->code = (schar)code;
        assert( (code & ~7) == 0 );
        reader->pt.x = pt.x + icvCodeDeltas[code].x;
        reader->pt.y = pt.y + icvCodeDeltas[code].y;
    }

    return pt;
}


/****************************************************************************************\
*                         Raster->Chain Tree (Suzuki algorithms)                         *
\****************************************************************************************/

typedef struct _CvContourInfo
{
    int flags;
    struct _CvContourInfo *next;        /* next contour with the same mark value */
    struct _CvContourInfo *parent;      /* information about parent contour */
    CvSeq *contour;             /* corresponding contour (may be 0, if rejected) */
    CvRect rect;                /* bounding rectangle */
    CvPoint origin;             /* origin point (where the contour was traced from) */
    int is_hole;                /* hole flag */
}
_CvContourInfo;


/*
  Structure that is used for sequental retrieving contours from the image.
  It supports both hierarchical and plane variants of Suzuki algorithm.
*/
typedef struct _CvContourScanner
{
    CvMemStorage *storage1;     /* contains fetched contours */
    CvMemStorage *storage2;     /* contains approximated contours
                                   (!=storage1 if approx_method2 != approx_method1) */
    CvMemStorage *cinfo_storage;        /* contains _CvContourInfo nodes */
    CvSet *cinfo_set;           /* set of _CvContourInfo nodes */
    CvMemStoragePos initial_pos;        /* starting storage pos */
    CvMemStoragePos backup_pos; /* beginning of the latest approx. contour */
    CvMemStoragePos backup_pos2;        /* ending of the latest approx. contour */
    schar *img0;                /* image origin */
    schar *img;                 /* current image row */
    int img_step;               /* image step */
    CvSize img_size;            /* ROI size */
    CvPoint offset;             /* ROI offset: coordinates, added to each contour point */
    CvPoint pt;                 /* current scanner position */
    CvPoint lnbd;               /* position of the last met contour */
    int nbd;                    /* current mark val */
    _CvContourInfo *l_cinfo;    /* information about latest approx. contour */
    _CvContourInfo cinfo_temp;  /* temporary var which is used in simple modes */
    _CvContourInfo frame_info;  /* information about frame */
    CvSeq frame;                /* frame itself */
    int approx_method1;         /* approx method when tracing */
    int approx_method2;         /* final approx method */
    int mode;                   /* contour scanning mode:
                                   0 - external only
                                   1 - all the contours w/o any hierarchy
                                   2 - connected components (i.e. two-level structure -
                                   external contours and holes) */
    int subst_flag;
    int seq_type1;              /* type of fetched contours */
    int header_size1;           /* hdr size of fetched contours */
    int elem_size1;             /* elem size of fetched contours */
    int seq_type2;              /*                                       */
    int header_size2;           /*        the same for approx. contours  */
    int elem_size2;             /*                                       */
    _CvContourInfo *cinfo_table[126];
}
_CvContourScanner;

#define _CV_FIND_CONTOURS_FLAGS_EXTERNAL_ONLY    1
#define _CV_FIND_CONTOURS_FLAGS_HIERARCHIC       2

/*
   Initializes scanner structure.
   Prepare image for scanning ( clear borders and convert all pixels to 0-1.
*/
CV_IMPL CvContourScanner
cvStartFindContours( void* _img, CvMemStorage* storage,
                     int  header_size, int mode,
                     int  method, CvPoint offset )
{
    int y;
    int step;
    CvSize size;
    uchar *img = 0;
    CvContourScanner scanner = 0;

    if( !storage )
        CV_Error( CV_StsNullPtr, "" );

    CvMat stub, *mat = cvGetMat( _img, &stub );

    if( !CV_IS_MASK_ARR( mat ))
        CV_Error( CV_StsUnsupportedFormat, "[Start]FindContours support only 8uC1 images" );

    size = cvSize( mat->width, mat->height );
    step = mat->step;
    img = (uchar*)(mat->data.ptr);

    if( method < 0 || method > CV_CHAIN_APPROX_TC89_KCOS )
        CV_Error( CV_StsOutOfRange, "" );

    if( header_size < (int) (method == CV_CHAIN_CODE ? sizeof( CvChain ) : sizeof( CvContour )))
        CV_Error( CV_StsBadSize, "" );

    scanner = (CvContourScanner)cvAlloc( sizeof( *scanner ));
    memset( scanner, 0, sizeof( *scanner ));

    scanner->storage1 = scanner->storage2 = storage;
    scanner->img0 = (schar *) img;
    scanner->img = (schar *) (img + step);
    scanner->img_step = step;
    scanner->img_size.width = size.width - 1;   /* exclude rightest column */
    scanner->img_size.height = size.height - 1; /* exclude bottomost row */
    scanner->mode = mode;
    scanner->offset = offset;
    scanner->pt.x = scanner->pt.y = 1;
    scanner->lnbd.x = 0;
    scanner->lnbd.y = 1;
    scanner->nbd = 2;
    scanner->mode = (int) mode;
    scanner->frame_info.contour = &(scanner->frame);
    scanner->frame_info.is_hole = 1;
    scanner->frame_info.next = 0;
    scanner->frame_info.parent = 0;
    scanner->frame_info.rect = cvRect( 0, 0, size.width, size.height );
    scanner->l_cinfo = 0;
    scanner->subst_flag = 0;

    scanner->frame.flags = CV_SEQ_FLAG_HOLE;

    scanner->approx_method2 = scanner->approx_method1 = method;

    if( method == CV_CHAIN_APPROX_TC89_L1 || method == CV_CHAIN_APPROX_TC89_KCOS )
        scanner->approx_method1 = CV_CHAIN_CODE;

    if( scanner->approx_method1 == CV_CHAIN_CODE )
    {
        scanner->seq_type1 = CV_SEQ_CHAIN_CONTOUR;
        scanner->header_size1 = scanner->approx_method1 == scanner->approx_method2 ?
            header_size : sizeof( CvChain );
        scanner->elem_size1 = sizeof( char );
    }
    else
    {
        scanner->seq_type1 = CV_SEQ_POLYGON;
        scanner->header_size1 = scanner->approx_method1 == scanner->approx_method2 ?
            header_size : sizeof( CvContour );
        scanner->elem_size1 = sizeof( CvPoint );
    }

    scanner->header_size2 = header_size;

    if( scanner->approx_method2 == CV_CHAIN_CODE )
    {
        scanner->seq_type2 = scanner->seq_type1;
        scanner->elem_size2 = scanner->elem_size1;
    }
    else
    {
        scanner->seq_type2 = CV_SEQ_POLYGON;
        scanner->elem_size2 = sizeof( CvPoint );
    }

    scanner->seq_type1 = scanner->approx_method1 == CV_CHAIN_CODE ?
        CV_SEQ_CHAIN_CONTOUR : CV_SEQ_POLYGON;

    scanner->seq_type2 = scanner->approx_method2 == CV_CHAIN_CODE ?
        CV_SEQ_CHAIN_CONTOUR : CV_SEQ_POLYGON;

    cvSaveMemStoragePos( storage, &(scanner->initial_pos) );

    if( method > CV_CHAIN_APPROX_SIMPLE )
    {
        scanner->storage1 = cvCreateChildMemStorage( scanner->storage2 );
    }

    if( mode > CV_RETR_LIST )
    {
        scanner->cinfo_storage = cvCreateChildMemStorage( scanner->storage2 );
        scanner->cinfo_set = cvCreateSet( 0, sizeof( CvSet ), sizeof( _CvContourInfo ),
                                          scanner->cinfo_storage );
    }

    /* make zero borders */
    memset( img, 0, size.width );
    memset( img + step * (size.height - 1), 0, size.width );

    for( y = 1, img += step; y < size.height - 1; y++, img += step )
    {
        img[0] = img[size.width - 1] = 0;
    }

    /* converts all pixels to 0 or 1 */
    cvThreshold( mat, mat, 0, 1, CV_THRESH_BINARY );

    return scanner;
}

/*
   Final stage of contour processing.
   Three variants possible:
      1. Contour, which was retrieved using border following, is added to
         the contour tree. It is the case when the icvSubstituteContour function
         was not called after retrieving the contour.

      2. New contour, assigned by icvSubstituteContour function, is added to the
         tree. The retrieved contour itself is removed from the storage.
         Here two cases are possible:
            2a. If one deals with plane variant of algorithm
                (hierarchical strucutre is not reconstructed),
                the contour is removed completely.
            2b. In hierarchical case, the header of the contour is not removed.
                It's marked as "link to contour" and h_next pointer of it is set to
                new, substituting contour.

      3. The similar to 2, but when NULL pointer was assigned by
         icvSubstituteContour function. In this case, the function removes
         retrieved contour completely if plane case and
         leaves header if hierarchical (but doesn't mark header as "link").
      ------------------------------------------------------------------------
      The 1st variant can be used to retrieve and store all the contours from the image
      (with optional convertion from chains to contours using some approximation from
      restriced set of methods). Some characteristics of contour can be computed in the
      same pass.

      The usage scheme can look like:

      icvContourScanner scanner;
      CvMemStorage*  contour_storage;
      CvSeq*  first_contour;
      CvStatus  result;

      ...

      icvCreateMemStorage( &contour_storage, block_size/0 );

      ...

      cvStartFindContours
              ( img, contour_storage,
                header_size, approx_method,
                [external_only,]
                &scanner );

      for(;;)
      {
          [CvSeq* contour;]
          result = icvFindNextContour( &scanner, &contour/0 );

          if( result != CV_OK ) break;

          // calculate some characteristics
          ...
      }

      if( result < 0 ) goto error_processing;

      cvEndFindContours( &scanner, &first_contour );
      ...

      -----------------------------------------------------------------

      Second variant is more complex and can be used when someone wants store not
      the retrieved contours but transformed ones. (e.g. approximated with some
      non-default algorithm ).

      The scheme can be the as following:

      icvContourScanner scanner;
      CvMemStorage*  contour_storage;
      CvMemStorage*  temp_storage;
      CvSeq*  first_contour;
      CvStatus  result;

      ...

      icvCreateMemStorage( &contour_storage, block_size/0 );
      icvCreateMemStorage( &temp_storage, block_size/0 );

      ...

      icvStartFindContours8uC1R
              ( <img_params>, temp_storage,
                header_size, approx_method,
                [retrival_mode],
                &scanner );

      for(;;)
      {
          CvSeq* temp_contour;
          CvSeq* new_contour;
          result = icvFindNextContour( scanner, &temp_contour );

          if( result != CV_OK ) break;

          <approximation_function>( temp_contour, contour_storage,
                                    &new_contour, <parameters...> );

          icvSubstituteContour( scanner, new_contour );
          ...
      }

      if( result < 0 ) goto error_processing;

      cvEndFindContours( &scanner, &first_contour );
      ...

      ----------------------------------------------------------------------------
      Third method to retrieve contours may be applied if contours are irrelevant
      themselves but some characteristics of them are used only.
      The usage is similar to second except slightly different internal loop

      for(;;)
      {
          CvSeq* temp_contour;
          result = icvFindNextContour( &scanner, &temp_contour );

          if( result != CV_OK ) break;

          // calculate some characteristics of temp_contour

          icvSubstituteContour( scanner, 0 );
          ...
      }

      new_storage variable is not needed here.

      Note, that the second and the third methods can interleave. I.e. it is possible to
      retain contours that satisfy with some criteria and reject others.
      In hierarchic case the resulting tree is the part of original tree with
      some nodes absent. But in the resulting tree the contour1 is a child
      (may be indirect) of contour2 iff in the original tree the contour1
      is a child (may be indirect) of contour2.
*/
static void
icvEndProcessContour( CvContourScanner scanner )
{
    _CvContourInfo *l_cinfo = scanner->l_cinfo;

    if( l_cinfo )
    {
        if( scanner->subst_flag )
        {
            CvMemStoragePos temp;

            cvSaveMemStoragePos( scanner->storage2, &temp );

            if( temp.top == scanner->backup_pos2.top &&
                temp.free_space == scanner->backup_pos2.free_space )
            {
                cvRestoreMemStoragePos( scanner->storage2, &scanner->backup_pos );
            }
            scanner->subst_flag = 0;
        }

        if( l_cinfo->contour )
        {
            cvInsertNodeIntoTree( l_cinfo->contour, l_cinfo->parent->contour,
                                  &(scanner->frame) );
        }
        scanner->l_cinfo = 0;
    }
}

/* replaces one contour with another */
CV_IMPL void
cvSubstituteContour( CvContourScanner scanner, CvSeq * new_contour )
{
    _CvContourInfo *l_cinfo;

    if( !scanner )
        CV_Error( CV_StsNullPtr, "" );

    l_cinfo = scanner->l_cinfo;
    if( l_cinfo && l_cinfo->contour && l_cinfo->contour != new_contour )
    {
        l_cinfo->contour = new_contour;
        scanner->subst_flag = 1;
    }
}


/*
    marks domain border with +/-<constant> and stores the contour into CvSeq.
        method:
            <0  - chain
            ==0 - direct
            >0  - simple approximation
*/
static void
icvFetchContour( schar                  *ptr,
                 int                    step,
                 CvPoint                pt,
                 CvSeq*                 contour,
                 int    _method )
{
    const schar     nbd = 2;
    int             deltas[16];
    CvSeqWriter     writer;
    schar           *i0 = ptr, *i1, *i3, *i4 = 0;
    int             prev_s = -1, s, s_end;
    int             method = _method - 1;

    assert( (unsigned) _method <= CV_CHAIN_APPROX_SIMPLE );

    /* initialize local state */
    CV_INIT_3X3_DELTAS( deltas, step, 1 );
    memcpy( deltas + 8, deltas, 8 * sizeof( deltas[0] ));

    /* initialize writer */
    cvStartAppendToSeq( contour, &writer );

    if( method < 0 )
        ((CvChain *) contour)->origin = pt;

    s_end = s = CV_IS_SEQ_HOLE( contour ) ? 0 : 4;

    do
    {
        s = (s - 1) & 7;
        i1 = i0 + deltas[s];
        if( *i1 != 0 )
            break;
    }
    while( s != s_end );

    if( s == s_end )            /* single pixel domain */
    {
        *i0 = (schar) (nbd | -128);
        if( method >= 0 )
        {
            CV_WRITE_SEQ_ELEM( pt, writer );
        }
    }
    else
    {
        i3 = i0;
        prev_s = s ^ 4;

        /* follow border */
        for( ;; )
        {
            s_end = s;

            for( ;; )
            {
                i4 = i3 + deltas[++s];
                if( *i4 != 0 )
                    break;
            }
            s &= 7;

            /* check "right" bound */
            if( (unsigned) (s - 1) < (unsigned) s_end )
            {
                *i3 = (schar) (nbd | -128);
            }
            else if( *i3 == 1 )
            {
                *i3 = nbd;
            }

            if( method < 0 )
            {
                schar _s = (schar) s;

                CV_WRITE_SEQ_ELEM( _s, writer );
            }
            else
            {
                if( s != prev_s || method == 0 )
                {
                    CV_WRITE_SEQ_ELEM( pt, writer );
                    prev_s = s;
                }

                pt.x += icvCodeDeltas[s].x;
                pt.y += icvCodeDeltas[s].y;

            }

            if( i4 == i0 && i3 == i1 )
                break;

            i3 = i4;
            s = (s + 4) & 7;
        }                       /* end of border following loop */
    }

    cvEndWriteSeq( &writer );

    if( _method != CV_CHAIN_CODE )
        cvBoundingRect( contour, 1 );

    assert( (writer.seq->total == 0 && writer.seq->first == 0) ||
            writer.seq->total > writer.seq->first->count ||
            (writer.seq->first->prev == writer.seq->first &&
             writer.seq->first->next == writer.seq->first) );
}



/*
   trace contour until certain point is met.
   returns 1 if met, 0 else.
*/
static int
icvTraceContour( schar *ptr, int step, schar *stop_ptr, int is_hole )
{
    int deltas[16];
    schar *i0 = ptr, *i1, *i3, *i4;
    int s, s_end;

    /* initialize local state */
    CV_INIT_3X3_DELTAS( deltas, step, 1 );
    memcpy( deltas + 8, deltas, 8 * sizeof( deltas[0] ));

    assert( (*i0 & -2) != 0 );

    s_end = s = is_hole ? 0 : 4;

    do
    {
        s = (s - 1) & 7;
        i1 = i0 + deltas[s];
        if( *i1 != 0 )
            break;
    }
    while( s != s_end );

    i3 = i0;

    /* check single pixel domain */
    if( s != s_end )
    {
        /* follow border */
        for( ;; )
        {
            s_end = s;

            for( ;; )
            {
                i4 = i3 + deltas[++s];
                if( *i4 != 0 )
                    break;
            }

            if( i3 == stop_ptr || (i4 == i0 && i3 == i1) )
                break;

            i3 = i4;
            s = (s + 4) & 7;
        }                       /* end of border following loop */
    }
    return i3 == stop_ptr;
}


static void
icvFetchContourEx( schar*               ptr,
                   int                  step,
                   CvPoint              pt,
                   CvSeq*               contour,
                   int  _method,
                   int                  nbd,
                   CvRect*              _rect )
{
    int         deltas[16];
    CvSeqWriter writer;
    schar        *i0 = ptr, *i1, *i3, *i4;
    CvRect      rect;
    int         prev_s = -1, s, s_end;
    int         method = _method - 1;

    assert( (unsigned) _method <= CV_CHAIN_APPROX_SIMPLE );
    assert( 1 < nbd && nbd < 128 );

    /* initialize local state */
    CV_INIT_3X3_DELTAS( deltas, step, 1 );
    memcpy( deltas + 8, deltas, 8 * sizeof( deltas[0] ));

    /* initialize writer */
    cvStartAppendToSeq( contour, &writer );

    if( method < 0 )
        ((CvChain *)contour)->origin = pt;

    rect.x = rect.width = pt.x;
    rect.y = rect.height = pt.y;

    s_end = s = CV_IS_SEQ_HOLE( contour ) ? 0 : 4;

    do
    {
        s = (s - 1) & 7;
        i1 = i0 + deltas[s];
        if( *i1 != 0 )
            break;
    }
    while( s != s_end );

    if( s == s_end )            /* single pixel domain */
    {
        *i0 = (schar) (nbd | 0x80);
        if( method >= 0 )
        {
            CV_WRITE_SEQ_ELEM( pt, writer );
        }
    }
    else
    {
        i3 = i0;

        prev_s = s ^ 4;

        /* follow border */
        for( ;; )
        {
            s_end = s;

            for( ;; )
            {
                i4 = i3 + deltas[++s];
                if( *i4 != 0 )
                    break;
            }
            s &= 7;

            /* check "right" bound */
            if( (unsigned) (s - 1) < (unsigned) s_end )
            {
                *i3 = (schar) (nbd | 0x80);
            }
            else if( *i3 == 1 )
            {
                *i3 = (schar) nbd;
            }

            if( method < 0 )
            {
                schar _s = (schar) s;
                CV_WRITE_SEQ_ELEM( _s, writer );
            }
            else if( s != prev_s || method == 0 )
            {
                CV_WRITE_SEQ_ELEM( pt, writer );
            }

            if( s != prev_s )
            {
                /* update bounds */
                if( pt.x < rect.x )
                    rect.x = pt.x;
                else if( pt.x > rect.width )
                    rect.width = pt.x;

                if( pt.y < rect.y )
                    rect.y = pt.y;
                else if( pt.y > rect.height )
                    rect.height = pt.y;
            }

            prev_s = s;
            pt.x += icvCodeDeltas[s].x;
            pt.y += icvCodeDeltas[s].y;

            if( i4 == i0 && i3 == i1 )  break;

            i3 = i4;
            s = (s + 4) & 7;
        }                       /* end of border following loop */
    }

    rect.width -= rect.x - 1;
    rect.height -= rect.y - 1;

    cvEndWriteSeq( &writer );

    if( _method != CV_CHAIN_CODE )
        ((CvContour*)contour)->rect = rect;

    assert( (writer.seq->total == 0 && writer.seq->first == 0) ||
            writer.seq->total > writer.seq->first->count ||
            (writer.seq->first->prev == writer.seq->first &&
             writer.seq->first->next == writer.seq->first) );

    if( _rect )  *_rect = rect;
}


CvSeq *
cvFindNextContour( CvContourScanner scanner )
{
    schar *img0;
    schar *img;
    int step;
    int width, height;
    int x, y;
    int prev;
    CvPoint lnbd;
    int nbd;
    int mode;

    if( !scanner )
        CV_Error( CV_StsNullPtr, "" );
    icvEndProcessContour( scanner );

    /* initialize local state */
    img0 = scanner->img0;
    img = scanner->img;
    step = scanner->img_step;
    x = scanner->pt.x;
    y = scanner->pt.y;
    width = scanner->img_size.width;
    height = scanner->img_size.height;
    mode = scanner->mode;
    lnbd = scanner->lnbd;
    nbd = scanner->nbd;

    prev = img[x - 1];

    for( ; y < height; y++, img += step )
    {
        for( ; x < width; x++ )
        {
            int p = img[x];

            if( p != prev )
            {
                _CvContourInfo *par_info = 0;
                _CvContourInfo *l_cinfo = 0;
                CvSeq *seq = 0;
                int is_hole = 0;
                CvPoint origin;

                if( !(prev == 0 && p == 1) )    /* if not external contour */
                {
                    /* check hole */
                    if( p != 0 || prev < 1 )
                        goto resume_scan;

                    if( prev & -2 )
                    {
                        lnbd.x = x - 1;
                    }
                    is_hole = 1;
                }

                if( mode == 0 && (is_hole || img0[lnbd.y * step + lnbd.x] > 0) )
                    goto resume_scan;

                origin.y = y;
                origin.x = x - is_hole;

                /* find contour parent */
                if( mode <= 1 || (!is_hole && mode == 2) || lnbd.x <= 0 )
                {
                    par_info = &(scanner->frame_info);
                }
                else
                {
                    int lval = img0[lnbd.y * step + lnbd.x] & 0x7f;
                    _CvContourInfo *cur = scanner->cinfo_table[lval - 2];

                    assert( lval >= 2 );

                    /* find the first bounding contour */
                    while( cur )
                    {
                        if( (unsigned) (lnbd.x - cur->rect.x) < (unsigned) cur->rect.width &&
                            (unsigned) (lnbd.y - cur->rect.y) < (unsigned) cur->rect.height )
                        {
                            if( par_info )
                            {
                                if( icvTraceContour( scanner->img0 +
                                                     par_info->origin.y * step +
                                                     par_info->origin.x, step, img + lnbd.x,
                                                     par_info->is_hole ) > 0 )
                                    break;
                            }
                            par_info = cur;
                        }
                        cur = cur->next;
                    }

                    assert( par_info != 0 );

                    /* if current contour is a hole and previous contour is a hole or
                       current contour is external and previous contour is external then
                       the parent of the contour is the parent of the previous contour else
                       the parent is the previous contour itself. */
                    if( par_info->is_hole == is_hole )
                    {
                        par_info = par_info->parent;
                        /* every contour must have a parent
                           (at least, the frame of the image) */
                        if( !par_info )
                            par_info = &(scanner->frame_info);
                    }

                    /* hole flag of the parent must differ from the flag of the contour */
                    assert( par_info->is_hole != is_hole );
                    if( par_info->contour == 0 )        /* removed contour */
                        goto resume_scan;
                }

                lnbd.x = x - is_hole;

                cvSaveMemStoragePos( scanner->storage2, &(scanner->backup_pos) );

                seq = cvCreateSeq( scanner->seq_type1, scanner->header_size1,
                                   scanner->elem_size1, scanner->storage1 );
                seq->flags |= is_hole ? CV_SEQ_FLAG_HOLE : 0;

                /* initialize header */
                if( mode <= 1 )
                {
                    l_cinfo = &(scanner->cinfo_temp);
                    icvFetchContour( img + x - is_hole, step,
                                     cvPoint( origin.x + scanner->offset.x,
                                              origin.y + scanner->offset.y),
                                     seq, scanner->approx_method1 );
                }
                else
                {
                    union { _CvContourInfo* ci; CvSetElem* se; } v;
                    v.ci = l_cinfo;
                    cvSetAdd( scanner->cinfo_set, 0, &v.se );
                    l_cinfo = v.ci;

                    icvFetchContourEx( img + x - is_hole, step,
                                       cvPoint( origin.x + scanner->offset.x,
                                                origin.y + scanner->offset.y),
                                       seq, scanner->approx_method1,
                                       nbd, &(l_cinfo->rect) );
                    l_cinfo->rect.x -= scanner->offset.x;
                    l_cinfo->rect.y -= scanner->offset.y;

                    l_cinfo->next = scanner->cinfo_table[nbd - 2];
                    scanner->cinfo_table[nbd - 2] = l_cinfo;

                    /* change nbd */
                    nbd = (nbd + 1) & 127;
                    nbd += nbd == 0 ? 3 : 0;
                }

                l_cinfo->is_hole = is_hole;
                l_cinfo->contour = seq;
                l_cinfo->origin = origin;
                l_cinfo->parent = par_info;

                if( scanner->approx_method1 != scanner->approx_method2 )
                {
                    l_cinfo->contour = icvApproximateChainTC89( (CvChain *) seq,
                                                      scanner->header_size2,
                                                      scanner->storage2,
                                                      scanner->approx_method2 );
                    cvClearMemStorage( scanner->storage1 );
                }

                l_cinfo->contour->v_prev = l_cinfo->parent->contour;

                if( par_info->contour == 0 )
                {
                    l_cinfo->contour = 0;
                    if( scanner->storage1 == scanner->storage2 )
                    {
                        cvRestoreMemStoragePos( scanner->storage1, &(scanner->backup_pos) );
                    }
                    else
                    {
                        cvClearMemStorage( scanner->storage1 );
                    }
                    p = img[x];
                    goto resume_scan;
                }

                cvSaveMemStoragePos( scanner->storage2, &(scanner->backup_pos2) );
                scanner->l_cinfo = l_cinfo;
                scanner->pt.x = x + 1;
                scanner->pt.y = y;
                scanner->lnbd = lnbd;
                scanner->img = (schar *) img;
                scanner->nbd = nbd;
                return l_cinfo->contour;

            resume_scan:
                
                prev = p;
                /* update lnbd */
                if( prev & -2 )
                {
                    lnbd.x = x;
                }
            }                   /* end of prev != p */
        }                       /* end of loop on x */

        lnbd.x = 0;
        lnbd.y = y + 1;
        x = 1;
        prev = 0;

    }                           /* end of loop on y */

    return 0;
}


/*
   The function add to tree the last retrieved/substituted contour,
   releases temp_storage, restores state of dst_storage (if needed), and
   returns pointer to root of the contour tree */
CV_IMPL CvSeq *
cvEndFindContours( CvContourScanner * _scanner )
{
    CvContourScanner scanner;
    CvSeq *first = 0;

    if( !_scanner )
        CV_Error( CV_StsNullPtr, "" );
    scanner = *_scanner;

    if( scanner )
    {
        icvEndProcessContour( scanner );

        if( scanner->storage1 != scanner->storage2 )
            cvReleaseMemStorage( &(scanner->storage1) );

        if( scanner->cinfo_storage )
            cvReleaseMemStorage( &(scanner->cinfo_storage) );

        first = scanner->frame.v_next;
        cvFree( _scanner );
    }

    return first;
}


#define ICV_SINGLE                  0
#define ICV_CONNECTING_ABOVE        1
#define ICV_CONNECTING_BELOW        -1
#define ICV_IS_COMPONENT_POINT(val) ((val) != 0)

#define CV_GET_WRITTEN_ELEM( writer ) ((writer).ptr - (writer).seq->elem_size)

typedef  struct CvLinkedRunPoint
{
    struct CvLinkedRunPoint* link;
    struct CvLinkedRunPoint* next;
    CvPoint pt;
}
CvLinkedRunPoint;


static int
icvFindContoursInInterval( const CvArr* src,
                           /*int minValue, int maxValue,*/
                           CvMemStorage* storage,
                           CvSeq** result,
                           int contourHeaderSize )
{
    int count = 0;
    cv::Ptr<CvMemStorage> storage00;
    cv::Ptr<CvMemStorage> storage01;
    CvSeq* first = 0;

    int i, j, k, n;

    uchar*  src_data = 0;
    int  img_step = 0;
    CvSize  img_size;

    int  connect_flag;
    int  lower_total;
    int  upper_total;
    int  all_total;

    CvSeq*  runs;
    CvLinkedRunPoint  tmp;
    CvLinkedRunPoint*  tmp_prev;
    CvLinkedRunPoint*  upper_line = 0;
    CvLinkedRunPoint*  lower_line = 0;
    CvLinkedRunPoint*  last_elem;

    CvLinkedRunPoint*  upper_run = 0;
    CvLinkedRunPoint*  lower_run = 0;
    CvLinkedRunPoint*  prev_point = 0;

    CvSeqWriter  writer_ext;
    CvSeqWriter  writer_int;
    CvSeqWriter  writer;
    CvSeqReader  reader;

    CvSeq* external_contours;
    CvSeq* internal_contours;
    CvSeq* prev = 0;

    if( !storage )
        CV_Error( CV_StsNullPtr, "NULL storage pointer" );

    if( !result )
        CV_Error( CV_StsNullPtr, "NULL double CvSeq pointer" );

    if( contourHeaderSize < (int)sizeof(CvContour))
        CV_Error( CV_StsBadSize, "Contour header size must be >= sizeof(CvContour)" );

    storage00 = cvCreateChildMemStorage(storage);
    storage01 = cvCreateChildMemStorage(storage);

    CvMat stub, *mat;

    mat = cvGetMat( src, &stub );
    if( !CV_IS_MASK_ARR(mat))
        CV_Error( CV_StsBadArg, "Input array must be 8uC1 or 8sC1" );
    src_data = mat->data.ptr;
    img_step = mat->step;
    img_size = cvGetMatSize( mat );

    // Create temporary sequences
    runs = cvCreateSeq(0, sizeof(CvSeq), sizeof(CvLinkedRunPoint), storage00 );
    cvStartAppendToSeq( runs, &writer );

    cvStartWriteSeq( 0, sizeof(CvSeq), sizeof(CvLinkedRunPoint*), storage01, &writer_ext );
    cvStartWriteSeq( 0, sizeof(CvSeq), sizeof(CvLinkedRunPoint*), storage01, &writer_int );

    tmp_prev = &(tmp);
    tmp_prev->next = 0;
    tmp_prev->link = 0;

    // First line. None of runs is binded
    tmp.pt.y = 0;
    i = 0;
    CV_WRITE_SEQ_ELEM( tmp, writer );
    upper_line = (CvLinkedRunPoint*)CV_GET_WRITTEN_ELEM( writer );

    tmp_prev = upper_line;
    for( j = 0; j < img_size.width; )
    {
        for( ; j < img_size.width && !ICV_IS_COMPONENT_POINT(src_data[j]); j++ )
            ;
        if( j == img_size.width )
            break;

        tmp.pt.x = j;
        CV_WRITE_SEQ_ELEM( tmp, writer );
        tmp_prev->next = (CvLinkedRunPoint*)CV_GET_WRITTEN_ELEM( writer );
        tmp_prev = tmp_prev->next;

        for( ; j < img_size.width && ICV_IS_COMPONENT_POINT(src_data[j]); j++ )
            ;

        tmp.pt.x = j-1;
        CV_WRITE_SEQ_ELEM( tmp, writer );
        tmp_prev->next = (CvLinkedRunPoint*)CV_GET_WRITTEN_ELEM( writer );
        tmp_prev->link = tmp_prev->next;
        // First point of contour
        CV_WRITE_SEQ_ELEM( tmp_prev, writer_ext );
        tmp_prev = tmp_prev->next;
    }
    cvFlushSeqWriter( &writer );
    upper_line = upper_line->next;
    upper_total = runs->total - 1;
    last_elem = tmp_prev;
    tmp_prev->next = 0;

    for( i = 1; i < img_size.height; i++ )
    {
//------// Find runs in next line
        src_data += img_step;
        tmp.pt.y = i;
        all_total = runs->total;
        for( j = 0; j < img_size.width; )
        {
            for( ; j < img_size.width && !ICV_IS_COMPONENT_POINT(src_data[j]); j++ )
                ;
            if( j == img_size.width ) break;

            tmp.pt.x = j;
            CV_WRITE_SEQ_ELEM( tmp, writer );
            tmp_prev->next = (CvLinkedRunPoint*)CV_GET_WRITTEN_ELEM( writer );
            tmp_prev = tmp_prev->next;

            for( ; j < img_size.width && ICV_IS_COMPONENT_POINT(src_data[j]); j++ )
                ;

            tmp.pt.x = j-1;
            CV_WRITE_SEQ_ELEM( tmp, writer );
            tmp_prev = tmp_prev->next = (CvLinkedRunPoint*)CV_GET_WRITTEN_ELEM( writer );
        }//j
        cvFlushSeqWriter( &writer );
        lower_line = last_elem->next;
        lower_total = runs->total - all_total;
        last_elem = tmp_prev;
        tmp_prev->next = 0;
//------//
//------// Find links between runs of lower_line and upper_line
        upper_run = upper_line;
        lower_run = lower_line;
        connect_flag = ICV_SINGLE;

        for( k = 0, n = 0; k < upper_total/2 && n < lower_total/2; )
        {
            switch( connect_flag )
            {
            case ICV_SINGLE:
                if( upper_run->next->pt.x < lower_run->next->pt.x )
                {
                    if( upper_run->next->pt.x >= lower_run->pt.x  -1 )
                    {
                        lower_run->link = upper_run;
                        connect_flag = ICV_CONNECTING_ABOVE;
                        prev_point = upper_run->next;
                    }
                    else
                        upper_run->next->link = upper_run;
                    k++;
                    upper_run = upper_run->next->next;
                }
                else
                {
                    if( upper_run->pt.x <= lower_run->next->pt.x  +1 )
                    {
                        lower_run->link = upper_run;
                        connect_flag = ICV_CONNECTING_BELOW;
                        prev_point = lower_run->next;
                    }
                    else
                    {
                        lower_run->link = lower_run->next;
                        // First point of contour
                        CV_WRITE_SEQ_ELEM( lower_run, writer_ext );
                    }
                    n++;
                    lower_run = lower_run->next->next;
                }
                break;
            case ICV_CONNECTING_ABOVE:
                if( upper_run->pt.x > lower_run->next->pt.x +1 )
                {
                    prev_point->link = lower_run->next;
                    connect_flag = ICV_SINGLE;
                    n++;
                    lower_run = lower_run->next->next;
                }
                else
                {
                    prev_point->link = upper_run;
                    if( upper_run->next->pt.x < lower_run->next->pt.x )
                    {
                        k++;
                        prev_point = upper_run->next;
                        upper_run = upper_run->next->next;
                    }
                    else
                    {
                        connect_flag = ICV_CONNECTING_BELOW;
                        prev_point = lower_run->next;
                        n++;
                        lower_run = lower_run->next->next;
                    }
                }
                break;
            case ICV_CONNECTING_BELOW:
                if( lower_run->pt.x > upper_run->next->pt.x +1 )
                {
                    upper_run->next->link = prev_point;
                    connect_flag = ICV_SINGLE;
                    k++;
                    upper_run = upper_run->next->next;
                }
                else
                {
                    // First point of contour
                    CV_WRITE_SEQ_ELEM( lower_run, writer_int );

                    lower_run->link = prev_point;
                    if( lower_run->next->pt.x < upper_run->next->pt.x )
                    {
                        n++;
                        prev_point = lower_run->next;
                        lower_run = lower_run->next->next;
                    }
                    else
                    {
                        connect_flag = ICV_CONNECTING_ABOVE;
                        k++;
                        prev_point = upper_run->next;
                        upper_run = upper_run->next->next;
                    }
                }
                break;
            }
        }// k, n

        for( ; n < lower_total/2; n++ )
        {
            if( connect_flag != ICV_SINGLE )
            {
                prev_point->link = lower_run->next;
                connect_flag = ICV_SINGLE;
                lower_run = lower_run->next->next;
                continue;
            }
            lower_run->link = lower_run->next;

            //First point of contour
            CV_WRITE_SEQ_ELEM( lower_run, writer_ext );

            lower_run = lower_run->next->next;
        }

        for( ; k < upper_total/2; k++ )
        {
            if( connect_flag != ICV_SINGLE )
            {
                upper_run->next->link = prev_point;
                connect_flag = ICV_SINGLE;
                upper_run = upper_run->next->next;
                continue;
            }
            upper_run->next->link = upper_run;
            upper_run = upper_run->next->next;
        }
        upper_line = lower_line;
        upper_total = lower_total;
    }//i

    upper_run = upper_line;

    //the last line of image
    for( k = 0; k < upper_total/2; k++ )
    {
        upper_run->next->link = upper_run;
        upper_run = upper_run->next->next;
    }

//------//
//------//Find end read contours
    external_contours = cvEndWriteSeq( &writer_ext );
    internal_contours = cvEndWriteSeq( &writer_int );

    for( k = 0; k < 2; k++ )
    {
        CvSeq* contours = k == 0 ? external_contours : internal_contours;

        cvStartReadSeq( contours, &reader );

        for( j = 0; j < contours->total; j++, count++ )
        {
            CvLinkedRunPoint* p_temp;
            CvLinkedRunPoint* p00;
            CvLinkedRunPoint* p01;
            CvSeq* contour;

            CV_READ_SEQ_ELEM( p00, reader );
            p01 = p00;

            if( !p00->link )
                continue;

            cvStartWriteSeq( CV_SEQ_ELTYPE_POINT | CV_SEQ_POLYLINE | CV_SEQ_FLAG_CLOSED,
                             contourHeaderSize, sizeof(CvPoint), storage, &writer );
            do
            {
                CV_WRITE_SEQ_ELEM( p00->pt, writer );
                p_temp = p00;
                p00 = p00->link;
                p_temp->link = 0;
            }
            while( p00 != p01 );

            contour = cvEndWriteSeq( &writer );
            cvBoundingRect( contour, 1 );

            if( k != 0 )
                contour->flags |= CV_SEQ_FLAG_HOLE;

            if( !first )
                prev = first = contour;
            else
            {
                contour->h_prev = prev;
                prev = prev->h_next = contour;
            }
        }
    }

    if( !first )
        count = -1;

    if( result )
        *result = first;

    return count;
}



/*F///////////////////////////////////////////////////////////////////////////////////////
//    Name: cvFindContours
//    Purpose:
//      Finds all the contours on the bi-level image.
//    Context:
//    Parameters:
//      img  - source image.
//             Non-zero pixels are considered as 1-pixels
//             and zero pixels as 0-pixels.
//      step - full width of source image in bytes.
//      size - width and height of the image in pixels
//      storage - pointer to storage where will the output contours be placed.
//      header_size - header size of resulting contours
//      mode - mode of contour retrieval.
//      method - method of approximation that is applied to contours
//      first_contour - pointer to first contour pointer
//    Returns:
//      CV_OK or error code
//    Notes:
//F*/
CV_IMPL int
cvFindContours( void*  img,  CvMemStorage*  storage,
                CvSeq**  firstContour, int  cntHeaderSize,
                int  mode,
                int  method, CvPoint offset )
{
    CvContourScanner scanner = 0;
    CvSeq *contour = 0;
    int count = -1;

    if( !firstContour )
        CV_Error( CV_StsNullPtr, "NULL double CvSeq pointer" );
    
    *firstContour = 0;

    if( method == CV_LINK_RUNS )
    {
        if( offset.x != 0 || offset.y != 0 )
            CV_Error( CV_StsOutOfRange,
            "Nonzero offset is not supported in CV_LINK_RUNS yet" );

        count = icvFindContoursInInterval( img, storage, firstContour, cntHeaderSize );
    }
    else
    {
        try
        {
            scanner = cvStartFindContours( img, storage, cntHeaderSize, mode, method, offset );

            do
            {
                count++;
                contour = cvFindNextContour( scanner );
            }
            while( contour != 0 );
        }
        catch(...)
        {
            if( scanner )
                cvEndFindContours(&scanner);
            throw;
        }

        *firstContour = cvEndFindContours( &scanner );
    }

    return count;
}

namespace cv
{
static void
_findContours( Mat& image, vector<vector<Point> >& contours,
               vector<Vec4i>* hierarchy, int mode, int method, Point offset )
{
    MemStorage storage(cvCreateMemStorage());
    CvMat _image = image;
    CvSeq* _contours = 0;
    if( hierarchy )
        hierarchy->clear();
    cvFindContours(&_image, storage, &_contours, sizeof(CvContour), mode, method, offset);
    if( !_contours )
    {
        contours.clear();
        return;
    }
    Seq<CvSeq*> all_contours(cvTreeToNodeSeq( _contours, sizeof(CvSeq), storage ));
    size_t i, total = all_contours.size();
    contours.resize(total);
    SeqIterator<CvSeq*> it = all_contours.begin();
    for( i = 0; i < total; i++, ++it )
    {
        CvSeq* c = *it;
        ((CvContour*)c)->color = (int)i;
        Seq<Point>(c).copyTo(contours[i]);
    }

    if( hierarchy )
    {
        hierarchy->resize(total);
        it = all_contours.begin();
        for( i = 0; i < total; i++, ++it )
        {
            CvSeq* c = *it;
            int h_next = c->h_next ? ((CvContour*)c->h_next)->color : -1;
            int h_prev = c->h_prev ? ((CvContour*)c->h_prev)->color : -1;
            int v_next = c->v_next ? ((CvContour*)c->v_next)->color : -1;
            int v_prev = c->v_prev ? ((CvContour*)c->v_prev)->color : -1;
            (*hierarchy)[i] = Vec4i(h_next, h_prev, v_next, v_prev);
        }
    }
}
}

void cv::findContours( Mat& image, vector<vector<Point> >& contours,
                   vector<Vec4i>& hierarchy, int mode, int method, Point offset )
{
    _findContours(image, contours, &hierarchy, mode, method, offset);
}

void cv::findContours( Mat& image, vector<vector<Point> >& contours,
                   int mode, int method, Point offset)
{
    _findContours(image, contours, 0, mode, method, offset);
}

namespace cv
{

static void addChildContour(const vector<vector<Point> >& contours,
                            const vector<Vec4i>& hierarchy,
                            int i, vector<CvSeq>& seq,
                            vector<CvSeqBlock>& block)
{
    size_t count = contours.size();
    for( ; i >= 0; i = hierarchy[i][0] )
    {
        const vector<Point>& ci = contours[i];
        cvMakeSeqHeaderForArray(CV_SEQ_POLYGON, sizeof(CvSeq), sizeof(Point),
                                !ci.empty() ? (void*)&ci[0] : 0, (int)ci.size(),
                                &seq[i], &block[i] );
        
        int h_next = hierarchy[i][0], h_prev = hierarchy[i][1],
            v_next = hierarchy[i][2], v_prev = hierarchy[i][3];
        seq[i].h_next = (size_t)h_next < count ? &seq[h_next] : 0;
        seq[i].h_prev = (size_t)h_prev < count ? &seq[h_prev] : 0;
        seq[i].v_next = (size_t)v_next < count ? &seq[v_next] : 0;
        seq[i].v_prev = (size_t)v_prev < count ? &seq[v_prev] : 0;
        
        if( v_next >= 0 )
            addChildContour(contours, hierarchy, v_next, seq, block);
    }
}
    
}

void cv::drawContours( Mat& image, const vector<vector<Point> >& contours,
                   int contourIdx, const Scalar& color, int thickness,
                   int lineType, const vector<Vec4i>& hierarchy,
                   int maxLevel, Point offset )
{
    CvMat _image = image;

    size_t i = 0, first = 0, last = contours.size();
    vector<CvSeq> seq;
    vector<CvSeqBlock> block;

    seq.resize(last);
    block.resize(last);
    
    for( i = first; i < last; i++ )
        seq[i].first = 0;
                              
    if( contourIdx >= 0 )
    {
        CV_Assert( 0 <= contourIdx && contourIdx < (int)last );
        first = contourIdx;
        last = contourIdx + 1;
    }
    
    for( i = first; i < last; i++ )
    {
        const vector<Point>& ci = contours[i];
        cvMakeSeqHeaderForArray(CV_SEQ_POLYGON, sizeof(CvSeq), sizeof(Point),
            !ci.empty() ? (void*)&ci[0] : 0, (int)ci.size(), &seq[i], &block[i] );
    }

    if( hierarchy.empty() || maxLevel == 0 || contourIdx < 0 )
        for( i = first; i < last; i++ )
        {
            seq[i].h_next = i < last-1 ? &seq[i+1] : 0;
            seq[i].h_prev = i > first ? &seq[i-1] : 0;
        }
    else
    {
        size_t count = last - first;
        CV_Assert(hierarchy.size() == contours.size());
        if( count == contours.size() )
        {
            for( i = first; i < last; i++ )
            {
                int h_next = hierarchy[i][0], h_prev = hierarchy[i][1],
                    v_next = hierarchy[i][2], v_prev = hierarchy[i][3];
                seq[i].h_next = (size_t)h_next < count ? &seq[h_next] : 0;
                seq[i].h_prev = (size_t)h_prev < count ? &seq[h_prev] : 0;
                seq[i].v_next = (size_t)v_next < count ? &seq[v_next] : 0;
                seq[i].v_prev = (size_t)v_prev < count ? &seq[v_prev] : 0;
            }
        }
        else
        {
            int child = hierarchy[first][2];
            if( child >= 0 )
            {
                addChildContour(contours, hierarchy, child, seq, block);
                seq[first].v_next = &seq[child];
            }
        }
    }

    cvDrawContours( &_image, &seq[first], color, color, contourIdx >= 0 ?
                   -maxLevel : maxLevel, thickness, lineType, offset );
}

void cv::approxPolyDP( const Mat& curve, vector<Point>& approxCurve,
                       double epsilon, bool closed )
{
    CV_Assert(curve.isContinuous() && curve.depth() == CV_32S &&
              ((curve.rows == 1 && curve.channels() == 2) ||
               curve.cols*curve.channels() == 2));
    CvMat _curve = curve;
    MemStorage storage(cvCreateMemStorage());
    Seq<Point> seq(cvApproxPoly(&_curve, sizeof(CvContour), storage, CV_POLY_APPROX_DP, epsilon, closed));
    seq.copyTo(approxCurve);
}

void cv::approxPolyDP( const Mat& curve, vector<Point2f>& approxCurve,
                       double epsilon, bool closed )
{
    CV_Assert(curve.isContinuous() && curve.depth() == CV_32F &&
              ((curve.rows == 1 && curve.channels() == 2) ||
               curve.cols*curve.channels() == 2));
    CvMat _curve = curve;
    MemStorage storage(cvCreateMemStorage());
    Seq<Point2f> seq(cvApproxPoly(&_curve, sizeof(CvContour), storage, CV_POLY_APPROX_DP, epsilon, closed));
    seq.copyTo(approxCurve);
}

double cv::arcLength( const Mat& curve, bool closed )
{
    CV_Assert(curve.isContinuous() &&
              (curve.depth() == CV_32S || curve.depth() == CV_32F) &&
              ((curve.rows == 1 && curve.channels() == 2) ||
               curve.cols*curve.channels() == 2));
    CvMat _curve = curve;
    return cvArcLength(&_curve, CV_WHOLE_SEQ, closed);
}


cv::Rect cv::boundingRect( const Mat& points )
{
    CV_Assert(points.isContinuous() &&
              (points.depth() == CV_32S || points.depth() == CV_32F) &&
              ((points.rows == 1 && points.channels() == 2) ||
               points.cols*points.channels() == 2));
    CvMat _points = points;
    return cvBoundingRect(&_points, 0);
}


double cv::contourArea( const Mat& contour, bool oriented )
{
    CV_Assert(contour.isContinuous() &&
              (contour.depth() == CV_32S || contour.depth() == CV_32F) &&
              ((contour.rows == 1 && contour.channels() == 2) ||
               contour.cols*contour.channels() == 2));
    CvMat _contour = contour;
    return cvContourArea(&_contour, CV_WHOLE_SEQ, oriented);
}


cv::RotatedRect cv::minAreaRect( const Mat& points )
{
    CV_Assert(points.isContinuous() &&
              (points.depth() == CV_32S || points.depth() == CV_32F) &&
              ((points.rows == 1 && points.channels() == 2) ||
               points.cols*points.channels() == 2));
    CvMat _points = points;
    return cvMinAreaRect2(&_points, 0);
}


void cv::minEnclosingCircle( const Mat& points,
                             Point2f& center, float& radius )
{
    CV_Assert(points.isContinuous() &&
              (points.depth() == CV_32S || points.depth() == CV_32F) &&
              ((points.rows == 1 && points.channels() == 2) ||
               points.cols*points.channels() == 2));
    CvMat _points = points;
    cvMinEnclosingCircle( &_points, (CvPoint2D32f*)&center, &radius );
}


double cv::matchShapes( const Mat& contour1,
                        const Mat& contour2,
                        int method, double parameter )
{
    CV_Assert(contour1.isContinuous() && contour2.isContinuous() &&
              (contour1.depth() == CV_32S || contour1.depth() == CV_32F) &&
              contour1.depth() == contour2.depth() &&
              ((contour1.rows == 1 && contour1.channels() == 2 &&
                contour2.rows == 1 && contour2.channels() == 2) ||
               (contour1.cols*contour1.channels() == 2 &&
                contour2.cols*contour2.channels() == 2)));
    
    CvMat c1 = Mat(contour1), c2 = Mat(contour2);
    return cvMatchShapes(&c1, &c2, method, parameter);
}


void cv::convexHull( const Mat& points, vector<int>& hull, bool clockwise )
{
    CV_Assert(points.isContinuous() &&
              (points.depth() == CV_32S || points.depth() == CV_32F) &&
              ((points.rows == 1 && points.channels() == 2) ||
               points.cols*points.channels() == 2));
    hull.resize(points.cols*points.rows*points.channels()/2);
    CvMat _points = Mat(points), _hull=Mat(hull);
    cvConvexHull2(&_points, &_hull, clockwise ? CV_CLOCKWISE : CV_COUNTER_CLOCKWISE, 0);
    hull.resize(_hull.cols + _hull.rows - 1);
}


void cv::convexHull( const Mat& points,
                     vector<Point>& hull, bool clockwise )
{
    CV_Assert(points.isContinuous() && points.depth() == CV_32S &&
              ((points.rows == 1 && points.channels() == 2) ||
               points.cols*points.channels() == 2));
    hull.resize(points.cols*points.rows*points.channels()/2);
    CvMat _points = Mat(points), _hull=Mat(hull);
    cvConvexHull2(&_points, &_hull, clockwise ? CV_CLOCKWISE : CV_COUNTER_CLOCKWISE, 1);
    hull.resize(_hull.cols + _hull.rows - 1);
}


void cv::convexHull( const Mat& points,
                     vector<Point2f>& hull, bool clockwise )
{
    CV_Assert(points.isContinuous() && points.depth() == CV_32F &&
              ((points.rows == 1 && points.channels() == 2) ||
               points.cols*points.channels() == 2));
    hull.resize(points.cols*points.rows*points.channels()/2);
    CvMat _points = Mat(points), _hull=Mat(hull);
    cvConvexHull2(&_points, &_hull, clockwise ? CV_CLOCKWISE : CV_COUNTER_CLOCKWISE, 1);
    hull.resize(_hull.cols + _hull.rows - 1);
}

bool cv::isContourConvex( const Mat& contour )
{
    CV_Assert(contour.isContinuous() &&
              (contour.depth() == CV_32S || contour.depth() == CV_32F) &&
              ((contour.rows == 1 && contour.channels() == 2) ||
               contour.cols*contour.channels() == 2));
    CvMat c = Mat(contour);
    return cvCheckContourConvexity(&c) > 0;
}

cv::RotatedRect cv::fitEllipse( const Mat& points )
{
    CV_Assert(points.isContinuous() &&
              (points.depth() == CV_32S || points.depth() == CV_32F) &&
              ((points.rows == 1 && points.channels() == 2) ||
               points.cols*points.channels() == 2));
    CvMat _points = points;
    return cvFitEllipse2(&_points);
}


void cv::fitLine( const Mat& points, Vec4f& line, int distType,
                  double param, double reps, double aeps )
{
    CV_Assert(points.isContinuous() &&
              (points.depth() == CV_32S || points.depth() == CV_32F) &&
              ((points.rows == 1 && points.channels() == 2) ||
               points.cols*points.channels() == 2));
    CvMat _points = points;
    cvFitLine(&_points, distType, param, reps, aeps, &line[0]);
}


void cv::fitLine( const Mat& points, Vec6f& line, int distType,
                  double param, double reps, double aeps )
{
    CV_Assert(points.isContinuous() &&
              (points.depth() == CV_32S || points.depth() == CV_32F) &&
              ((points.rows == 1 && points.channels() == 3) ||
               points.cols*points.channels() == 3));
    CvMat _points = points;
    cvFitLine(&_points, distType, param, reps, aeps, &line[0]);
}

double cv::pointPolygonTest( const Mat& contour,
                             Point2f pt, bool measureDist )
{
    CV_Assert(contour.isContinuous() &&
              (contour.depth() == CV_32S || contour.depth() == CV_32F) &&
              ((contour.rows == 1 && contour.channels() == 2) ||
               contour.cols*contour.channels() == 2));
    CvMat c = Mat(contour);
    return cvPointPolygonTest( &c, pt, measureDist );
}

/* End of file. */
