/* rgb_io.h */

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
  short int magic;
  short int type;
  short int dim;
  short int xsize;
  short int ysize;
  short int zsize;
  int min;
  int max;
  int waste;
  char name[80];
  int space[102];
} RGB_HEADER;

void long_int_read ( FILE *fp, int *pr, int ys, int byte_swap );

int long_int_reverse_bytes ( int y );

void rgb_data_read ( FILE *fp, int byte_swap, int xs, int ys, 
  int pl, int type, unsigned char *r, unsigned char *g, 
  unsigned char *b );

void rgb_data_write ( FILE *fp, int byte_swap, int xs, int ys, 
  int pl, unsigned char *r, unsigned char *g, unsigned char *b,
  int type );

int rgb_header_read ( FILE *fp, int byte_swap, int *xs, 
  int *ys, int *pl, int *type );

void rgb_header_write ( FILE *fp, int byte_swap, int xs, 
  int ys, int pl, int type );

void rgb_rle_data_read ( FILE *fp, int byte_swap, int xs, 
  int ys, int pl, int type, unsigned char *r, 
  unsigned char *g, unsigned char *b );

void rgb_rle_data_write ( FILE *fp, int byte_swap, int xs, 
  int ys, int pl, unsigned char *r, unsigned char *g,
  unsigned char *b, int type );

short int short_int_reverse_bytes ( short int y );

void timestamp ( void );

#ifdef __cplusplus
}
#endif