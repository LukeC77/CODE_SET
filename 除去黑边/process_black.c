/**************************************************************

 *     process_black.c
 *
 *
 *     Implementation for process_black.h     
 *
 **************************************************************/

#include "process_black.h"

extern const Except_T Pnmrdr_Badformat;  
extern const Except_T Pnmrdr_Count;
const Except_T Width_Height_Error = {"Width or Height is 0"};
extern const Except_T File_Failed;

/********** Index_T Struct ********
 * A structure used to store index pair.
 * The col represents bit's column index.
 * The row represents bit's row index.
 *
 ************************/
struct Index_T {
        int col, row;
};

/********** process ********
 *
 * Reads a PBM image from the provided input file, clears the edges,
 * and then writes the resulting image to stdout.
 *
 * Parameters:
 *      FILE *inputfp: The input file pointer from which to 
                        read the PBM image.
 *
 * Return: void
 *
 * Expects
 *      inputfp must not be NULL.
 * Notes:
 *      Will CRE if inputfp is NULL or if reading the PBM fails.
 ************************/
void process(FILE *inputfp) 
{
        if (inputfp == NULL) {
                RAISE(File_Failed);
        }
        Bit2_T bitmap = pbmread(inputfp);

        clear(bitmap);

        pbmwrite(stdout, bitmap);

        Bit2_free(&bitmap);
}

/********** pbmread ********
 *
 * Reads a PBM image from the input file and 
 * stores the data in a 2D bit array (Bit2_T).
 *
 * Parameters:
 *      FILE *inputfp: The input file pointer from which 
                       to read the PBM image.
 *
 * Return: A Bit2_T structure representing the 2D bitmap image.
 *
 * Expects
 *      inputfp must not be NULL.
 *      The input file must be a valid PBM file with the P1 (plain) 
 *      or P4 (raw) format.
 *
 * Notes:
 *      Will CRE if the file is not a valid PBM image.
 *      Will raise Pnmrdr_Badformat for invalid PBM formatting.
 *      Will raise Width_Height_Error if the width or height is 0.
 ************************/
Bit2_T pbmread (FILE *inputfp) 
{
        if (inputfp == NULL) {
                RAISE(File_Failed);
        }
        Pnmrdr_T newpbm = Pnmrdr_new(inputfp);
        Pnmrdr_mapdata data = Pnmrdr_data(newpbm);

        if (data.type != Pnmrdr_bit) {
                RAISE(Pnmrdr_Badformat);
        }

        if (data.width == 0 || data.height == 0) {
                RAISE(Width_Height_Error);
        } 

        Bit2_T bitmap = Bit2_new(data.width, data.height);
        Bit2_map_row_major(bitmap, populate, newpbm);

        Pnmrdr_free(&newpbm);
        return bitmap;
}

/********** populate ********
 *
 * Populates the 2D bit array (Bit2_T) with data 
 * from the PBM image file.
 *
 * Parameters:
 *      int col:  The column index in the bit array.
 *      int row:  The row index in the bit array.
 *      Bit2_T a: The 2D bit array pointer.
 *      int num:  Unused in this function (always voided).
 *      void *cl: The context, in this case, a Pnmrdr_T reader object.
 *
 * Return: void
 *
 * Expects
 *      Bit2_T a must be valid.
 *      col and row must be within the dimensions of the bit array.
 *      The value read from the PBM must be 0 or 1.
 * Notes:
 *      Will raise Pnmrdr_Badformat if the pixel value is not 0 or 1.
 ************************/
void populate(int col, int row, Bit2_T a, int num, void *cl)
{
        (void) num;
        int insert_bit = Pnmrdr_get(cl);
        if (insert_bit != 0 && insert_bit != 1) {
                RAISE(Pnmrdr_Badformat);
        }
        Bit2_put(a, col, row, insert_bit);
}

/********** clear ********
 *
 * Clears the edges of a PBM image by setting any black pixel (value 1) 
 * on the edge and its connected neighbors to white (value 0).
 *
 * Parameters:
 *      Bit2_T array: The 2D bit array pointer 
 *                    representing the PBM image.
 *
 * Return: void
 *
 * Expects
 *      Bit2_T array must not be NULL and should contain valid bitmap data.
 * Notes:
 *      Will CRE if array is NULL or if the dimensions are invalid.
 ************************/
void clear(Bit2_T array)
{
        int width = Bit2_width(array);
        int height = Bit2_height(array);
        Seq_T Q = Seq_new(10);
        /*check the first line*/
        for (int j = 0, i = 0; j < width; j++) {
                if (Bit2_get(array, j, i) == 1) {
                        clear_helper(array, Q, j, i);
                }
        }
        /*check edges from 2nd to last - 1 line*/
        for (int i = 1; i < height - 1; i++) {
                for (int j = 0; j < width; j = j + width - 1) {
                        if (Bit2_get(array, j, i) == 1) {
                                clear_helper(array, Q, j, i);
                        }
                }
        }
        /*check last line*/
        for (int j = 0, i = height - 1; j < width; j++) {
                if (Bit2_get(array, j, i) == 1) {
                        clear_helper(array, Q, j, i);
                }
        }
        Seq_free(&Q);
}

