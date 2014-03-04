/******************************************************************************

  Copyright (c) 2003-2007,2012 by Turku PET Centre

  analyze.h

  2003-10-05 VO
  2003-12-05 VO
  2004-09-20 VO
  2007-02-27 VO
  2012-02-04 VO


******************************************************************************/
#ifndef _ANALYZE_H
#define _ANALYZE_H


#ifdef __STRICT_ANSI__
char* strdup(const char* s);
int strcasecmp(char *s1, char *s2);
#endif

/*****************************************************************************/
/* Backup file extension */
#ifndef BACKUP_EXTENSION
#define BACKUP_EXTENSION ".bak"
#endif 
/*****************************************************************************/
#define ANALYZE_HEADER_KEY_SIZE 40
#define ANALYZE_HEADER_IMGDIM_SIZE 108
#define ANALYZE_HEADER_HISTORY_SIZE 200
/*****************************************************************************/
#define ANALYZE_FLIP_DEFAULT 1
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
typedef struct {
  int sizeof_hdr;
  char data_type[10];
  char db_name[18];
  int extents;
  short int session_error;
  char regular;
  char hkey_un0;
} ANALYZE_HEADER_KEY;

typedef struct {
  short int dim[8];
  short int unused8;
  short int unused9;
  short int unused10;
  short int unused11;
  short int unused12;
  short int unused13;
  short int unused14;
  short int datatype;
  short int bitpix;
  short int dim_un0;
  float pixdim[8];
  float vox_offset;
  float funused1;
  float funused2;
  float funused3;
  float cal_max;
  float cal_min;
  float compressed;
  float verified;
  int glmax;
  int glmin;
} ANALYZE_HEADER_IMGDIM;

typedef struct {
  char descrip[80];
  char aux_file[24];
  char orient;
  char originator[10];
  char generated[10];
  char scannum[10];
  char patient_id[10];
  char exp_date[10];
  char exp_time[10];
  char hist_un0[3];
  int views;
  int vols_added;
  int start_field;
  int field_skip;
  int omax;
  int omin;
  int smax;
  int smin;
} ANALYZE_HEADER_HISTORY;

typedef struct {
  ANALYZE_HEADER_KEY hk;
  ANALYZE_HEADER_IMGDIM dime;
  ANALYZE_HEADER_HISTORY hist;
  int little;
} ANALYZE_DSR;
/*****************************************************************************/
int anaExists(const char *dbname);
int anaExistsNew(
  const char *dbname, char *hdrfile, char *imgile, char *siffile);
int anaRemove(const char *dbname);
void anaRemoveFNameExtension(char *fname);
int anaDatabaseExists(
  const char *dbname, char *hdrfile, char *imgfile, char *siffile);
int anaMakeSIFName(const char *dbname, char *siffile);
/*****************************************************************************/
int anaFlipping();
/*****************************************************************************/
int anaReadHeader(const char *filename, ANALYZE_DSR *h);
int anaReadImagedata(const char *filename, ANALYZE_DSR *h, int frame, char *data);
/*****************************************************************************/
int anaWriteHeader(const char *filename, ANALYZE_DSR *h);
int anaWriteImagedata(const char *filename, ANALYZE_DSR *h, const char *data);
/*****************************************************************************/
int anaPrintHeader(ANALYZE_DSR *h, FILE *fp);
/*****************************************************************************/
#endif