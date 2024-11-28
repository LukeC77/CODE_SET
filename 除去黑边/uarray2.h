/**************************************************************

 *     UArray2.h
 *
 *     Xiaoyan Xie (xxie05)
 *     Yifan Cao (ycao12)
 *     Sept 26 2024
 *
 *     CS 40 HW02 - iii
 *
 *     This header file defines the interface for a 2D unboxed array 
 *     (UArray2_T). It provides the necessary functions to create, access, 
 *     modify, and free a 2D array. The 2D array is implemented using 
 *     an underlying 1D unboxed array (UArray) where each element can 
 *     be accessed via its column and row indices. The array can be 
 *     traversed either in column-major or row-major order.
 *
 *
 **************************************************************/


#ifndef UARRAY2_H
#define UARRAY2_H

#include <stdlib.h>
#include <stdio.h>
#include <uarray.h>
#include "except.h"
#include <stdbool.h>

typedef struct UArray2_T *UArray2_T;

extern  const Except_T Mem_Failed;
extern const Except_T Out_Of_Bound;
extern const Except_T Invalid_Bound;
extern const Except_T Arr_NULL;
extern const Except_T Invalid_Size;



extern UArray2_T UArray2_new(int width, int height, int size);  



extern void* UArray2_at(UArray2_T array, int col, int row);


extern int UArray2_size(UArray2_T array);


extern int UArray2_width(UArray2_T array);



extern int UArray2_height(UArray2_T array);



extern void UArray2_map_col_major(UArray2_T array, 
                                  void apply(int column, 
                                             int row, UArray2_T a, 
                                             void *p1, 
                                             void *p2), 
                                  void *cl);


extern void UArray2_map_row_major(UArray2_T array, 
                                  void apply(int column, 
                                             int row, 
                                             UArray2_T a, 
                                             void *p1, 
                                             void *p2), 
                                  void *cl);


extern void UArray2_free(UArray2_T *array);


#undef UARRAY2_H

#endif
