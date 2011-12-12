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

/****************************************************************************************\
*                                       Watershed                                        *
\****************************************************************************************/

typedef struct CvWSNode
{
    struct CvWSNode* next;
    int mask_ofs;
    int img_ofs;
}
CvWSNode;

typedef struct CvWSQueue
{
    CvWSNode* first;
    CvWSNode* last;
}
CvWSQueue;

static CvWSNode*
icvAllocWSNodes( CvMemStorage* storage )
{
    CvWSNode* n = 0;
    
    int i, count = (storage->block_size - sizeof(CvMemBlock))/sizeof(*n) - 1;

    n = (CvWSNode*)cvMemStorageAlloc( storage, count*sizeof(*n) );
    for( i = 0; i < count-1; i++ )
        n[i].next = n + i + 1;
    n[count-1].next = 0;

    return n;
}


CV_IMPL void
cvWatershed( const CvArr* srcarr, CvArr* dstarr )
{
    const int IN_QUEUE = -2;
    const int WSHED = -1;
    const int NQ = 256;
    cv::Ptr<CvMemStorage> storage;
    
    CvMat sstub, *src;
    CvMat dstub, *dst;
    CvSize size;
    CvWSNode* free_node = 0, *node;
    CvWSQueue q[NQ];
    int active_queue;
    int i, j;
    int db, dg, dr;
    int* mask;
    uchar* img;
    int mstep, istep;
    int subs_tab[513];

    // MAX(a,b) = b + MAX(a-b,0)
    #define ws_max(a,b) ((b) + subs_tab[(a)-(b)+NQ])
    // MIN(a,b) = a - MAX(a-b,0)
    #define ws_min(a,b) ((a) - subs_tab[(a)-(b)+NQ])

    #define ws_push(idx,mofs,iofs)  \
    {                               \
        if( !free_node )            \
            free_node = icvAllocWSNodes( storage );\
        node = free_node;           \
        free_node = free_node->next;\
        node->next = 0;             \
        node->mask_ofs = mofs;      \
        node->img_ofs = iofs;       \
        if( q[idx].last )           \
            q[idx].last->next=node; \
        else                        \
            q[idx].first = node;    \
        q[idx].last = node;         \
    }

    #define ws_pop(idx,mofs,iofs)   \
    {                               \
        node = q[idx].first;        \
        q[idx].first = node->next;  \
        if( !node->next )           \
            q[idx].last = 0;        \
        node->next = free_node;     \
        free_node = node;           \
        mofs = node->mask_ofs;      \
        iofs = node->img_ofs;       \
    }

    #define c_diff(ptr1,ptr2,diff)      \
    {                                   \
        db = abs((ptr1)[0] - (ptr2)[0]);\
        dg = abs((ptr1)[1] - (ptr2)[1]);\
        dr = abs((ptr1)[2] - (ptr2)[2]);\
        diff = ws_max(db,dg);           \
        diff = ws_max(diff,dr);         \
        assert( 0 <= diff && diff <= 255 ); \
    }

    src = cvGetMat( srcarr, &sstub );
    dst = cvGetMat( dstarr, &dstub );

    if( CV_MAT_TYPE(src->type) != CV_8UC3 )
        CV_Error( CV_StsUnsupportedFormat, "Only 8-bit, 3-channel input images are supported" );

    if( CV_MAT_TYPE(dst->type) != CV_32SC1 )
        CV_Error( CV_StsUnsupportedFormat,
            "Only 32-bit, 1-channel output images are supported" );
    
    if( !CV_ARE_SIZES_EQ( src, dst ))
        CV_Error( CV_StsUnmatchedSizes, "The input and output images must have the same size" );

    size = cvGetMatSize(src);
    storage = cvCreateMemStorage();

    istep = src->step;
    img = src->data.ptr;
    mstep = dst->step / sizeof(mask[0]);
    mask = dst->data.i;

    memset( q, 0, NQ*sizeof(q[0]) );

    for( i = 0; i < 256; i++ )
        subs_tab[i] = 0;
    for( i = 256; i <= 512; i++ )
        subs_tab[i] = i - 256;

    // draw a pixel-wide border of dummy "watershed" (i.e. boundary) pixels
    for( j = 0; j < size.width; j++ )
        mask[j] = mask[j + mstep*(size.height-1)] = WSHED;

    // initial phase: put all the neighbor pixels of each marker to the ordered queue -
    // determine the initial boundaries of the basins
    for( i = 1; i < size.height-1; i++ )
    {
        img += istep; mask += mstep;
        mask[0] = mask[size.width-1] = WSHED;

        for( j = 1; j < size.width-1; j++ )
        {
            int* m = mask + j;
            if( m[0] < 0 ) m[0] = 0;
            if( m[0] == 0 && (m[-1] > 0 || m[1] > 0 || m[-mstep] > 0 || m[mstep] > 0) )
            {
                uchar* ptr = img + j*3;
                int idx = 256, t;
                if( m[-1] > 0 )
                    c_diff( ptr, ptr - 3, idx );
                if( m[1] > 0 )
                {
                    c_diff( ptr, ptr + 3, t );
                    idx = ws_min( idx, t );
                }
                if( m[-mstep] > 0 )
                {
                    c_diff( ptr, ptr - istep, t );
                    idx = ws_min( idx, t );
                }
                if( m[mstep] > 0 )
                {
                    c_diff( ptr, ptr + istep, t );
                    idx = ws_min( idx, t );
                }
                assert( 0 <= idx && idx <= 255 );
                ws_push( idx, i*mstep + j, i*istep + j*3 );
                m[0] = IN_QUEUE;
            }
        }
    }

    // find the first non-empty queue
    for( i = 0; i < NQ; i++ )
        if( q[i].first )
            break;

    // if there is no markers, exit immediately
    if( i == NQ )
        return;

    active_queue = i;
    img = src->data.ptr;
    mask = dst->data.i;

    // recursively fill the basins
    for(;;)
    {
        int mofs, iofs;
        int lab = 0, t;
        int* m;
        uchar* ptr;
        
        if( q[active_queue].first == 0 )
        {
            for( i = active_queue+1; i < NQ; i++ )
                if( q[i].first )
                    break;
            if( i == NQ )
                break;
            active_queue = i;
        }

        ws_pop( active_queue, mofs, iofs );

        m = mask + mofs;
        ptr = img + iofs;
        t = m[-1];
        if( t > 0 ) lab = t;
        t = m[1];
        if( t > 0 )
        {
            if( lab == 0 ) lab = t;
            else if( t != lab ) lab = WSHED;
        }
        t = m[-mstep];
        if( t > 0 )
        {
            if( lab == 0 ) lab = t;
            else if( t != lab ) lab = WSHED;
        }
        t = m[mstep];
        if( t > 0 )
        {
            if( lab == 0 ) lab = t;
            else if( t != lab ) lab = WSHED;
        }
        assert( lab != 0 );
        m[0] = lab;
        if( lab == WSHED )
            continue;

        if( m[-1] == 0 )
        {
            c_diff( ptr, ptr - 3, t );
            ws_push( t, mofs - 1, iofs - 3 );
            active_queue = ws_min( active_queue, t );
            m[-1] = IN_QUEUE;
        }
        if( m[1] == 0 )
        {
            c_diff( ptr, ptr + 3, t );
            ws_push( t, mofs + 1, iofs + 3 );
            active_queue = ws_min( active_queue, t );
            m[1] = IN_QUEUE;
        }
        if( m[-mstep] == 0 )
        {
            c_diff( ptr, ptr - istep, t );
            ws_push( t, mofs - mstep, iofs - istep );
            active_queue = ws_min( active_queue, t );
            m[-mstep] = IN_QUEUE;
        }
        if( m[mstep] == 0 )
        {
            c_diff( ptr, ptr + istep, t );
            ws_push( t, mofs + mstep, iofs + istep );
            active_queue = ws_min( active_queue, t );
            m[mstep] = IN_QUEUE;
        }
    }
}


