/**************************************************************
 *
 *      uarray2.c
 *
 *      Xiaoyan Xie (xxie05)
 *      Yifan Cao (ycao12)
 *      Sept 26 2024
 *      
 *      CS 40 HW02 - iii
 *
 *      Implements the UArray2 interface, ensuring that all functions
 *      meet the  expectations set by the client.
 *
 *
 **************************************************************/
#include "uarray2.h"
#include <stdlib.h>
#include <stdio.h>
#include <uarray.h>
#include <assert.h>
#include <stdbool.h>

/********** UArray2_T Struct ********
 * A structure used to store variables. The width represents the number of
 * columns in a 2D array, the height represents the number of rows,
 * element_size specifies the size of each element, and Uarray_T is an
 * unboxed container that holds the array's data.
 *
 ************************/
struct UArray2_T {
    int width;          
    int height;         
    int size;           
    UArray_T outer;     
};

const Except_T Out_Of_Bound = {"Out_Of_Bound"};
const Except_T Invalid_Bound = {"Invalid Bound"};
const Except_T Arr_NULL = {"Input Array is NULL"};
const Except_T Invalid_Size = {"Size is nonpositive"};


/********** UArray2_new ********
 *
 * Return a 2D UArray implemented with array of arrays. The outer array stands
 * for the rows of the 2D array, storing pointers to arrays where elements are
 * stored.
 *
 * Parameters:
 *      int width:  The number of columns
 *      int height: The number of rows
 *      int size:   The size of the element that stores in the array
 *
 * Return:
 *      A new UArray2_T structure representing the 2D array.
 *
 * Expects:
 *      - width >= 0
 *      - height >= 0
 *      - size > 0
 *      - Memory is allocated successfully.
 *
 * Notes:
 *      - Checked runtime error if width/height is negative or size is
 *        nonpositive.
 *      - UArray new can raise Mem_Failed if memory allocation fails.
 *
 ************************/
extern UArray2_T UArray2_new(int width, int height, int size) 
{
        assert(width >= 0 && height >= 0 && size > 0); /* Check valid input */

        UArray2_T Array2D = malloc(sizeof(*Array2D));
        assert(Array2D != NULL);  

        Array2D->outer = UArray_new(height, sizeof(UArray_T));
        /* Check memory allocation for outer array */
        assert(Array2D->outer != NULL);  

        Array2D->width = width;
        Array2D->height = height;
        Array2D->size = size;

        for (int i = 0; i < height; i++) {
                UArray_T inner = UArray_new(width, size);
                /* Check memory allocation for inner arrays */
                assert(inner != NULL);  

                for (int j = 0; j < width; j++) {
                        int *curr_ele = UArray_at(inner, j);
                        *curr_ele = 0;
                }

                UArray_T *p_to_row = UArray_at(Array2D->outer, i);
                *p_to_row = inner;
        }

        return Array2D;
}

/********** UArray2_at ********
 *
 * Return the void pointer to the value of the (col, row) element.
 *
 * Parameters:
 *      UArray2_T array: The 2D array we want to manipulate
 *      int col:         Column index
 *      int row:         Row index
 *
 * Return:
 *      A void pointer to the element at the specified (col, row).
 *
 * Expects:
 *      - The array is initialized.
 *      - The col and row are within bounds and nonnegative.
 *
 * Notes:
 *      - Checked runtime error if col/row is negative or out of bounds.
 *
 ************************/
extern void* UArray2_at(UArray2_T array, int col, int row) 
{
        assert(array != NULL);  
        assert(col >= 0 && col < array->width);  /* Bounds checks */
        assert(row >= 0 && row < array->height);  

        UArray_T *inner_arr = UArray_at(array->outer, row);
        return UArray_at(*inner_arr, col);
}

/********** UArray2_size ********
 *
 * Return the size of each element stored in inner array
 *
 * Parameters:
 *      UArray2_T array: the 2D array whose size of element stored in it
 *                       is *required                     
 *
 * Return: The size of an element in the 2D array
 * Expects
 *      - The array is initialized and not NULL.
 *      - The size is not negative.
 * Notes:
 *      - If the array is uninitialized or NULL, or the size is negative, 
 *        there will be runtime error.      
 *
 ************************/
extern int UArray2_size(UArray2_T array)
{
        assert(array != NULL);
        return array->size;
}

