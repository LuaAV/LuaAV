# include <math.h>
# include <stdio.h>
# include <stdlib.h>
# include <sys/types.h>
/*# include <unistd.h> */
# include <time.h>

# include "rgb_io.h"

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************/

void long_int_read ( FILE *fp, int *pr, int ys, int byte_swap )

/******************************************************************************/
/*
  Purpose:

    LONG_INT_READ reads ints.

  Modified:

    06 October 2000

  Author:

    John Burkardt

  Parameters:

    FILE *FP, the pointer to the file.

    int *PR, a pointer to the int data.

    int YS, the Y size, in pixels, of the image.

    int BYTE_SWAP, 0 for no byte swapping, 1 for byte swapping.
*/
{
  int i;
  int w;

  fread ( pr, 4, ys, fp );

  if ( byte_swap )
  {
    for ( i = 0; i < ys; i++ )
    {
      w = *pr;
      *pr = long_int_reverse_bytes ( w );
      pr++;
    }
  }
  return;
}
/******************************************************************************/

int long_int_reverse_bytes ( int x )

/******************************************************************************/
/*
  Purpose:

    LONG_INT_REVERSE_BYTES reverses the four bytes in a int.

  Modified:

    06 October 2000

  Author:

    John Burkardt

  Parameters:

    int X, the value whose bytes are to be reversed.

    int LONG_INT_REVERSE_BYTES, the value with bytes in reverse order.
*/
{
  char c;
  union
  {
    int ylongint;
    char ychar[4];
  } y;

  y.ylongint = x;
  
  c = y.ychar[0];
  y.ychar[0] = y.ychar[3];
  y.ychar[3] = c;

  c = y.ychar[1];
  y.ychar[1] = y.ychar[2];
  y.ychar[2] = c;

  return ( y.ylongint );
}
/******************************************************************************/
 
void rgb_data_read ( FILE *fp, int byte_swap, int xs, int ys, 
  int pl, int type, unsigned char *r, unsigned char *g, 
  unsigned char *b )

/******************************************************************************/
/*
  Purpose:

    RGB_DATA_READ reads the image data from an RGB file.

  Modified:

    05 October 2000

  Reference:

    Howard Burdick,
    Digital Imaging, Theory and Applications,
    McGraw Hill, 1997, page 248-251.

  Parameters:

    FILE *FP, the pointer to the file.

    int BYTE_SWAP, 0 for no byte swapping, 1 for byte swapping.

    int XS, YS, the X and Y sizes, in pixels, of the image.

    int PL, the number of planes of data.  PL is 1 for a black
    and white image, 3 for RGB, and 4 for RGB plus ALPHA.

    int TYPE, the file type.
    1, uncompressed data.
    257, RLE (run length encoded) data.

    unsigned char *R, *G, *B, the pixel data.
*/
{
  int c;
  unsigned char *d = 0;
  int y;
/* 
  Non-RLE character data.
*/
  for ( c = 0; c < pl; c++ )
  {
    if ( c == 0 )
    {
      d = r;
    }
    else if ( c == 1 )
    {
      d = g;
    }
    else if ( c == 2 )
    {
      d = b;
    }
    d = d + ( ( ys - 1 ) * xs );

    for ( y = 0; y < ys; y++ )
    {
      fread ( d, 1, xs, fp );
      d = d - xs;
    }

  }

  return;
}
/******************************************************************************/
    
void rgb_data_write ( FILE *fp, int byte_swap, int xs, int ys, 
  int pl, unsigned char *r, unsigned char *g, unsigned char *b, 
  int type )

/******************************************************************************/
/*
  Purpose:

    RGB_DATA_WRITE writes the image data to an RGB file.

  Modified:

    05 October 2000

  Reference:

    Howard Burdick,
    Digital Imaging, Theory and Applications,
    McGraw Hill, 1997, page 248.

  Parameters:

    FILE *FP, the pointer to the file.

    int BYTE_SWAP, 0 for no byte swapping, 1 for byte swapping.

    int XS, YS, the X and Y sizes, in pixels, of the image.

    int PL, the number of planes of data.  PL is 1 for a black
    and white image, 3 for RGB, and 4 for RGB plus ALPHA.

    unsigned char *R, *G, *B, the pixel data.

    int TYPE, the file type.
    1, uncompressed data.
    257, RLE (run length encoded) data.
*/
{
  int c;
  unsigned char *d = 0;
  int y;
/* 
  Non-RLE data.
*/

  for ( c = 0; c < pl; c++ )
  {
    if ( c == 0 )
    {
      d = r;
    }
    else if ( c == 1 )
    {
      d = g;
    }
    else if ( c == 2 )
    {
      d = b;
    }

    d = d + ( ( ys - 1 ) * xs );

    for ( y = 0; y < ys; y++ )
    {
      fwrite ( d, 1, xs, fp );
      d = d - xs;
    }
  }

  return;
}
/******************************************************************************/