void cv::watershed( const Mat& src, Mat& markers )
{
    CvMat _src = src, _markers = markers;
    cvWatershed( &_src, &_markers );
}


/****************************************************************************************\
*                                         Meanshift                                      *
\****************************************************************************************/

CV_IMPL void
cvPyrMeanShiftFiltering( const CvArr* srcarr, CvArr* dstarr, 
                         double sp0, double sr, int max_level,
                         CvTermCriteria termcrit )
{
    const int cn = 3;
    const int MAX_LEVELS = 8;
    cv::Ptr<CvMat> src_pyramid[MAX_LEVELS+1];
    cv::Ptr<CvMat> dst_pyramid[MAX_LEVELS+1];
    cv::Ptr<CvMat> mask0;
    int i, j, level;
    //uchar* submask = 0;

    #define cdiff(ofs0) (tab[c0-dptr[ofs0]+255] + \
        tab[c1-dptr[(ofs0)+1]+255] + tab[c2-dptr[(ofs0)+2]+255] >= isr22)

    memset( src_pyramid, 0, sizeof(src_pyramid) );
    memset( dst_pyramid, 0, sizeof(dst_pyramid) );
    
    double sr2 = sr * sr;
    int isr2 = cvRound(sr2), isr22 = MAX(isr2,16);
    int tab[768];
    CvMat sstub0, *src0;
    CvMat dstub0, *dst0;

    src0 = cvGetMat( srcarr, &sstub0 );
    dst0 = cvGetMat( dstarr, &dstub0 );

    if( CV_MAT_TYPE(src0->type) != CV_8UC3 )
        CV_Error( CV_StsUnsupportedFormat, "Only 8-bit, 3-channel images are supported" );
    
    if( !CV_ARE_TYPES_EQ( src0, dst0 ))
        CV_Error( CV_StsUnmatchedFormats, "The input and output images must have the same type" );

    if( !CV_ARE_SIZES_EQ( src0, dst0 ))
        CV_Error( CV_StsUnmatchedSizes, "The input and output images must have the same size" );

    if( (unsigned)max_level > (unsigned)MAX_LEVELS )
        CV_Error( CV_StsOutOfRange, "The number of pyramid levels is too large or negative" );

    if( !(termcrit.type & CV_TERMCRIT_ITER) )
        termcrit.max_iter = 5;
    termcrit.max_iter = MAX(termcrit.max_iter,1);
    termcrit.max_iter = MIN(termcrit.max_iter,100);
    if( !(termcrit.type & CV_TERMCRIT_EPS) )
        termcrit.epsilon = 1.f;
    termcrit.epsilon = MAX(termcrit.epsilon, 0.f);

    for( i = 0; i < 768; i++ )
        tab[i] = (i - 255)*(i - 255);

    // 1. construct pyramid
    src_pyramid[0] = src0;
    dst_pyramid[0] = dst0;
    for( level = 1; level <= max_level; level++ )
    {
        src_pyramid[level] = cvCreateMat( (src_pyramid[level-1]->rows+1)/2,
                        (src_pyramid[level-1]->cols+1)/2, src_pyramid[level-1]->type );
        dst_pyramid[level] = cvCreateMat( src_pyramid[level]->rows,
                        src_pyramid[level]->cols, src_pyramid[level]->type );
        cvPyrDown( src_pyramid[level-1], src_pyramid[level] );
        //CV_CALL( cvResize( src_pyramid[level-1], src_pyramid[level], CV_INTER_AREA ));
    }

    mask0 = cvCreateMat( src0->rows, src0->cols, CV_8UC1 );
    //CV_CALL( submask = (uchar*)cvAlloc( (sp+2)*(sp+2) ));

    // 2. apply meanshift, starting from the pyramid top (i.e. the smallest layer)
    for( level = max_level; level >= 0; level-- )
    {
        CvMat* src = src_pyramid[level];
        CvSize size = cvGetMatSize(src);
        uchar* sptr = src->data.ptr;
        int sstep = src->step;
        uchar* mask = 0;
        int mstep = 0;
        uchar* dptr;
        int dstep;
        float sp = (float)(sp0 / (1 << level));
        sp = MAX( sp, 1 );

        if( level < max_level )
        {
            CvSize size1 = cvGetMatSize(dst_pyramid[level+1]);
            CvMat m = cvMat( size.height, size.width, CV_8UC1, mask0->data.ptr );
            dstep = dst_pyramid[level+1]->step;
            dptr = dst_pyramid[level+1]->data.ptr + dstep + cn;
            mstep = m.step;
            mask = m.data.ptr + mstep;
            //cvResize( dst_pyramid[level+1], dst_pyramid[level], CV_INTER_CUBIC );
            cvPyrUp( dst_pyramid[level+1], dst_pyramid[level] );
            cvZero( &m );

            for( i = 1; i < size1.height-1; i++, dptr += dstep - (size1.width-2)*3, mask += mstep*2 )
            {
                for( j = 1; j < size1.width-1; j++, dptr += cn )
                {
                    int c0 = dptr[0], c1 = dptr[1], c2 = dptr[2];
                    mask[j*2 - 1] = cdiff(-3) || cdiff(3) || cdiff(-dstep-3) || cdiff(-dstep) ||
                        cdiff(-dstep+3) || cdiff(dstep-3) || cdiff(dstep) || cdiff(dstep+3);
                }
            }

            cvDilate( &m, &m, 0, 1 );
            mask = m.data.ptr;
        }

        dptr = dst_pyramid[level]->data.ptr;
        dstep = dst_pyramid[level]->step;

        for( i = 0; i < size.height; i++, sptr += sstep - size.width*3,
                                          dptr += dstep - size.width*3,
                                          mask += mstep )
        {   
            for( j = 0; j < size.width; j++, sptr += 3, dptr += 3 )
            {               
                int x0 = j, y0 = i, x1, y1, iter;
                int c0, c1, c2;

                if( mask && !mask[j] )
                    continue;

                c0 = sptr[0], c1 = sptr[1], c2 = sptr[2];

                // iterate meanshift procedure
                for( iter = 0; iter < termcrit.max_iter; iter++ )
                {
                    uchar* ptr;
                    int x, y, count = 0;
                    int minx, miny, maxx, maxy;
                    int s0 = 0, s1 = 0, s2 = 0, sx = 0, sy = 0;
                    double icount;
                    int stop_flag;

                    //mean shift: process pixels in window (p-sigmaSp)x(p+sigmaSp)
                    minx = cvRound(x0 - sp); minx = MAX(minx, 0);
                    miny = cvRound(y0 - sp); miny = MAX(miny, 0);
                    maxx = cvRound(x0 + sp); maxx = MIN(maxx, size.width-1);
                    maxy = cvRound(y0 + sp); maxy = MIN(maxy, size.height-1);
                    ptr = sptr + (miny - i)*sstep + (minx - j)*3; 

                    for( y = miny; y <= maxy; y++, ptr += sstep - (maxx-minx+1)*3 )
                    {
                        int row_count = 0;
                        x = minx;
                        for( ; x + 3 <= maxx; x += 4, ptr += 12 )
                        {
                            int t0 = ptr[0], t1 = ptr[1], t2 = ptr[2];
                            if( tab[t0-c0+255] + tab[t1-c1+255] + tab[t2-c2+255] <= isr2 )
                            {                        
                                s0 += t0; s1 += t1; s2 += t2;
                                sx += x; row_count++;
                            }
                            t0 = ptr[3], t1 = ptr[4], t2 = ptr[5];
                            if( tab[t0-c0+255] + tab[t1-c1+255] + tab[t2-c2+255] <= isr2 )
                            {                        
                                s0 += t0; s1 += t1; s2 += t2;
                                sx += x+1; row_count++;
                            }
                            t0 = ptr[6], t1 = ptr[7], t2 = ptr[8];
                            if( tab[t0-c0+255] + tab[t1-c1+255] + tab[t2-c2+255] <= isr2 )
                            {                        
                                s0 += t0; s1 += t1; s2 += t2;
                                sx += x+2; row_count++;
                            }
                            t0 = ptr[9], t1 = ptr[10], t2 = ptr[11];
                            if( tab[t0-c0+255] + tab[t1-c1+255] + tab[t2-c2+255] <= isr2 )
                            {                        
                                s0 += t0; s1 += t1; s2 += t2;
                                sx += x+3; row_count++;
                            }
                        }
                        
                        for( ; x <= maxx; x++, ptr += 3 )
                        {      
                            int t0 = ptr[0], t1 = ptr[1], t2 = ptr[2];
                            if( tab[t0-c0+255] + tab[t1-c1+255] + tab[t2-c2+255] <= isr2 )
                            {                        
                                s0 += t0; s1 += t1; s2 += t2;
                                sx += x; row_count++;
                            }
                        }
                        count += row_count;
                        sy += y*row_count;
                    }

                    if( count == 0 )
                        break;

                    icount = 1./count;
                    x1 = cvRound(sx*icount);
                    y1 = cvRound(sy*icount);
                    s0 = cvRound(s0*icount);
                    s1 = cvRound(s1*icount);
                    s2 = cvRound(s2*icount);

                    stop_flag = (x0 == x1 && y0 == y1) || abs(x1-x0) + abs(y1-y0) +
                        tab[s0 - c0 + 255] + tab[s1 - c1 + 255] +
                        tab[s2 - c2 + 255] <= termcrit.epsilon;
                
                    x0 = x1; y0 = y1;
                    c0 = s0; c1 = s1; c2 = s2;

                    if( stop_flag )
                        break;
                }

                dptr[0] = (uchar)c0;
                dptr[1] = (uchar)c1;
                dptr[2] = (uchar)c2;
            }
        }
    }
}