/********** UArray2_width ********
 *
 * Return 2D array's width
 *
 * Parameters:
 *      UArray2_T array: The 2D UArray whose width we want to determine                      
 *
 * Return: The number of columns (width) in the 2D array
 * Expects
 *      - 
 *      - The `array` parameter is initialzed and not NULL.
 *      - The width is not negative and in bound

 * Notes:
 *      - If the array is uninitialized or NULL, or width is negative/out of
 *  bound, there will be runtime error.      
 *        
 ************************/
extern int UArray2_width(UArray2_T array)
{
        assert(array != NULL); 
        return array->width;
}

/********** UArray2_height ********
 *
 * Return 2D array's height
 *
 * Parameters:
 *      UArray2_T array: The 2D UArray whose height we want to determine                      
 *
 * Return: The number of rows (height) in the 2D array
 * Expects
 *      - The `array` parameter is initialzed and not NULL.
 *      - The height is not negative and in bound.
 * Notes:
 *      - If the array is uninitialized or NULL, or height is negative/out
 *       of bound, there will be runtime error.      
 *        
 ************************/
extern int UArray2_height(UArray2_T array)
{
        assert(array != NULL);  
        return array->height;
}

/********** UArray2_map_col_major ********
 *
 * Apply a function to each element of the 2D array in row-major order.
 *
 * Parameters:
 *      UArray2_T array: The 2d array we want to manipulate
 *      void apply(): The function to apply to all elements
 *      void *cl: The closure that passed to apply function                 
 *
 * Return: None
 * Expects
 *      - Array is initialzed and not NULL.
 *      
 * Notes:
 *      - If the array is uninitialized or NULL, 
 *        there will be runtime error.      
 *      - Test the function using Pnmrdr and diff to check 
 *        if the elements are stored in a col-major order.
 *      
 ************************/
extern void UArray2_map_col_major(UArray2_T array, void apply(int column, 
                int row, UArray2_T a, void *curr, void *cl), void *cl)
{
        assert(array != NULL);

        for (int j = 0; j < array->width; j++) {
                for (int i = 0; i < array->height; i++) {
                        UArray_T *inner_arr = UArray_at(array->outer, i);
                        void *curr = UArray_at(*inner_arr, j);
                        apply(j, i, array, curr, cl);
                }
        }
}

/********** UArray2_map_row_major ********
 *
 * Apply a function to each element of the 2D array in row-major order.
 *
 * Parameters:
 *      UArray2_T array: The 2d array we want to manipulate
 *      void apply(): The function to apply to all elements
 *      void *cl: The closure that passed to apply function                 
 *
 * Return: None
 * Expects
 *      - Array is initialzed and not NULL.
 *      
 * Notes:
 *      - If the array is uninitialized or NULL, 
 *        there will be runtime error.      
 *      - Test the function using Pnmrdr and diff to check 
 *        if the elements are stored in a row-major order.
 *      
 ************************/
extern void UArray2_map_row_major(UArray2_T array, void apply(int column,
                int row, UArray2_T a, void *curr, void *cl), void *cl) 
{
        assert(array != NULL);  

        for (int i = 0; i < array->height; i++) {
                UArray_T *inner_arr = UArray_at(array->outer, i);
                for (int j = 0; j < array->width; j++) {
                        void *curr = UArray_at(*inner_arr, j);
                        apply(j, i, array, curr, cl);
                }
        }
}

/********** UArray2_free ********
 *
 * This function will free the memory allocated for 2D array.
 *
 * Parameters:
 *       UArray2_T *array: pointer to the 2D array that need to be freed
 *
 * Return: None
 * Expects
 *       - Memory is freed and there is no memory leak 
 *       - Array is not NULL
 * Notes:
 *      - There will be runtime error if the array is NULL
 *      - Inner array will be freed first, 
 *        and then it will free the outer array of pointers.
 *      - Use valgrind to check if there is memory leak
 *      
 ************************/
extern void UArray2_free(UArray2_T *array)
{
        /* Check array is not null */
        assert(array != NULL && *array != NULL);  
        for (int i = 0; i < (*array)->height; i++) {
                UArray_T *inner_arr = UArray_at((*array)->outer, i);
                if (inner_arr != NULL) {
                        UArray_free(inner_arr);
                }
        }

        UArray_free(&(*array)->outer);

        free(*array);
        *array = NULL;
}