int rgb_header_read ( FILE *fp, int byte_swap, int *xs, int *ys, 
  int *pl, int *type )

/******************************************************************************/
/*
  Purpose:

    RGB_HEADER_READ reads the header from an RGB file.

  Modified:

    06 November 2000

  Reference:

    Howard Burdick,
    Digital Imaging, Theory and Applications,
    McGraw Hill, 1997, page 248.

  Parameters:

    FILE *FP, the pointer to the file.

    int BYTE_SWAP, 0 for no byte swapping, 1 for byte swapping.

    int XS, YS, the X and Y sizes, in pixels, of the image.

    int PL, the number of planes of data.  PL is 1 for a black
    and white image, 3 for RGB, and 4 for RGB plus ALPHA.

    int TYPE, the file type.
    1, uncompressed data.
    257, RLE (run length encoded) data.
*/
{
  RGB_HEADER header;

  fread ( &header, sizeof ( header ), 1, fp );

  if ( byte_swap )
  {
    header.magic = short_int_reverse_bytes ( header.magic );
    header.type = short_int_reverse_bytes ( header.type );
    header.dim = short_int_reverse_bytes ( header.dim );
    header.xsize = short_int_reverse_bytes ( header.xsize );
    header.ysize = short_int_reverse_bytes ( header.ysize );
    header.zsize = short_int_reverse_bytes ( header.zsize );
    header.min = long_int_reverse_bytes ( header.min );
    header.max = long_int_reverse_bytes ( header.max );
    header.waste = long_int_reverse_bytes ( header.waste );
  }

  if ( header.magic != 474 )
  {
    printf ( "\n" );
    printf ( "RGB_HEADER_READ: Fatal error.\n" );
    printf ( "  Invalid magic number: %hd.\n", header.magic );
    printf ( "  Perhaps you need to reverse the byte swapping option.\n" );
    return 1;
  }

  *xs = ( int ) header.xsize;
  *ys = ( int ) header.ysize;
  *pl = ( int ) header.zsize;
  *type = ( int ) header.type;

  if ( *type == 1 ) 
  {
   /* printf ( "\n" );
    printf ( "RGB_HEADER_READ:\n" );
    printf ( "  Data is NOT run-length encoded.\n" ); */
  }
  else if ( *type == 257 )
  {
   /* printf ( "\n" );
    printf ( "RGB_HEADER_READ:\n" );
    printf ( "  Data is RLE (run-length encoded).\n" ); */
  }
  else
  {
    printf ( "\n" );
    printf ( "RGB_HEADER_READ: Fatal error.\n" );
    printf ( "  This routine can only read RGB types 1 and 257.\n" );
    printf ( "  This file has type %d.\n", header.type );
    return 1;
  }

  return 0;
}
/******************************************************************************/

void rgb_header_write ( FILE *fp, int byte_swap, int xs, int ys, 
  int pl, int type )

/******************************************************************************/
/*
  Purpose:

    RGB_HEADER_WRITE writes the header of an RGB file.

  Modified:

    05 October 2000

  Reference:

    Howard Burdick,
    Digital Imaging, Theory and Applications,
    McGraw Hill, 1997, page 248.

  Parameters:

    FILE *FP, the pointer to the file.

    int BYTE_SWAP, 0 for no byte swapping, 1 for byte swapping.

    int XS, YS, the X and Y sizes, in pixels, of the image.

    int PL, the number of planes of data.  PL is 1 for a black
    and white image, 3 for RGB, and 4 for RGB plus ALPHA.

    int TYPE, the file type.
    1, uncompressed data.
    257, RLE (run length encoded) data.
*/
{
  RGB_HEADER header;
  int i;

  header.magic = 474;
  header.type = type;
  header.dim = 3;
  header.xsize = xs;
  header.ysize = ys;
  header.zsize = pl;
  header.min = 0;
  header.max = 255;
  header.waste = 0;
                        
  for ( i = 0; i < 80; i++ )
  {
    header.name[i] = 0;
  }

  for ( i = 0; i < 102; i++ )
  {
    header.space[i] = 0;
  }

  if ( byte_swap )
  {
    header.magic = short_int_reverse_bytes ( header.magic );
    header.type = short_int_reverse_bytes ( header.type );
    header.dim = short_int_reverse_bytes ( header.dim );
    header.xsize = short_int_reverse_bytes ( header.xsize );
    header.ysize = short_int_reverse_bytes ( header.ysize );
    header.zsize = short_int_reverse_bytes ( header.zsize );
    header.min = long_int_reverse_bytes ( header.min );
    header.max = long_int_reverse_bytes ( header.max );
    header.waste = long_int_reverse_bytes ( header.waste );
  }

  fwrite ( &header, sizeof ( header ), 1, fp );

  return;
}
/******************************************************************************/
 
