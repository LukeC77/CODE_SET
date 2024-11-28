/**************************************************************
 *
 *      sudoku.c
 *
 *
 *      Verify if the file contains a valid Sudoku solution.
 *      If the solution is correct, return `EXIT_SUCCESS`.
 *      If the solution is incorrect or invalid, return `EXIT_FAILURE`.
 *
 **************************************************************/

#include "uarray2.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <uarray.h>
#include "except.h"
#include <assert.h>
#include <stdbool.h>
#include "pnmrdr.h"
#include "set.h"


void read_and_process(FILE *file_p);
bool is_valid_heading(Pnmrdr_T rdr);
bool read_pixels(Pnmrdr_T rdr, UArray2_T sudoku);
bool check_sudoku(Pnmrdr_T rdr);
bool check_sub_sudoku(UArray2_T sudoku);
bool validate_sequence(UArray2_T sudoku, int index, bool is_row);
bool validate_sub_sequence(UArray2_T sudoku, int start_col, int start_row);
int int_cmp(const void *a, const void *b);
unsigned int int_hash(const void *a);


/********** main ********
 *
 * Initialize the program, read a file or stdin,process Sudoku puzzle,
 * and determine if the Sudoku is solved or not.
 * 
 *
 * Parameters:
 *      int argc: number of command line arguments.
 *      char *argv[]: array of command line arguments.
 *
 * Return: 
 *      EXIT_SUCCESS if the Sudoku is solved, EXIT_FAILURE otherwise.
 *
 * Expects:
 *      - File or stdin to be provided and readable.
 *
 * Notes:
 *      - Terminates with EXIT_FAILURE if the file cannot be opened,
 *        or if the Sudoku puzzle is not valid.
 ************************/
int main(int argc, char *argv[])
{
        FILE *file_p;
        
        /* check # argc */
        assert(argc == 1 || argc == 2);

        /* get file name and read */
        if (argc == 1) {
                file_p = stdin;
        }
        else if (argc == 2) {
                file_p = fopen(argv[1], "r");
                if (file_p == NULL) {
                        exit(1);
                }    
        }

        read_and_process(file_p);
        return 0;
}

/********** read_and_process ********
 *
 * Reads the file, checks for valid Sudoku format, and processes the grid.
 *
 * Parameters:
 *      FILE *file_p: pointer to the file to read from
 *
 * Return:
 *      None.
 *
 * Expects:
 *      - file_p must point to an open and valid file.
 *
 * Notes:
 *      - Will CRE if file_p is NULL or the file is incorrectly formatted.
 *      - Handles exceptions related to Pnmrdr errors.
 *
 ************************/
void read_and_process(FILE *file_p) {

        Pnmrdr_T rdr = NULL;  

        TRY {   
                rdr = Pnmrdr_new(file_p);
                if (rdr == NULL) {
                        fclose(file_p);
                        exit(1);
                }
                
                if (!is_valid_heading(rdr)) {
                        Pnmrdr_free(&rdr);
                        fclose(file_p);
                        exit(1);
                };

                if (!check_sudoku(rdr)) {
                        Pnmrdr_free(&rdr);
                        fclose(file_p);
                        exit(1);
                };  
        }
        EXCEPT(Mem_Failed) {
                Pnmrdr_free(&rdr);
                fclose(file_p);
        }
        EXCEPT(Invalid_Bound) {
                Pnmrdr_free(&rdr);
                fclose(file_p);
        }
        EXCEPT(Out_Of_Bound) {
                if (rdr != NULL) {
                        Pnmrdr_free(&rdr);
                }
                fclose(file_p);
        }
        EXCEPT(Invalid_Size) {
                Pnmrdr_free(&rdr);
                fclose(file_p);
        }
        EXCEPT(Arr_NULL) {
                Pnmrdr_free(&rdr);
                fclose(file_p);
        }
        EXCEPT (Pnmrdr_Badformat) {
                
                if (rdr != NULL) {
                        Pnmrdr_free(&rdr); 
                }
        fclose(file_p); 
        exit(1);
        }
        END_TRY;

        if (rdr != NULL) {
                Pnmrdr_free(&rdr);
        }
        if(file_p != NULL) {
                fclose(file_p);   
        }
        
}

