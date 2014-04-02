/* ANALYZETM Header File Format
*
* (c) Copyright, 1986-1995
* Biomedical Imaging Resource
* Mayo Foundation
*
* dbh.h
*
* databse sub-definitions
*/
#ifndef _ANALYZE_H
#define _ANALYZE_H

#include <cstdio>
/*****************************************************************************/
#define ANALYZE_HEADER_KEY_SIZE 40
#define ANALYZE_HEADER_IMGDIM_SIZE 108
#define ANALYZE_HEADER_HISTORY_SIZE 200
/*****************************************************************************/
/* Analyze datatypes */
#define ANALYZE_DT_NONE 0
#define ANALYZE_DT_UNKNOWN 0
#define ANALYZE_DT_BINARY 1
#define ANALYZE_DT_UNSIGNED_CHAR 2
#define ANALYZE_DT_SIGNED_SHORT 4
#define ANALYZE_DT_SIGNED_INT 8
#define ANALYZE_DT_FLOAT 16
#define ANALYZE_DT_COMPLEX 32
#define ANALYZE_DT_DOUBLE 64
#define ANALYZE_DT_RGB 128
#define ANALYZE_DT_ALL 255
/*****************************************************************************/

/*****************************************************************************/
struct header_key  /* header key */
{ /* off + size */
    int sizeof_hdr; /* 0 + 4 */
    char data_type[10]; /* 4 + 10 */
    char db_name[18]; /* 14 + 18 */
    int extents; /* 32 + 4 */
    short int session_error; /* 36 + 2 */
    char regular; /* 38 + 1 */
    char hkey_un0; /* 39 + 1 */
} ; /* total=40 bytes */

struct image_dimension
{ /* off + size */
    short int dim[8]; /* 0 + 16 */
    short int unused8; /* 16 + 2 */
    short int unused9; /* 18 + 2 */
    short int unused10; /* 20 + 2 */
    short int unused11; /* 22 + 2 */
    short int unused12; /* 24 + 2 */
    short int unused13; /* 26 + 2 */
    short int unused14; /* 28 + 2 */
    short int datatype; /* 30 + 2 */
    short int bitpix; /* 32 + 2 */
    short int dim_un0; /* 34 + 2 */
    float pixdim[8]; /* 36 + 32 */
    /*
    pixdim[] specifies the voxel dimensitons:
    pixdim[1] - voxel width
    pixdim[2] - voxel height
    pixdim[3] - interslice distance
    ...etc
    */
    float vox_offset; /* 68 + 4 */
    float funused1; /* 72 + 4 */
    float funused2; /* 76 + 4 */
    float funused3; /* 80 + 4 */
    float cal_max; /* 84 + 4 */
    float cal_min; /* 88 + 4 */
    float compressed; /* 92 + 4 */
    float verified; /* 96 + 4 */
    int glmax,glmin; /* 100 + 8 */
} ; /* total=108 bytes */

struct data_history
{ /* off + size */
    char descrip[80]; /* 0 + 80 */
    char aux_file[24]; /* 80 + 24 */
    char orient; /* 104 + 1 */
    char originator[10]; /* 105 + 10 */
    char generated[10]; /* 115 + 10 */
    char scannum[10]; /* 125 + 10 */
    char patient_id[10]; /* 135 + 10 */
    char exp_date[10]; /* 145 + 10 */
    char exp_time[10]; /* 155 + 10 */
    char hist_un0[3]; /* 165 + 3 */
    int views; /* 168 + 4 */
    int vols_added; /* 172 + 4 */
    int start_field; /* 176 + 4 */
    int field_skip; /* 180 + 4 */
    int omax, omin; /* 184 + 8 */
    int smax, smin; /* 192 + 8 */
} ;

typedef struct
{
    struct header_key hk; /* 0 + 40 */
    struct image_dimension dime; /* 40 + 108 */
    struct data_history hist; /* 148 + 200 */
    int little;
} ANALYZE_DSR; /* total= 348 bytes */

typedef struct
{
 float real;
 float imag;
} COMPLEX;

/*****************************************************************************/
int anaReadHeader(const char *filename, ANALYZE_DSR *h);
int anaReadImagedata(const char *filename, const ANALYZE_DSR *h, int frame, char *data);
/*****************************************************************************/
int anaWriteHeader(const char *filename, const ANALYZE_DSR *h);
int anaWriteImagedata(const char *filename, const ANALYZE_DSR *h, const char *data);
/*****************************************************************************/
int anaPrintHeader(const ANALYZE_DSR *h, FILE *fp);
/*****************************************************************************/
#endif