/********** clear_helper ********
 *
 * Clears a specific pixel and its connected neighbors 
 *  recursively using BFS.
 *
 * Parameters:
 *      Bit2_T array: The 2D bit array representing the PBM image.
 *      Seq_T Q       The sequence pointer used to store the pixels 
 *                    to be processed.
 *      int col:      The column index of the pixel to clear.
 *      int row:      The row index of the pixel to clear.
 *
 * Return: void
 *
 * Expects
 *      Bit2_T array must not be NULL.
 *      col and row must be within the dimensions of the bit array.
 * Notes:
 *      Will CRE if array or Q is NULL or if col/row are 
 *      out of bounds.
 ************************/
void clear_helper(Bit2_T array, Seq_T Q, int col, int row)
{
        Bit2_put(array, col, row, 0);
        check_neighbor(array, Q, col, row);
        while (Seq_length(Q) != 0) {
                Index_T id = Seq_remlo(Q);
                Bit2_put(array, id->col, id->row, 0);   
                check_neighbor(array, Q, id->col, id->row);                         
                free(id);
        }        
}

/********** check_neighbor ********
 *
 * Checks the neighbors of the current pixel (col, row) and adds any 
 * neighboring black pixels (value 1) to the sequence for 
 * further processing.
 *
 * Parameters:
 *      Bit2_T array: The 2D bit array representing the PBM image.
 *      Seq_T Q       The sequence pointer used to store the pixels 
 *                    to be processed.
 *      int col:      The column index of the current pixel.
 *      int row:      The row index of the current pixel.
 *
 * Return: void
 *
 * Expects
 *      Bit2_T array must not be NULL.
 *      col and row must be within the dimensions of the bit array.
 * Notes:
 *      Will CRE if array or Q is NULL or if col/row are out of bounds.
 ************************/
void check_neighbor(Bit2_T array, Seq_T Q, int col, int row)
{
        int width = Bit2_width(array);
        int height = Bit2_height(array);   
        int j = col;
        int i = row;     
        if (j - 1 >= 0) {
                int next_bit = Bit2_get(array, j - 1, i);
                if (next_bit == 1 && next_bit != 2) {
                        /*blackedge will be set to 2 as a mark*/
                        Bit2_put(array, j - 1, i, 2);
                        insert_to_Seq(Q, j - 1, i);
                }
        }
        if (j + 1 < width) {
                int next_bit = Bit2_get(array, j + 1, i);
                if (next_bit == 1 && next_bit != 2) {
                        Bit2_put(array, j + 1, i, 2);
                        insert_to_Seq(Q, j + 1, i);
                }
        }
        if (i - 1 >= 0) {
                int next_bit = Bit2_get(array, j, i - 1);
                if (next_bit == 1 && next_bit != 2) {
                        Bit2_put(array, j , i - 1, 2);
                        insert_to_Seq(Q, j, i - 1);
                }
        }
        if (i + 1 < height) {
                int next_bit = Bit2_get(array, j, i + 1);
                if (next_bit == 1 && next_bit != 2) {
                        Bit2_put(array, j, i + 1, 2);
                        insert_to_Seq(Q, j, i + 1);
                }
        }        
}

/********** insert_to_Seq ********
 *
 * Inserts the pixel coordinates (col, row) into the sequence 
 * for processing.
 *
 * Parameters:
 *      Seq_T Q   The sequence pointer into which the pixel coordinates 
 *                are inserted.
 *      int col:  The column index of the pixel.
 *      int row:  The row index of the pixel.
 *
 * Return: void
 *
 * Expects
 *      Seq_T Q must not be NULL.
 * Notes:
 *      Will CRE if Q is NULL or memory allocation for the 
        pixel fails.
 ************************/
void insert_to_Seq(Seq_T Q, int col, int row) 
{
        Index_T pair = malloc(sizeof(*pair));
        if (pair == NULL) {
                RAISE(Mem_Failed);
        }
        pair -> col = col;
        pair -> row = row;
        Seq_addhi(Q, pair);
}

/********** pbmwrite ********
 *
 * Writes the 2D bit array representing a PBM image to the output file 
 * in plain PBM format.
 *
 * Parameters:
 *      FILE *outputfp: The output file pointer where the PBM image 
 *                      will be written.
 *      Bit2_T bitmap:  The 2D bit array representing the PBM image.
 *
 * Return: void
 *
 * Expects
 *      Bit2_T bitmap must be a valid 2D bit array.
 *      outputfp must be a valid file pointer.
 * Notes:
 *      Will CRE if outputfp or bitmap is NULL.
 ************************/
void pbmwrite(FILE *outputfp, Bit2_T bitmap)
{
        int height = Bit2_height(bitmap);
        int width = Bit2_width(bitmap);
        fprintf(outputfp, "P1\n");
        fprintf(outputfp, "%d %d\n", width, height);
        for (int i = 0; i < height; i++) {
                for (int j = 0; j < width; j++) {
                        int curr_bit = Bit2_get(bitmap, j, i);
                        printf("%d ", curr_bit);
                }
                printf("\n");
        }
        
}