void rgb_rle_data_read ( FILE *fp, int byte_swap, int xs, int ys, 
  int pl, int type, unsigned char *r, unsigned char *g, 
  unsigned char *b )

/******************************************************************************/
/*
  Purpose:

    RGB_RLE_DATA_READ reads the image data from an RGB RLE file.

  Modified:

    05 October 2000

  Reference:

    Howard Burdick,
    Digital Imaging, Theory and Applications,
    McGraw Hill, 1997, page 248-251.

  Parameters:

    FILE *FP, the pointer to the file.

    int BYTE_SWAP, 0 for no byte swapping, 1 for byte swapping.

    int XS, YS, the X and Y sizes, in pixels, of the image.

    int PL, the number of planes of data.  PL is 1 for a black
    and white image, 3 for RGB, and 4 for RGB plus ALPHA.

    int TYPE, the file type.
    1, uncompressed data.
    257, RLE (run length encoded) data.

    unsigned char *R, *G, *B, the pixel data.
*/
{
  int c;
  int count;
  unsigned char *d = 0;
  unsigned char *in;
  unsigned char *out;
  int *p = 0;
  int *pb;
  int *pg;
  unsigned char pixel;
  int *po;
  int *pr;
  int *s = 0;
  int s2;
  int *sb;
  int *sg;
  unsigned char *si;
  int *so;
  int *sr;
  int y;
/* 
  Run-length encoded data.
  Allocate buffers for pointer and size tables.
*/
  pr = ( int * ) malloc ( ys*4 );
  pg = ( int * ) malloc ( ys*4 );
  pb = ( int * ) malloc ( ys*4 );
  po = ( int * ) malloc ( ys*4 );
  sr = ( int * ) malloc ( ys*4 );
  sg = ( int * ) malloc ( ys*4 );
  sb = ( int * ) malloc ( ys*4 );
  so = ( int * ) malloc ( ys*4 );
  si = ( unsigned char * ) malloc ( xs*4 );
/* 
  Skip past the 512 byte header.
*/
  fseek ( fp, 512, 0 );
/* 
  Read pointer and size tables.
*/
  long_int_read ( fp, pr, ys, byte_swap );

  if ( pl >= 3 )
  {
    long_int_read ( fp, pg, ys, byte_swap );
    long_int_read ( fp, pb, ys, byte_swap );
  }

  if ( pl == 4 )
  {
    long_int_read ( fp, po, ys, byte_swap );
  }

  long_int_read ( fp, sr, ys, byte_swap );

  if ( pl >= 3 )
  {    
    long_int_read ( fp, sg, ys, byte_swap );
    long_int_read ( fp, sb, ys, byte_swap );
  }

  if ( pl == 4 )
  {
    long_int_read ( fp, so, ys, byte_swap );
  }    
/* 
  Process each plane.
  Initialize buffer pointers.
*/
  for ( c = 0; c < pl; c++ )
  {
    switch ( c )
    {
      case 0:
        d = r;
        p = pr;
        s = sr;
        break;
      case 1:
        d = g;
        p = pg;
        s = sg;
        break;
      case 2:
        d = b;
        p = pb;
        s = sb;
        break;
    } 
/* 
  Process all lines. 
*/
    for ( y = 0; y < ys; y++ )
    { 
      out = d + ( ( ys - y - 1 ) * xs );

      fseek ( fp, p[y], 0 );
      fread ( si, 1, s[y], fp );
      in = si;
      s2 = 0; 
/* 
  Process all runs in the current line.
*/
      while ( s2 < s[y] )
      {
        pixel = in[s2++];
        count = ( ( int ) pixel & 0x0000007f );
        if ( count != 0 )
        {
          if ( ( pixel & 0x80 ) == 0 )
          {
            pixel = in[s2++];
            while ( count > 0 )
            { 
              *out++ = pixel;
              count--; 
            }
          }
          else
          {
            while ( count > 0 )
            {
              *out++ = in[s2++];
              count--;
            }
          }
        }
        else
        {
          s2 = s[y] + 1;
        }
      }
    }
  }
/* 
  Free the pointer and size tables. 
*/
  free ( pr );
  free ( pg );
  free ( pb );
  free ( po );
  free ( sr );
  free ( sg );
  free ( sb );
  free ( si );
  free ( so );

  return;
}
/******************************************************************************/
    
