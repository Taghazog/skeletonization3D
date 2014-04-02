/******************************************************************************
  Copyright (c) 2001-2004 by Turku PET Centre

  swap.c

  Byte swapping for little to big endian (and vice versa) conversion
  to be implemented in C programs.

  Written by Vesa Oikonen

  Based on free codes in web.

  2001-05-15 VO
  2002-01-20 VO
    Added new functions.
  2002-02-01 VO
    Change in swawbip(), no effect on results.
  2002-02-21 VO
    little_endian() algorithm changed.
  2002-08-23 VO
    Added function swawip().
    Also included function printf32bits() for testing purposes.
  2004-09-17 VO
    Doxygen style comments.


******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
/*****************************************************************************/
#include "trabecula/swap.hpp"
/*****************************************************************************/

/*****************************************************************************/
int little_endian()
{
  int x=1;
  if(*(char *)&x==1) return(1); else return(0);
}
/*****************************************************************************/

/*****************************************************************************/
void swap(void *from, void *to, int size) {
  unsigned char c;
  unsigned short int s;
  unsigned long l;

  switch(size) {
    case 1:
      *(char *)to=*(char *)from;
      break;
    case 2:
      c=*(unsigned char *)from;
      *(unsigned char *)to = *((unsigned char *)from+1);
      *((unsigned char *)to+1) = c;
      /*swab(from, to, size); // NOT ANSI */
      break;
    case 4:
      s=*(unsigned short *)from;
      *(unsigned short *)to = *((unsigned short *)from+1);
      *((unsigned short *)to+1) = s;
      swap((char*)to, (char*)to, 2);
      swap((char*)((unsigned short *)to+1), (char*)((unsigned short *)to+1), 2);
      break;
    case 8:
      l=*(unsigned long *)from;
      *(unsigned long *)to = *((unsigned long *)from+1);
      *((unsigned long *)to+1) = l;
      swap((char *)to, (char *)to, 4);
      swap((char*)((unsigned long *)to+1), (char*)((unsigned long *)to+1), 4);
      break;
  }
}
/*****************************************************************************/

/*****************************************************************************/
void swabip(void *buf, int size) {
  int i;
  unsigned char c;

  for(i=1; i<size; i+=2) {
    c=*((unsigned char *)buf+i);
    *((unsigned char *)buf+i)=*((unsigned char *)buf+(i-1));
    *((unsigned char *)buf+(i-1))=c;
  }
}
/*****************************************************************************/

/*****************************************************************************/
void swawbip(void *buf, int size) {
  int i;
  unsigned char c, *cptr;

  cptr=(unsigned char*)buf;
  for(i=0; i<size; i+=4, cptr+=4) {
    c=cptr[0]; cptr[0]=cptr[3]; cptr[3]=c;
    c=cptr[1]; cptr[1]=cptr[2]; cptr[2]=c;
  }
}
/*****************************************************************************/

/*****************************************************************************/
void swawip(void *buf, int size) {
  int i;
  unsigned short int s, *sptr;

  sptr=(unsigned short int*)buf;
  for(i=0; i<size; i+=4, sptr+=2) {
    s=sptr[0]; sptr[0]=sptr[1]; sptr[1]=s;
  }
}
/*****************************************************************************/

/*****************************************************************************/
void printf32bits(void *buf) {
  unsigned int u, i;
  int j;

  memcpy(&u, buf, 4);
  for(i=32; i>0; i--) {
    j=i-1;if(i<32 && (i%8)==0) printf(" ");
    if(u & (1L<<j)) printf("1"); else printf("0");
  }
  printf("\n");
}
/*****************************************************************************/

/*****************************************************************************/