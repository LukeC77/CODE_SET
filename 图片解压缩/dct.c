/*
 *     dct.c
 * 
 *     Purpose: Implementation for dct. Defines two functions that allow
 *              for transformation between pixel space and DCT space. Utilizes
 *              two mapping functions to get the cosine coefficients and Y
 *              values for any given block.
 */

#include "dct.h"
#include "assert.h"

typedef A2Methods_UArray2 A2;

void calcDCT(int col, int row, A2 array2, A2Methods_Object *ptr, void *cl);
void calc4Y(int col, int row, A2 array2, A2Methods_Object *ptr, void *cl);
float roundAY(float x);
float roundBCD(float x);

/********** psToDCT ********
 *
 * Purpose: Use the discrete cosine transform to transform the Y values of a
 *          block of pixels into the cosine coefficients a, b, c, and d.
 *
 * Parameters: 
 *      A2 arrayCV: The 2D array with component video values.
 *      Seq_T seq: A sequence containing CodeInfo_T structs for each code word.
 * 
 * Return: None.
 *      
 * Expects: The 2D array and the sequence have both been initialized.
 * 
 * Notes: The sequence is updated with the cosine coefficients of each block.
 *        Utilizes an apply function to calculate the cosine coefficients of
 *        each block.
 *      
 ************************/
void psToDCT(A2 arrayCV, Seq_T seq)
{
        assert(arrayCV != NULL && seq != NULL);
        A2Methods_T methods = uarray2_methods_blocked;
        assert(methods != NULL);
        methods->map_block_major(arrayCV, calcDCT, seq);
}

/********** dctToPS ********
 *
 * Purpose: Use the inverse of the discrete cosine transform to compute the
 *          4 Y values of a block of pixels from the cosine coefficients.
 *
 * Parameters: 
 *      A2 arrayCV: The 2D array to hold component video values.
 *      Seq_T seq: The sequence pertaining to the code word to store the Y
 *                 values in.
 * 
 * Return: None.
 *      
 * Expects: The 2D array and the sequence have both been initialized.
 * 
 * Notes: The sequence is updated with the Y values of each block.
 *        Utilizes an apply function to calculate the Y values for each code
 *        word.
 *      
 ************************/
void dctToPS(A2 arrayCV, Seq_T seq)
{
        assert(arrayCV != NULL && seq != NULL);
        A2Methods_T methods = uarray2_methods_blocked;
        assert(methods != NULL);
        methods->map_block_major(arrayCV, calc4Y, seq);
}

/********** calcDCT ********
 *
 * Purpose: Calculate the cosine coefficients for a block.
 *
 * Parameters: 
 *      int col: Column index of the element
 *      int row: Row index of the element.
 *      A2 array2: The 2D array with component video pixels.
 *      A2Methods_Object *ptr: Pointer to an element in the 2D array.
 *      void *cl: Pointer to a sequence.
 * 
 * Return: None.
 *      
 * Expects: The array2 has been initialized and contains Component_T structs
 *          with defined Y values.
 * 
 * Notes: Binds the a, b, c, and d values to fit within a certain range, such
 *        that a is a 9 bit unsigned scaled integer and b, c, d are 5 bit
 *        signed scaled integers.
 *        
 ************************/
void calcDCT(int col, int row, A2 array2, A2Methods_Object *ptr, void *cl)
{
        /* Find the first pixel in a block */
        if (col % 2 == 0 && row % 2 == 0) {
                assert(array2 != NULL);
                A2Methods_T methods = uarray2_methods_blocked;
                assert(methods != NULL);
                /* Get the other three Component_T structs in the block */
                Component_T cp2 = methods->at(array2, col + 1, row);
                Component_T cp3 = methods->at(array2, col, row + 1);
                Component_T cp4 = methods->at(array2, col + 1, row + 1);
                float Y1 = ((Component_T)ptr)->Y;
                float Y2 = cp2->Y;
                float Y3 = cp3->Y;
                float Y4 = cp4->Y;
                unsigned w = methods->width(array2);
                unsigned blocks_in_row = w / 2;
                /* Find the index of the block using the given indices */
                int block = (col / 2) + (row / 2) * blocks_in_row;
                /* Get the CodeInfo_T struct associated with said block */
                CodeInfo_T ct = Seq_get((Seq_T)cl, block);
                assert(ct != NULL);
                /* Transform from pixel space to DCT
                   Code a in 9 unsigned bits and b, c, d in 5 signed bits */
                float a = roundAY((Y4 + Y3 + Y2 + Y1) / 4.0);
                float b = roundBCD((Y4 + Y3 - Y2 - Y1) / 4.0);
                float c = roundBCD((Y4 - Y3 + Y2 - Y1) / 4.0);
                float d = roundBCD((Y4 - Y3 - Y2 + Y1) / 4.0);
                ct->a = (unsigned) round(a * 63);
                ct->b = (int) round(b * 103);
                ct->c = (int) round(c * 103);
                ct->d = (int) round(d * 103);
        }
}

