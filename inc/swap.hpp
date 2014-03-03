/******************************************************************************
  Copyright (c) 2001,2002 by Turku PET Centre

  swap.h
  
  Versions:
  2002-02-21 Vesa Oikonen
  2002-08-23 VO

******************************************************************************/
#ifndef _SWAP_H
#define _SWAP_H
/*****************************************************************************/
extern int little_endian();
extern void swap(void *from, void *to, int size);
extern void swabip(void *buf, int size);
extern void swawbip(void *buf, int size);
extern void swawip(void *buf, int size);
/*****************************************************************************/
extern void printf32bits(void *buf);
/*****************************************************************************/
#endif