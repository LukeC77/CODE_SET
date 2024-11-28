/*
 *     uarray2b.c
 * 
 *     Purpose: Implementation for uarray2b. Follows the interface for UArray2
 *              but utilizes blocking to improve locality. Defines the struct 
 *              for a UArray2b_T, which contains an outer UArray_T to store all
 *              of the blocks as well as information on its width, height, size,
 *              number of blocks, and blocksize. Defines many of the same
 *              functions as UArray2 but the mapping is done by blocks, not rows
 *              or columns. Relies on Hanson's interface of UArray_T for many of
 *              the finer details.
 */

#include <math.h>
#include "uarray.h"
#include "uarray2b.h"
#include <assert.h>
#include <mem.h>

#define T UArray2b_T
struct T {
        UArray_T outer;
        int width;
        int height;
        int num_blocks;
        int size;
        int blocksize;
};


/********** UArray2b_new ********
 *
 * Purpose: Allocates and initializes a UArray2b.
 *
 * Parameters: 
 *      int width: The width of the two-dimensional array.
 *      int height: The height of the two-dimensional array.
 *      int size: The number of bytes that each element occupies.
 *      int blocksize: The number of cells on one side of a block.
 * 
 * Return: The UArray2b that consists of blocked arrays.
 *      
 * Expects: Width and height are not negative (>= 0). Size and blocksize are
 *          both positive (> 0).
 * 
 * Notes: The width and height represent the dimensions of the drawn out
 *        UArray2b. However, it is actually represented as a 2D grid where the
 *        rows represent the blocks and the columns represent the cells within
 *        each block. Calculations are done using the width, height, and
 *        blocksize to figure out the number of blocks and cells.
 *      
 ************************/
T UArray2b_new (int width, int height, int size, int blocksize)
{
        int i;
        T array2b;
        assert(width >= 0 && height >= 0);
        assert(size >= 1);
        assert(blocksize >= 1);
        NEW(array2b);
        array2b->num_blocks = ((width + blocksize - 1) / blocksize) * \
                             ((height + blocksize - 1) / blocksize);
        int num_blocks = array2b->num_blocks;
        array2b->outer = UArray_new(num_blocks, sizeof(UArray_T));
        for (i = 0; i < num_blocks; i++) {
                UArray_T *blockp = UArray_at(array2b->outer, i);
                *blockp = UArray_new(blocksize * blocksize, size);
        }
        array2b->width = width;
        array2b->height = height;
        array2b->size = size;
        array2b->blocksize = blocksize;
        return array2b;
}

/********** UArray2b_new_64K_block ********
 *
 * Purpose: Create a UArray2b and default the blocksize to be as large as
 *          possible while allowing a block to fit in 64KB of RAM (where 1 KB
 *          is 1024 bytes).
 *
 * Parameters: 
 *      int width: The width of the two-dimensional array.
 *      int height: The height of the two-dimensional array.
 *      int size: The number of bytes that each element occupies.
 * 
 * Return: The UArray2b with a set default blocksize.
 *      
 * Expects: Width and height are not negative (>= 0). Size is positive (> 0).
 * 
 * Notes: Sets the blocksize to be 1 if a single cell will not fit in 64KB.
 *      
 ************************/
T UArray2b_new_64K_block(int width, int height, int size)
{
        assert(size >= 1);
        int max_blocksize = (int)sqrt(65536 / size);
        if (max_blocksize < 1) {
                max_blocksize = 1;
        }
        return UArray2b_new(width, height, size, max_blocksize);

}

/********** UArray2b_free ********
 *
 * Purpose: Deallocates and clears *array2b.
 *
 * Parameters:
 *      T *array2b: A pointer to a UArray2b.
 * 
 * Return: None.
 *
 * Expects: *arrayb and uarray2b are not null.
 * 
 * Notes: Frees all of the blocks before freeing the outer UArray. Raises a CRE
 *        if any of the UArray_T's (inner and outer) or their pointers are null.
 *      
 ************************/
void UArray2b_free (T *array2b)
{
        int i;
        assert(array2b && *array2b);
        for (i = 0; i < (*array2b)->num_blocks; i++) {
                UArray_T *block = UArray_at((*array2b)->outer, i);
                UArray_free(block);
        }
        UArray_free(&(*array2b)->outer);
        FREE(*array2b);
}

/********** UArray2b_width ********
 *
 * Purpose: Get the width of the two-dimensional array. 
 *
 * Parameters:
 *      T array2b: An instance of a UArray2b. 
 * 
 * Return: The width of the array2b.
 *
 * Expects: The width is nonnegative.
 * 
 * Notes: Hanson assertion catches if array2b is null.
 *      
 ************************/
int UArray2b_width (T array2b)
{
        assert(array2b);
        return array2b->width;
}

/********** UArray2b_height ********
 *
 * Purpose: Get the height of the two-dimensional array. 
 *
 * Parameters:
 *      T array2b: An instance of a UArray2b. 
 * 
 * Return: The height of the array2b.
 *
 * Expects: The height is nonnegative.
 * 
 * Notes: Hanson assertion catches if array2b is null.
 *      
 ************************/
