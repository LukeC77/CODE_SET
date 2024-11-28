/*
 *     chroma.c
 * 
 *     Purpose: Implementation for chroma. Goes through each pixel of a 2D array
 *              using a mapping function to get the average of the chroma
 *              elements and store them in a sequence or update the chroma
 *              elements of the pixels. Relies on functions from arith40 to do
 *              the converison between floats and four-bit values. On the
 *              compression side, information is lost by storing only the
 *              average of the four chroma values.
 */

#include "chroma.h"
#include "assert.h"

typedef A2Methods_UArray2 A2;

typedef struct mycl {
        /* Sequence from compress40 to store CodeInfo_T structs */
        Seq_T code_info;
        float sumPb; /* Sum of Pb values of a 2-by-2 block */
        float sumPr; /* Sum of Pr values of a 2-by-2 block */
} *mycl;

void calcPbPr(int col, int row, A2 array2, A2Methods_Object *ptr, void *cl);
void populatePbPr(int col, int row, A2 array2, A2Methods_Object *ptr, void *cl);

/********** encodeChroma ********
 *
 * Purpose: Calculate the average of the chroma elements for all of the 2-by-2 
 *          block in a 2D array and then convert them to four-bit values.
 *
 * Parameters: 
 *      A2 array: The 2D array with component video values.
 *      Seq_T seq: A sequence to store a CodeInfo_T struct for each block.
 * 
 * Return: None.
 *      
 * Expects: The given 2D array contains pixels in component video color space.
 *          The sequence passed in has already been initialized and should be 
 *          empty.
 * 
 * Notes: The sequence is updated with the average chroma values converted to
 *        four-bit values. Utilizes an apply function to calculate the chroma 
 *        value averages of a block and populate the sequence. Information may
 *        be lost at this step because we only save the average of the chroma
 *        elements, thus making it impossible to determine the individual chroma
 *        values for each pixel when decompressing.
 *      
 ************************/
void encodeChroma(A2 arrayCV, Seq_T seq)
{
        assert(arrayCV != NULL && seq != NULL);
        A2Methods_T methods = uarray2_methods_blocked;
        assert(methods != NULL);
        /* Compute the averages of Pb and Pr for a block and then add to seq */
        struct mycl cl = {.code_info = seq, .sumPb = 0.0, .sumPr = 0.0};
        methods->map_block_major(arrayCV, calcPbPr, &cl);
        /* Converts chroma values from floats to 4 bit values and update the
           fields of the CodeInfo_T structs */
        for (int i = 0; i < Seq_length(seq); i++) {
                CodeInfo_T ct = Seq_get(seq, i);
                assert(ct != NULL);
                ct->Pb_index = Arith40_index_of_chroma(ct->Pb_avg);
                ct->Pr_index = Arith40_index_of_chroma(ct->Pr_avg);
        }
}

/********** decodeChroma ********
 *
 * Purpose: Convert the four-bit chroma codes to be the chroma value averages as
 *          floats. Populate the 2D array of Component_T structs with the
 *          resulting chroma vaues.
 *
 * Parameters: 
 *      A2 array: The 2D array meant to have component video values.
 *      Seq_T seq: A sequence containing CodeInfo_T structs for each code word.
 * 
 * Return: None.
 *      
 * Expects: The passed in sequence already contains CodeInfo_T structs whose
 *          Pb_index and Pr_index values are already defined.
 * 
 * Notes: Utilizes an apply function to update the chroma values for each
 *        element in the 2D array. Information is lost converting from 4-bit
 *        chroma codes to float chroma values.
 *      
 ************************/
void decodeChroma(A2 arrayCV, Seq_T seq)
{
        assert(arrayCV != NULL && seq != NULL);
        /* Converts 4-bit chroma codes to floats */
        for (int i = 0; i < Seq_length(seq); i++) {
                CodeInfo_T ct = Seq_get(seq, i);
                assert(ct != NULL);
                ct->Pb_avg = Arith40_chroma_of_index(ct->Pb_index);
                ct->Pr_avg = Arith40_chroma_of_index(ct->Pr_index);
        }     

        /* Populate the Pb and Pr values of each Component_T in the 2D array */
        A2Methods_T methods = uarray2_methods_blocked;
        assert(methods != NULL);
        methods->map_block_major(arrayCV, populatePbPr, seq);
}