void rgb_rle_data_write ( FILE *fp, int byte_swap, int xs, int ys, 
  int pl, unsigned char *r, unsigned char *g, unsigned char *b, 
  int type )

/******************************************************************************/
/*
  Purpose:

    RGB_RLE_DATA_WRITE writes the image data to an RGB RLE file.

  Modified:

    05 October 2000

  Reference:

    Howard Burdick,
    Digital Imaging, Theory and Applications,
    McGraw Hill, 1997, page 248.

  Parameters:

    FILE *FP, the pointer to the file.

    int BYTE_SWAP, 0 for no byte swapping, 1 for byte swapping.

    int XS, YS, the X and Y sizes, in pixels, of the image.

    int PL, the number of planes of data.  PL is 1 for a black
    and white image, 3 for RGB, and 4 for RGB plus ALPHA.

    unsigned char *R, *G, *B, the pixel data.

    int TYPE, the file type.
    1, uncompressed data.
    257, RLE (run length encoded) data.
*/
{
  char           bcnt;
  unsigned char *c = 0;
  int       cc;
  int       cnt;
  unsigned char *d;
  int       f;
  int            i;
  unsigned char *o;
  int      *p = 0;
  int      *pb;
  int      *pg;
  int      *pr;
  int      *s = 0;
  int      *sb;
  int       scnt;
  int      *sg;
  unsigned char *so;
  int      *sr;
  int       x;
  int       y;
/* 
  Run-length encoded data.
  Allocate buffers for pointer and size tables.
*/
  pr = ( int * ) malloc (ys*4);
  pg = ( int * ) malloc (ys*4);
  pb = ( int * ) malloc (ys*4);
  sr = ( int * ) malloc (ys*4);
  sg = ( int * ) malloc (ys*4);
  sb = ( int * ) malloc (ys*4);
  so = ( unsigned char * ) malloc (xs*4); 
/* 
  Set pointer and size buffers to zero.
*/
  for ( y = 0; y < ys; y++ )
  {
    pr[y] = 0;
    pg[y] = 0;
    pb[y] = 0;
    sr[y] = 0;
    sg[y] = 0;
    sb[y] = 0;  
  }
/* 
  Write the zeroed pointer and size buffers to the file,
  just to save space for them.
*/
  fseek ( fp, 512, 0 );
  fwrite ( pr, 4, ys, fp );

  if ( pl >= 3 )
  {
    fwrite ( pg, 4, ys, fp );
    fwrite ( pb, 4, ys, fp );
  }  

  fwrite ( sr, 4, ys, fp );

  if ( pl >= 3 )
  {
    fwrite ( sg, 4, ys, fp );
    fwrite ( sb, 4, ys, fp );
  }  
/* 
  Process each plane, run-length encode all lines,
  updating pointer and size buffers. 
*/
  f = 512 + ( ( (ys*4) * pl ) * 2 );
/*
  Initialize buffer pointers.
*/
  for ( cc = 0; cc < pl; cc++ )
  {
    switch ( cc )
    {
      case 0:
        c = r;
        p = pr;
        s = sr;
        break;
      case 1:
        c = g;
        p = pg;
        s = sg;
        break;
      case 2:
        c = b;
        p = pb;
        s = sb;
        break;
    }
/* 
  Process all lines. 
*/
    for ( y = 0; y < ys; y++ )
    {
      d = c + ( ( ys - y - 1 ) * xs );
      p[y] = f;
      x = 0;
      scnt = 0;
/* 
  Process all pixels in current line. 
*/
      while ( x < xs )
      {
        o = so + 1;
/* 
  Special case: last pixel in line. 
*/
        if ( x == ( xs - 1 ) )
        {
          x++;
          *o = *d;
          *so = ( unsigned char ) 1;
          scnt = scnt + 2;
          fwrite ( so, 1, 2, fp );
        }
        else
        {
          if ( ( *d == *(d+1) ) && ( *d == *(d+2) ) )
          {
/* 
  Run of identical pixels. 
*/
            *o = *d;
            cnt = 1;  
            x++;
            while ( ( *d == *(d+1) ) && ( x < xs ) && ( cnt < 126 ) )
            {
              d++;
              cnt++;
              x++;
            }
            d++;
            *so = ( unsigned char ) cnt;
            scnt = scnt + 2;
            fwrite ( so, 1, 2, fp );
          }
          else
          {    
/* 
  Run of different pixels. 
*/
            cnt = 0;
            while (
              ( ( *d != *(d+1) ) ||
              ( ( *d == *(d+1) ) && ( *d != *(d+2) ) ) ) &&
              ( x < xs ) && ( cnt < 126 ) )
            {  
              *o++ = *d++;
              cnt++;
              x++;
            }    
            *so = ( unsigned char ) cnt + 0x80;
            scnt = scnt + cnt + 1;
            fwrite ( so, 1, (cnt+1), fp );  
          }
        }
      }
/* 
  This last byte must be 0 for compatibility with SGI utilities. 
*/
      bcnt = 0;
      fwrite ( &bcnt, 1, 1, fp );
      scnt++;
      s[y] = scnt;
      f = f + scnt;
    }
  }
/*
  Skip past the 512 byte header.
*/
  fseek ( fp, 512, 0 );
/* 
  Write out the completed pointer buffers. 
*/
  if ( byte_swap )
  {
    for ( i = 0; i < ys; i++ )
    {
      pr[i] = long_int_reverse_bytes ( pr[i] );
    }
  }

  fwrite ( pr, 4, ys, fp );

  if ( pl >= 3 )
  {
    if ( byte_swap )
    {
      for ( i = 0; i < ys; i++ )
      {
        pg[i] = long_int_reverse_bytes ( pg[i] );
      }
    }
    fwrite ( pg, 4, ys, fp );
    if ( byte_swap )
    {
      for ( i = 0; i < ys; i++ )
      {
        pb[i] = long_int_reverse_bytes ( pb[i] );
      }
    }
   fwrite ( pb, 4, ys, fp );
  }
/* 
  Write out the completed size buffers. 
*/
  if ( byte_swap )
  {
    for ( i = 0; i < ys; i++ )
    {
      sr[i] = long_int_reverse_bytes ( sr[i] );
    }
  }
  fwrite ( sr, 4, ys, fp );

  if ( pl >= 3 )
  {
    if ( byte_swap )
    {
      for ( i = 0; i < ys; i++ )
      {
        sg[i] = long_int_reverse_bytes ( sg[i] );
      }
    }
    fwrite ( sg, 4, ys, fp );
    if ( byte_swap )
    {
      for ( i = 0; i < ys; i++ )
      {
        sb[i] = long_int_reverse_bytes ( sb[i] );
      }
    }
    fwrite ( sb, 4, ys, fp );
  }
/* 
  Free the pointer buffers.
*/
  free ( pr );
  free ( pg );
  free ( pb );
/* 
  Free the size buffers.
*/
  free ( sr );
  free ( sg );
  free ( sb );
  free ( so );

  return;
}