int UArray2b_height (T array2b)
{
        assert(array2b);
        return array2b->height;
}

/********** UArray2b_size ********
 *
 * Purpose: Get the number of bytes that an element of array2b takes up.
 *
 * Parameters:
 *      T array2b: An instance of a UArray2b. 
 * 
 * Return: The size of elements in array2b.
 *
 * Expects: The size is greater than 0.
 * 
 * Notes: Hanson assertion catches if array2b is null.
 *      
 ************************/
int UArray2b_size (T array2b)
{
        assert(array2b);
        return array2b->size;
}

/********** UArray2b_blocksize ********
 *
 * Purpose: Get the number of cells on one side of a block in array2b.
 *
 * Parameters:
 *      T array2b: An instance of a UArray2b. 
 * 
 * Return: The blocksize of array2b.
 *
 * Expects: The blocksize is greater than 0.
 * 
 * Notes: Hanson assertion catches if array2b is null.
 *      
 ************************/
int UArray2b_blocksize(T array2b)
{
        assert(array2b);
        return array2b->blocksize;
}

/********** UArray2b_at ********
 *
 * Purpose: Provides access to elements within array2b.
 *
 * Parameters:
 *      T array2b: An instance of a UArray2b.
 *      int column: An integer respresenting a column in the 2D array. 
 *      int row: An integer representing a row in the 2D array. 
 * 
 * Return: A pointer to the element located at the given row and column. 
 *
 * Expects: The indices given for column and row are less than the width and
 *          height respectively, and greater than or equal to 0.
 * 
 * Notes: Raises a CRE if the column is greater than or equal to the width, or
 *        if the row is greater than or equqal to the height. Column and row
 *        must also be greater than or equal to 0. Accesses the element by
 *        determining the block based on the given indices, and then finds the
 *        cell within the block.
 *      
 ************************/
void *UArray2b_at(T array2b, int column, int row)
{
        assert(array2b);
        assert(column >= 0 && column < array2b->width);
        assert(row >= 0 && row < array2b->height);
        int blocksize = array2b->blocksize;
        int w = array2b->width;
        int blocks_in_row = (w + blocksize - 1) / blocksize; 
        /* Formulas for calculating the block index and cell index in row-major
           order */
        int block = blocks_in_row * (row / blocksize) + (column / blocksize);
        int cell = blocksize * (row % blocksize) + (column % blocksize);
        UArray_T *inner = UArray_at(array2b->outer, block);
        return UArray_at(*inner, cell);
}

/********** UArray2b_map ********
 *
 * Purpose: Traverses through a UArray2 in order of blocks and calls an apply
 *          function for each element.
 *
 * Parameters:
 *      T array2b: An instance of a UArray2.
 *      void apply(int col, int row, T array2b, void *elem, void *cl): A 
 *      function that can be called for an element in the array2b.
 *      *cl: An application specific pointer that gets passed along to apply
 *           at each call.
 * 
 * Return: None.
 *
 * Expects: The apply function passed in is meant to be called on every element.
 * 
 * Notes: Hanson assertion catches if array2b is null. Traverses through each
 *        block first, calculating the number of rows and columns for that block
 *        to avoid visiting any cells that are not used on the bottom and right
 *        edges. Goes through the cells of a block in row-major order.
 *      
 ************************/
void UArray2b_map(T array2b, void apply(int col, int row, T array2b, \
                  void *elem, void *cl), void *cl)
{
        assert(array2b);
        int num_blocks = array2b->num_blocks;
        int blocksize = array2b->blocksize;
        int w = array2b->width;
        int h = array2b->height;
        int blocks_in_row = (w + blocksize - 1) / blocksize; 
        int i;
        /* Loop through all of the blocks first */
        for (i = 0; i < num_blocks; i++) {
                UArray_T *block = UArray_at(array2b->outer, i);
                int block_row = i / blocks_in_row;
                int block_col = i % blocks_in_row;
                int j;
                int k;
                int num_rows;
                int num_cols;
                /* Calculate the number of rows and columns in the block, with
                   the blocks in the bottom row and right column using
                   h % blocksize and w % blocksize respectively  */
                if ((block_row + 1) * blocksize > h) {
                        num_rows = h % blocksize;
                } else {
                        num_rows = blocksize;
                }
                if ((block_col + 1) * blocksize > w) {
                        num_cols = w % blocksize;
                } else {
                        num_cols = blocksize;
                }
                /* Loop through each cell in row-major order */
                for (j = 0; j < num_rows; j++) {
                        for (k = 0; k < num_cols; k++) {
                                /* Calculate the column and row indices with
                                   respect to the whole UArray2b */
                                int col = block_col * blocksize + k;
                                int row = block_row * blocksize + j;
                                /* Assuming that UArray functions are quicker,
                                   but could also use UArray2b_at and pass in
                                   the col and row */
                                apply(col, row, array2b, \
                                      UArray_at(*block, j * blocksize + k), cl);
                        }
                }
        }
}

#undef T