/**************************************************************

 *     process_black.h
 *
 *     Xiaoyan Xie (xxie05)
 *     Yifan Cao (ycao12)
 *     Sept 26 2024
 *
 *     CS 40 HW02 - iii
 *
 *     This header file defines the interface for reading, processing,
 *     and writing PBM  images using a 2D bit array Bit2_T). 
 *     The functions in this file include reading a PBM file,
 *     clearing the edges of the bitmap, and writing the processed bitmap 
 *     back to a file.
 *     
 *
 **************************************************************/


#ifndef PROCESS_BLACK_H
#define PROCESS_BLACK_H

#include "bit2.h"
#include "pnmrdr.h"
#include "seq.h"

extern const Except_T Width_Height_Error;

typedef struct Index_T *Index_T;

void process(FILE *inputfp);
Bit2_T pbmread (FILE *inputfp);
void populate(int col, int row, Bit2_T a, int num, void *cl);
void clear(Bit2_T array);
void clear_helper(Bit2_T array, Seq_T Q, int col, int row);
void check_neighbor(Bit2_T array, Seq_T Q, int col, int row);
void insert_to_Seq(Seq_T Q, int col, int row);
void pbmwrite(FILE *outputfp, Bit2_T bitmap);


#undef PROCESS_BLACK_H
#endif