/********** calc4Y ********
 *
 * Purpose: Calculate the 4 Y values for a block.
 *
 * Parameters: 
 *      int col: Column index of the element
 *      int row: Row index of the element.
 *      A2 array2: The 2D array with component video pixels.
 *      A2Methods_Object *ptr: Pointer to an element in the 2D array.
 *      void *cl: Pointer to a sequence.
 * 
 * Return: None.
 *      
 * Expects: The sequence in the closure has been intialized and contains
 *          CodeInfo_T structs whose cosine coefficients are defined.
 * 
 * Notes: Binds the Y values to fit within the range 0 to 1.
 *        
 ************************/
void calc4Y(int col, int row, A2 array2, A2Methods_Object *ptr, void *cl)
{
        /* Find the first pixel in a block */
        if (col % 2 == 0 && row % 2 == 0) {
                assert(array2 != NULL);
                A2Methods_T methods = uarray2_methods_blocked;
                assert(methods !=  NULL);
                unsigned w = methods->width(array2);
                unsigned blocks_in_row = w / 2;
                /* Find the index of the block using the given indices */
                int block = (col / 2) + (row / 2) * blocks_in_row;
                /* Get the CodeInfo_T struct associated with said block */
                CodeInfo_T ct = Seq_get((Seq_T)cl, block);
                assert(ct != NULL);

                Component_T cp1 = ptr;
                Component_T cp2 = methods->at(array2, col + 1, row);
                Component_T cp3 = methods->at(array2, col, row + 1);
                Component_T cp4 = methods->at(array2, col + 1, row + 1);
                /* Convert a, b, cm and d from scaled integers to floats */
                float a = roundAY(ct->a / 63.0);
                float b = roundBCD(ct->b / 103.0);
                float c = roundBCD(ct->c / 103.0);
                float d = roundBCD(ct->d / 103.0);
                cp1->Y = roundAY(a - b - c + d);
                cp2->Y = roundAY(a - b + c - d);
                cp3->Y = roundAY(a + b - c - d);
                cp4->Y = roundAY(a + b + c + d);
        }
}

/********** roundAY ********
 *
 * Purpose: Bind the given float to fit within a certain range.
 *
 * Parameters: 
 *      float x: The cosine coefficient a or Y value being bound.
 *      
 * Return: None.
 *      
 * Expects: The passed in argument is a float.
 * 
 * Notes: Sets x to be 0 if it is less than 0, and sets x to be 1 if x is
 *        greater than 1.
 *      
 ************************/
float roundAY(float x)
{
        if (x <= 1 && x >= 0) {
                return x;
        } else if (x > 1) {
                return 1;
        } else {
                return 0;
        }        
}

/********** roundBCD ********
 *
 * Purpose: Bind the cosine coefficient a to fit within a certain range.
 *
 * Parameters: 
 *      float x: The cosine coefficient (b, c, or d) being bound.
 *      
 * Return: None.
 *      
 * Expects: The passed in argument is a float.
 * 
 * Notes: Sets x to be -0.3 if it is less than 0, and sets x to be 0.3 if x is
 *        greater than 0.3.
 *      
 ************************/
float roundBCD(float x) 
{
        if (x <= 0.3 && x >= -0.3) {
                return x;
        } else if (x > 0.3) {
                return 0.3;
        } else {
                return -0.3;
        }
}