/******************************************************************************/

short int short_int_reverse_bytes ( short int x )

/******************************************************************************/
/*
  Purpose:

    SHORT_INT_REVERSE_BYTES reverses the two bytes in a short int.

  Modified:

    06 October 2000

  Author:

    John Burkardt

  Parameters:

    short int X, a value whose bytes are to be reversed.

    short int SHORT_INT_REVERSE_BYTES, the value with bytes in reverse order.
*/
{
  char c;
  union
  {
    short int yshortint;
    char ychar[2];
  } y;

  y.yshortint = x;
  
  c = y.ychar[0];
  y.ychar[0] = y.ychar[1];
  y.ychar[1] = c;

  return ( y.yshortint );
}
/**********************************************************************/

void timestamp ( void )

/**********************************************************************/
/*
  Purpose:

    TIMESTAMP prints the current YMDHMS date as a time stamp.

  Example:

    May 31 2001 09:45:54 AM

  Modified:

    21 August 2002

  Author:

    John Burkardt

  Parameters:

    None
*/
{
#define TIME_SIZE 29

  static char time_buffer[TIME_SIZE];
  const struct tm *tm;
  size_t len;
  time_t now;

  now = time ( NULL );
  tm = localtime ( &now );

  len = strftime ( time_buffer, TIME_SIZE, "%d %B %Y %I:%M:%S %p", tm );

  if ( len != 0 ) 
  {
    printf ( "%s\n", time_buffer );
  }

  return;
#undef TIME_SIZE
}

#ifdef __cplusplus
}
#endif