/********** check_sudoku ********
 *
 * Driver function to check if the Sudoku grid is valid.
 * It validates rows, columns, and subgrids.
 *
 * Parameters:
 *      Pnmrdr_T rdr: reference to the Pnmrdr object storing sudoku pixels
 *
 * Return:
 *      bool: Returns true if Sudoku is valid, false otherwise.
 *
 * Expects:
 *      - Pnmrdr object must be initialized and valid.
 *
 * Notes:
 *      - Will CRE if the reader is uninitialized or the input is malformed.
 * 
 ************************/
bool check_sudoku(Pnmrdr_T rdr) {
        UArray2_T sudoku = UArray2_new(9, 9, sizeof(int));
        assert(sudoku != NULL);

        if (!read_pixels(rdr, sudoku)) {
                UArray2_free(&sudoku);
                exit(1);
        }
       
        /* validate rows */
        for (int i = 0; i < 9; i++) {
                if (!validate_sequence(sudoku, i, true)) {
                        UArray2_free(&sudoku);  
                        return false;
                }
        }

        /* validate columns */
        for (int i = 0; i < 9; i++) {
                if (!validate_sequence(sudoku, i, false)) { 
                        UArray2_free(&sudoku);  
                        return false;
                }
        }

        if (!check_sub_sudoku(sudoku)) {
                UArray2_free(&sudoku);  
                return false;
        };

        UArray2_free(&sudoku);
        return true;
}


/********** validate_sequence ********
 *
 * Validates a single row or column in the Sudoku grid.
 *
 * Parameters:
 *      UArray2_T sudoku: the 2D array storing the sudoku pixels
 *      int index: row or column index to check
 *      bool is_row: flag to indicate if it's a row (true) or a column (false)
 *
 * Return:
 *      bool: true if the sequence is valid, false otherwise
 *
 * Expects:
 *      - sudoku is initialized.
 *
 * Notes:
 *      - Will CRE if sequence contains invalid values or duplicates.
 *
 ************************/
bool validate_sequence(UArray2_T sudoku, int index, bool is_row)
{
        Set_T sequence = Set_new(9, int_cmp, int_hash);
        if (sequence == NULL) {
                return false;
        }

        for (int i = 0; i < 9; i++) {
                int pixel;
                if (is_row) {
                        pixel = *((int *)UArray2_at(sudoku, i, index));
                } else {
                        pixel = *((int *)UArray2_at(sudoku, index, i));
                }

                /* out-of-bound check */
                if (pixel < 1 || pixel > 9) {
                        Set_free(&sequence);
                        return false;
                }
                /* duplicate check */
                if (Set_member(sequence, &pixel)) {
                        Set_free(&sequence);
                        return false;
                }

                Set_put(sequence, &pixel);
        }

        /* ensure exactly 9 numbers per sequence */
        if (Set_length(sequence) != 9) {
                Set_free(&sequence);
                return false;
        }

        Set_free(&sequence);
        return true;
}


/********** check_sub_sudoku ********
 *
 * Checks if all the 3x3 subgrids in the Sudoku grid are valid.
 *
 * Parameters:
 *      UArray2_T sudoku: the 2D array storing the sudoku pixels
 *
 * Return:
 *      bool: true if all subgrids are valid, false otherwise.
 *
 * Expects:
 *      - sudoku is initialized.
 *
 * Notes:
 *      - Will CRE if any subgrid contains invalid values or duplicates.
 *
 ************************/
bool check_sub_sudoku(UArray2_T sudoku)
{       
       /* Iterate through each of the 9 subgrids (3x3) */
       for (int row = 0; row < 3; row++) {
                for (int col = 0; col < 3; col++) {
                        /* Validate each subgrid */
                        if (!validate_sub_sequence(sudoku, col * 3, row * 3)) {
                                return false;
                        }
                }
       }
       return true;
}


/********** validate_sub_sequence ********
 *
 * Validates a single 3x3 subgrid within the Sudoku.
 *
 * Parameters:
 *      UArray2_T sudoku: the 2D array storing the sudoku pixels
 *      int start_col: starting column of the subgrid
 *      int start_row: starting row of the subgrid
 *
 * Return:
 *      bool: true if the subgrid is valid, false otherwise.
 *
 * Expects:
 *      - sudoku is initialized.
 *
 * Notes:
 *      - Will CRE if the subgrid contains invalid values or duplicates.
 *
 ************************/
