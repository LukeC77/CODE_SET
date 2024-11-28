/**************************************************************

 *     bit2.h
 *
 *     Xiaoyan Xie (xxie05)
 *     Yifan Cao (ycao12)
 *     Sept 26 2024
 *
 *     CS 40 HW02 - iii
 *
 *     This header file defines the interface for a 2D bit array 
 *     that provides basic operations like creating, 
 *     accessing, modifying, and iterating over a 2D array 
 *     of bits. The array is implemented as an array of arrays, 
 *     using a 1D array structure (UArray_T) for each row of the 2D grid.
 *     
 *
 **************************************************************/

#ifndef BIT2_H
#define BIT2_H

#include <stdlib.h>
#include <stdio.h>
#include "uarray.h"
#include "except.h"
#include <stdbool.h>

extern const Except_T Mem_Failed;
extern const Except_T Beyond_Bound;
extern const Except_T Invalid_Bound;
extern const Except_T Arr_NULL;

typedef struct Bit2_T *Bit2_T;

Bit2_T Bit2_new(int width, int height); 
int Bit2_put(Bit2_T array, int column, int row, int elem);
int Bit2_get(Bit2_T array, int column, int row);
int Bit2_size(Bit2_T array);
int Bit2_width(Bit2_T array);
int Bit2_height(Bit2_T array);
void Bit2_map_col_major(Bit2_T array, void apply(
    int col, int row, Bit2_T a, int num, void *cl), void *cl);
void Bit2_map_row_major(Bit2_T array, void apply(
    int col, int row, Bit2_T a, int num, void *cl), void *cl);
void Bit2_free(Bit2_T *array);

#undef BIT2_H
#endif