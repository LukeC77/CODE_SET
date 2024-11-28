/**************************************************************

 *     bit2.c
 *
 *     Xiaoyan Xie (xxie05)
 *     Yifan Cao (ycao12)
 *     Sept 26 2024
 *
 *     CS 40 HW02 - iii
 *
 *     Implementation for bit2.h     
 *
 **************************************************************/

#include "bit2.h"


/********** Bit2_T Struct ********
 * A structure used to store variables. The width represents the number of
 * columns in a 2D array, the height represents the number of rows,
 * and Uarray_T is an unboxed container that holds the array's data.
 *
 ************************/
struct Bit2_T {
        int width;
        int height;
        UArray_T outer; 
};

const Except_T Out_Of_Bound = {"Out_Of_Bound"};
const Except_T Invalid_Bound = {"Invalud Bound"};
const Except_T Arr_NULL = {"Input Array is NULL"};


/********** Bit2_new ********
 *
 * Creates a new 2D bit array with the given width and height.
 *
 * Parameters:
 *      int width:  The number of columns in the bit array.
 *      int height: The number of rows in the bit array.
 *
 * Return: A new 2D bit array (Bit2_T).
 *
 * Expects:
 *      width and height must be non-negative.
 *
 * Notes:
 *      Will raise Mem_Failed if memory allocation fails.
 *      Will raise Invalid_Bound if width or height is invalid.
 ************************/
Bit2_T Bit2_new(int width, int height)
{
        Bit2_T bitmap = malloc(sizeof(*bitmap));
        if (bitmap == NULL) {
                RAISE(Mem_Failed);
        }
        if (height <= 0) {
                RAISE(Invalid_Bound);
        }
        bitmap->outer = UArray_new(height, sizeof(UArray_T));
        bitmap->width = width;
        bitmap->height = height;
        for (int i = 0; i < height; i++) {
                if (width <= 0) {
                        RAISE(Invalid_Bound);
                }
                UArray_T inner = UArray_new(width, sizeof(int));

                for (int j = 0; j < width; j++) {
                        int *num = UArray_at(inner, j);
                        *num = 0;
                }

                UArray_T *p_to_i = UArray_at(bitmap->outer, i);
                *p_to_i = inner;
        }

        return bitmap;
}

/********** Bit2_put ********
 *
 * Sets the bit value at the specified column and row in the array.
 *
 * Parameters:
 *      Bit2_T array:  The 2D bit array pointer.
 *      int column:    The column index where the bit should be set.
 *      int row:       The row index where the bit should be set.
 *      int elem:      The new value to set (typically 0 or 1).
 *
 * Return: The previous value of the bit at (column, row).
 *
 * Expects:
 *      array must not be NULL.
 *      column and row must be within bounds of the array.
 *
 * Notes:
 *      Will raise Arr_NULL if the array is NULL.
 *      Will raise Out_Of_Bound if the column or row is invalid.
 ************************/
int Bit2_put(Bit2_T array, int column, int row, int elem)
{
        if (array == NULL) {
                RAISE(Arr_NULL);
        }
        if (row >= array->height || row < 0) {
                RAISE(Out_Of_Bound);
        }            
        UArray_T *arr = UArray_at(array->outer, row);

        if (column >= UArray_length(*arr) || column < 0) {
                RAISE(Out_Of_Bound);
        }     
        int *curr_bit = UArray_at(*arr, column);
  
        int pre_bit = *curr_bit;
        *curr_bit = elem;  
        return pre_bit;
}

/********** Bit2_get ********
 *
 * Retrieves the bit value at the specified column and row in the array.
 *
 * Parameters:
 *      Bit2_T array:  The 2D bit array pointer.
 *      int column:    The column index from which to retrieve the bit.
 *      int row:       The row index from which to retrieve the bit.
 *
 * Return: The value of the bit at (column, row).
 *
 * Expects:
 *      array must not be NULL.
 *      column and row must be within bounds of the array.
 *
 * Notes:
 *      Will raise Arr_NULL if the array is NULL.
 *      Will raise Out_Of_Bound if the column or row is invalid.
 ************************/
int Bit2_get(Bit2_T array, int column, int row)
{
        if (array == NULL) {
                RAISE(Arr_NULL);
        }
        if (row >= array->height || row < 0) {
                RAISE(Out_Of_Bound);
        }           
        UArray_T *arr = UArray_at(array->outer, row);

        if (column >= UArray_length(*arr) || column < 0) {
                RAISE(Out_Of_Bound);
        }     
        int *curr_bit = UArray_at(*arr, column);
   
        return *curr_bit;
}