bool validate_sub_sequence(UArray2_T sudoku, int start_col, int start_row)
{
        Set_T sub_s = Set_new(9, int_cmp, int_hash);
        if (sub_s == NULL) {
                return false;
        }

        /* Iterate through the 3x3 subgrid */
        for (int row = 0; row < 3; row++) {
                for (int col = 0; col < 3; col++) {
                        /* Access the element in the subgrid */
                        int pixel = *((int *)UArray2_at(
                                sudoku, start_col + col, start_row + row));
                        if (pixel < 1 || pixel > 9) {         
                                Set_free(&sub_s);
                                return false;
                        }
                        /* Check for duplicate numbers */
                        if (Set_member(sub_s, &pixel)) {
                                Set_free(&sub_s);
                                return false;
                        }
                        Set_put(sub_s, &pixel);
               }
        }

        Set_free(&sub_s);
        return true;
}

/********** int_cmp ********
 *
 * Compares two integer values.
 *
 * Parameters:
 *      const void *a: pointer to the first integer.
 *      const void *b: pointer to the second integer.
 *
 * Return:
 *      int: the difference between the two integers.
 *           Returns a positive value if a > b, negative if a < b, 
 *           and 0 if equal.
 *
 * Expects:
 *      - a and b are valid pointers to integers.
 *
 ************************/
int int_cmp(const void *a, const void *b) {
        return (*(int *)a - *(int *)b);
}

/********** int_hash ********
 *
 * Hash function for an integer value.
 *
 * Parameters:
 *      const void *a: pointer to the integer value to hash.
 *
 * Return:
 *      unsigned int: the hash value for the integer.
 *
 * Expects:
 *      - a is a valid pointer to an unsigned integer.
 *
 ************************/
unsigned int int_hash(const void *a) {
        return *(unsigned int *)a;
}

/********** read_pixels ********
 *
 * Reads the pixel values from the Pnmrdr reader and populates the Sudoku.
 *
 * Parameters:
 *      Pnmrdr_T rdr: the Pnmrdr reader object storing sudoku pixels.
 *      UArray2_T sudoku: the 2D array to store the Sudoku pixels.
 *
 * Return:
 *      bool: true if the grid read successfully, false otherwise.
 *
 * Expects:
 *      - sudoku is initialized.
 *
 * Notes:
 *      - Returns false if any value is out of range or if an invalid 
 *        format is encountered.
 *
 ************************/
bool read_pixels(Pnmrdr_T rdr, UArray2_T sudoku)
{       
        if (sudoku == NULL) {
                return false;
        }

        TRY {   
                for (int row = 0; row < 9; row++) { /*for each row check cols*/
                        for (int col = 0; col < 9; col++) { 
                                int element = Pnmrdr_get(rdr);
                                /* check pixel validity*/
                                if (element > 9 || element < 1) {
                                        return false;
                                }
                                *((int *)UArray2_at(sudoku, col, row)) = 
                                element;
                        }
                }
        }
        EXCEPT(Pnmrdr_Badformat) {
                return false;  
        }
        END_TRY;

        return true;
}

/********** is_valid_heading ********
 *
 * Validates the PGM file header.
 *
 * Parameters:
 *      Pnmrdr_T rdr: the Pnmrdr reader object storing sudoku pixels.
 *
 * Return:
 *      bool: true if the file header is valid, false otherwise.
 *
 * Expects:
 *      - rdr is initialized.
 *      - The PGM file has a valid width, height, and max value.
 *
 * Notes:
 *      - Returns false if the format is invalid.
 *
 ************************/
bool is_valid_heading(Pnmrdr_T rdr) 
{
        if (rdr == NULL) {
                return false;
        }
        TRY {
                /*check headings*/
                Pnmrdr_mapdata data = Pnmrdr_data(rdr);
                if (data.type != Pnmrdr_gray) {
                        exit(1);
                }
                if (data.width != 9 || data.height != 9) {
                        return false;
                }
                if (data.denominator != 9) {
                        return false;
                }

        } EXCEPT(Pnmrdr_Badformat) {  /* File is not in a valid format */
                return false;
        } END_TRY;

        return true;
}       