/********** calcPbpr ********
 *
 * Purpose: Calculate the average Pb and Pr values for every block in a 2D
 *          array of Component_T structs.
 *
 * Parameters: 
 *      int col: Column index of the element
 *      int row: Row index of the element.
 *      A2 array2: The 2D array with component video pixels. Set to void.
 *      A2Methods_Object *ptr: Pointer to an element in the 2D array.
 *      void *cl: Pointer to the mycl struct.
 * 
 * Return: None.
 *      
 * Expects: The passed in ptr is a pointer to a Component_T element in arrayCV.
 *          cl points to struct mycl which contains the code_info sequence from
 *          compress40, meant to hold CodeInfo_T structs for each block.
 * 
 * Notes: The sumPb and sumPr accumulate values of Pb and Pr of visited pixels
 *        respectively. When the last pixel in a block is visited, compute the
 *        average of Pb and Pr and store the information in a new CodeInfo_T
 *        struct, which is then passed into the sequence. Allocates memory for
 *        CodeInfo_T structs that must be freed in the compress40 function.
 *        
 ************************/
void calcPbPr(int col, int row, A2 array2, A2Methods_Object *ptr, void *cl)
{
        (void) array2;
        Component_T pixel = ptr;
        mycl closure = cl;
        Seq_T code_info = closure->code_info;
        float *spb = &(closure->sumPb);
        float *spr = &(closure->sumPr);
        /* Add the Pb and Pr values to sumPb and sumPr respectively */
        *spb += pixel->Pb;
        *spr += pixel->Pr;

        /* Upon finding the last pixel of the block, calculate the averages */
        if (col % 2 == 1 && row % 2 == 1 && (col + row) % 2 == 0) {
                CodeInfo_T ct;
                NEW(ct);
                ct->Pb_avg = *spb / 4.0;
                ct->Pr_avg = *spr / 4.0;
                Seq_addhi(code_info, ct);
                /* Reset sumPb and sumPr to 0 for the next block */
                *spb = 0.0;
                *spr = 0.0;
        }
}

/********** populatePbpr ********
 *
 * Purpose: Update the chroma elements of the pixels for a 2D array.
 *
 * Parameters: 
 *      int col: Column index of the element
 *      int row: Row index of the element.
 *      A2 array2: The 2D array with component video pixels. Set to void.
 *      A2Methods_Object *ptr: Pointer to an element in the 2D array.
 *      void *cl: Pointer to the mycl struct.
 * 
 * Return: None.
 *      
 * Expects: The passed in sequence already contains CodeInfo_T structs whose
 *          Pb_avg and Pr_avg values are already defined.
 * 
 * Notes: The four pixels of any given block are updated with the same Pb and Pr
 *        values, equal to the average value. The block indices are equivalent
 *        to identifying the blocks in row-major order.
 *        
 ************************/
void populatePbPr(int col, int row, A2 array2, A2Methods_Object *ptr, void *cl)
{
        Seq_T seq = cl;
        A2Methods_T methods = uarray2_methods_blocked;
        assert(methods != NULL);
        Component_T pixel = ptr;
        /* The number of blocks in arrayCV is equal to the length of seq
         * The block is determined using the given indices of the element and
         * the CodeInfo_T pertaining to that block is obtained
         */
        unsigned w = methods->width(array2);
        unsigned blocks_in_row = w / 2;
        int block = (col / 2) + (row / 2) * blocks_in_row;
        CodeInfo_T ct = Seq_get(seq, block);
        assert(ct != NULL);
        pixel->Pb = ct-> Pb_avg;
        pixel->Pr = ct-> Pr_avg;
}