/********** Bit2_size ********
 *
 * Returns the size (in bytes) of the internal structure of the array.
 *
 * Parameters:
 *      Bit2_T array:  The 2D bit array pointer.
 *
 * Return: The size in bytes of the bit array.
 *
 * Expects:
 *      array must not be NULL.
 *
 * Notes:
 *      Will raise Arr_NULL if the array is NULL.
 ************************/
int Bit2_size(Bit2_T array)
{
        if (array == NULL) {
                RAISE(Arr_NULL);
        }        
        UArray_T *first_entry = UArray_at(array->outer, 0);

        return UArray_size(*first_entry);
}

/********** Bit2_width ********
 *
 * Retrieves the width (number of columns) of the bit array.
 *
 * Parameters:
 *      Bit2_T array:  The 2D bit array pointer.
 *
 * Return: The width of the bit array.
 *
 * Expects:
 *      array must not be NULL.
 *
 * Notes:
 *      Will raise Arr_NULL if the array is NULL.
 ************************/
int Bit2_width(Bit2_T array)
{
        if (array == NULL) {
                RAISE(Arr_NULL);
        }
        return array->width;
}

/********** Bit2_height ********
 *
 * Retrieves the height (number of rows) of the bit array.
 *
 * Parameters:
 *      Bit2_T array:  The 2D bit array pointer.
 *
 * Return: The height of the bit array.
 *
 * Expects:
 *      array must not be NULL.
 *
 * Notes:
 *      Will raise Arr_NULL if the array is NULL.
 ************************/
int Bit2_height(Bit2_T array)
{
        if (array == NULL) {
                RAISE(Arr_NULL);
        }
        return array->height;
}

/********** Bit2_map_col_major ********
 *
 * Applies a function to every element of the array in column-major order.
 *
 * Parameters:
 *      Bit2_T array:  The 2D bit array pointer.
 *      void apply():  The function pointerto apply to each element.
 *      void *cl:      A closure or additional parameters passed to apply.
 *
 * Return: void
 *
 * Expects:
 *      array must not be NULL.
 *      apply function must be a valid function pointer.
 *
 * Notes:
 *      Will raise Arr_NULL if the array is NULL.
 ************************/
void Bit2_map_col_major(Bit2_T array, void apply(
    int col, int row, Bit2_T a, int curr_bit, void *cl), void *cl)
{
        if (array == NULL) {
                RAISE(Arr_NULL);
        }
        for (int j = 0; j < array->width; j++) {
                for (int i = 0; i < array->height; i++) {
                        UArray_T *inner_arr = UArray_at(array->outer, i);
                        int *curr_bit = UArray_at(*inner_arr, j);
                        apply(j, i, array, *curr_bit, cl);
                }
        }
}

/********** Bit2_map_row_major ********
 *
 * Applies a function to every element of the array in row-major order.
 *
 * Parameters:
 *      Bit2_T array:  The 2D bit array pointer.
 *      void apply():  The function pointer to apply to each element.
 *      void *cl:      A closure or additional parameters passed to apply.
 *
 * Return: void
 *
 * Expects:
 *      array must not be NULL.
 *      apply function must be a valid function pointer.
 *
 * Notes:
 *      Will raise Arr_NULL if the array is NULL.
 ************************/
void Bit2_map_row_major(Bit2_T array, void apply(
    int col, int row, Bit2_T a, int curr_bit, void *cl), void *cl)
{
        if (array == NULL) {
                RAISE(Arr_NULL);
        }
        for (int i = 0; i < array->height; i++) {
                UArray_T *inner_arr = UArray_at(array->outer, i);
                for (int j = 0; j < array->width; j++) {
                        int *curr_bit = UArray_at(*inner_arr, j);
                        apply(j, i, array, *curr_bit, cl);
                }
        }
}

/********** Bit2_free ********
 *
 * Frees the memory associated with the 2D bit array.
 *
 * Parameters:
 *      Bit2_T *array:  The pointer to the 2D bit array pointer.
 *
 * Return: void
 *
 * Expects:
 *      array must not be NULL, and *array must point to a valid Bit2_T.
 *
 * Notes:
 *      Will raise Arr_NULL if the array is NULL or *array is NULL.
 ************************/
void Bit2_free(Bit2_T *array)
{
        if (array == NULL || *array == NULL) {
                RAISE(Arr_NULL);
        }
        for (int i = 0; i < (*array)->height; i++) {
                UArray_T *inner_arr = UArray_at((*array)->outer, i);
                UArray_free(inner_arr);
        }
        UArray_T outer_p = (*array)->outer;
        UArray_free(&outer_p);
        free(*array);
}
