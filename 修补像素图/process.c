/**************************************************************
 *
 *                     process.c
 *
 *
 *     Summary:
 *     This file implements various functions for processing data from a file, 
 *     including reading lines, extracting numeric and non-numeric data, 
 *     routing data into a hash table, freeing allocated resources, 
 *     and outputting results in the P5 PGM format.
 *
 **************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <ctype.h>
#include <atom.h>
#include "seq.h"
#include "process.h"


extern const Except_T Mem_Failed;
/********** data_processing **********
 *
 * Processes each line of the given file, extracts relevant data, routes 
 * the data to a hash table, and outputs the results.
 *
 * Parameters:
 *      FILE *fp:        Pointer to the file stream from which lines are read.
 *      char *file_name: Pointer to the string containing the name of the file.
 *
 * Return:
 *      None.
 *
 * Notes:
 *      - The function reads lines from the file, processes them, and stores
 *        results in a hash table.
 *      - Extracts numeric and non-numeric data from each line using the 
 *        extract function and routes it to the table using route_to_table.
 *      - Frees allocated memory for each line and its extracted components 
 *        after processing.
 *      - Calls output to handle correct lines and outputs results based on 
 *        the extracted data.
 *      - Frees the hash table and associated resources before exiting.
 *
 * Excepts:
 *      - Potential memory issues if allocation or deallocation fails, though 
 *        not explicitly handled within this function.
 *      - May indirectly propagate errors from called functions like readaline, 
 *        extract, route_to_table, and output if they raise exceptions or fail.
 ***************************************/
void data_processing(FILE* fp, char* file_name)
{       
        char* line;
        int* digi;
        char* non_digi;
        size_t line_size;
        char* target;

        /* Initialize a hash table */
        Table_T table = Table_new(50, NULL, NULL);

        /* add a while loop to porcess each line until reach EOF */
        while ((line_size = readaline(fp, &line)) != 0) {
                unsigned non_digi_len = extract(line, &digi, 
                                                &non_digi, line_size);
                route_to_table(table, digi, non_digi, non_digi_len, &target);
                free(line);
                free(non_digi);
                line = NULL;
                digi = NULL;
                non_digi = NULL;
        }        
        Seq_T correct_lines = Table_get(table, target);
        (void) correct_lines;
        (void) file_name;
        output(correct_lines, file_name);
        Table_map(table, vfree, NULL);

        Table_free(&table);
}

/********** route_to_table **********
 *
 * Routes numeric data to a sequence in a hash table based on a key derived 
 * from the non-numeric data, updating the target pointer if the sequence 
 * contains more than one entry.
 *
 * Parameters:
 *      Table_T table:           Hash table where sequences of numeric data 
 *                               are stored, keyed by non-numeric data.
 *      int *digi:               Pointer to the numeric data to be added to 
 *                               the sequence associated with the key.
 *      char *non_digi:          Pointer to the non-numeric data used to 
 *                               generate the key for the hash table.
 *      unsigned non_digi_len:   Length of the non-numeric data.
 *      char **target:           Pointer to a string where the key of the 
 *                               sequence with more than one entry will be 
 *                               stored.
 *
 * Return:
 *      None.
 *
 * Notes:
 *      - A sequence corresponding to the key derived from non_digi will be 
 *        created in the table if it does not exist, and digi will be added to
 *        it.
 *      - If the sequence already exists, digi will be added to the existing 
 *        sequence.
 *      - If the sequence length exceeds 1, *target will be set to point to 
 *        the key.
 *
 * Excepts:
 *      - Raises Mem_Failed if memory allocation for adding digi to the 
 *        sequence fails.
 ***************************************/
void route_to_table(Table_T table, 
                    int* digi, 
                    char* non_digi, 
                    unsigned non_digi_len,
                    char** target)
{       
        /* Get the key for hash table using non_digit bytes */
        const char* key = Atom_new(non_digi, non_digi_len);

        /* Check if the key exists in the table */
        Seq_T digi_seq = Table_get(table, key);
        if (digi_seq == NULL) {
                digi_seq = Seq_new(10);
                /* Store the new sequence */ 
                Table_put(table, key, digi_seq);  
        } 
        /* Insert value into the sequence for key */
        if(Seq_addhi(digi_seq, digi) != digi) {
                RAISE(Mem_Failed);
        }
        /* set target to point to  correct line sequence */
        if (Seq_length(digi_seq) > 1) {
                *target = (char*)key;
        }
}

/********** extract **********
 *
 * Extracts digit and non-digit characters from a line, storing digits as 
 * integers and non-digits as a string, while returning the length of the 
 * non-digit sequence.
 *
 * Parameters:
 *      char *line:         Pointer to the line of characters to be processed.
 *      int **digi:         Pointer to an integer array where extracted 
 *                          numeric values will be stored.
 *      char **non_digi:    Pointer to a character array where extracted 
 *                          non-numeric characters will be stored.
 *      unsigned line_size: The size of the line, indicating the number of 
 *                          characters to process.
 *
 * Return:
 *      The length of the non-digit sequence stored in *non_digi.
 *
 * Notes:
 *      - *digi will contain integers derived from clusters of digit characters 
 *        in line, with a sentinel value of -1 at the end.
 *      - *non_digi will contain the non-digit characters from line as a 
 *        null-terminated string.
 *      - The function returns the length of the non-digit string stored in 
 *        *non_digi.
 *
 * Excepts:
 *      - Raises Mem_Failed if memory allocation or reallocation for digi or 
 *        non_digi fails.
 ***************************************/
unsigned extract(char* line, int** digi, char** non_digi, unsigned line_size) 
{
        /* allocate mem for two arrays in heap, check if malloc failed */
        unsigned digi_idx = 0, non_digi_idx = 0;

        *digi = (int *)malloc(sizeof(int) * line_size);
        *non_digi = (char *)malloc(sizeof(char) * line_size);

        /* Error checking for mem allocation failure */
        if (*digi == NULL || *non_digi == NULL) {
                RAISE(Mem_Failed);
        }

        /* extract non-digit and digit bytes then store them seperately */
        int sum = 0;
        bool read_digi = false;
        for (unsigned i = 0; i < line_size; i++) {
                /* find clustered digit bytes and convert to 1 int */
                if (line[i] >= '0' && line[i] <= '9') {
                        sum = 10 * sum + (int)(line[i] - '0');
                        read_digi = true;
                }
                else {  
                        /* reading digits before encountering this non-digit */
                        if (read_digi) {
                                (*digi)[digi_idx] = sum;
                                digi_idx++;//store digit
                                sum = 0;
                                read_digi = false;
                        }
                        (*non_digi)[non_digi_idx] = line[i];//store non-digit
                        non_digi_idx++;
                }
        }

        /* add null character */
        if (non_digi_idx == line_size) {
                (*non_digi) = (char *)realloc(*non_digi, 
                                              sizeof(char) * (line_size + 1));
                if (*non_digi == NULL) {
                        RAISE(Mem_Failed);
                }
        }
        (*non_digi)[non_digi_idx] = '\0';
        non_digi_idx++;

        /* add -1 at the end of digi
         * which works as a sentry to bound of array */
        if (digi_idx == line_size) {
                (*digi) = (int *)realloc(*digi, 
                                          sizeof(int) * (line_size + 1));
                if (*digi == NULL) {
                        RAISE(Mem_Failed);
                }
        }
        (*digi)[digi_idx] = -1;
        digi_idx++;

        return non_digi_idx;
}

/********** vfree **********
 *
 * Frees the memory associated with a sequence of pointers stored as values 
 * in a hash table, including the sequence itself.
 *
 * Parameters:
 *      const void *key:    Pointer to the key associated with the value in the 
 *                          hash table. The key is not used in this function.
 *      void **value:       Pointer to a sequence (Seq_T) of dynamically 
 *                          allocated items, each of which will be freed.
 *      void *cl:           A client data pointer, not used in this function.
 *
 * Return:
 *      None.
 *
 * Notes:
 *      - All dynamically allocated items within the sequence pointed to by 
 *        *value will be freed.
 *      - The sequence itself will be freed.
 *
 * Excepts:
 *      - Undefined behavior if value is NULL or does not point to a valid 
 *        sequence.
 ***************************************/

void vfree(const void *key, void **value, void *cl, int num) {
        (void) key;
        (void) cl;
        num = 10;
        void(num);
        int SIZE = Seq_length(*value);
        for (int i = 0; i < SIZE; i++) {
                free(Seq_get(*value, i));
        }
        Seq_T temp = *value;
        Seq_free(&temp);
}

/********** output **********
 *
 * Outputs the data stored in a sequence as a P5 PGM (Portable GrayMap) image 
 * format to standard output, including the dimensions and pixel data.
 *
 * Parameters:
 *      Seq_T sq:         A sequence containing arrays of integers representing 
 *                        pixel values.
 *      char *file_name:  Pointer to a string containing the name of the file 
 *                        being processed, used for metadata in the output.
 *
 * Return:
 *      None.
 *
 * Notes:
 *      - Outputs the P5 PGM format header, including image width, height, and
 *        maximum pixel value (255), followed by pixel data to the standard 
 *        output.
 *      - The output format will include the number of columns based on the 
 *        first sequence element and the number of rows equal to the sequence 
 *        length.
 *
 * Excepts:
 *      - Undefined behavior if SQ is NULL or contains invalid or NULL arrays.
 *      - Assumes that all arrays in SQ are terminated by -1 to determine their 
 *        length; if this is not the case, behavior is undefined.
 ***************************************/
void output(Seq_T SQ, char* file_name) 
{
        int line_num = Seq_length(SQ);
        fprintf(stdout, "P5\n");
        fprintf(stdout, "# %s\n", file_name);

        /* calculate the num of col */
        int *arr = Seq_get(SQ, 0);
        int col_num = 0;
        for (int i = 0; arr[i] != -1; i++) {
                col_num++;
        }
        arr = NULL;

        /* output P5 pgm */
        fprintf(stdout, "%d ", col_num);
        fprintf(stdout, "%d\n", line_num);
        fprintf(stdout, "%d\n", 255);
        for (int i = 0; i < line_num; i++) {
                arr = Seq_get(SQ, i);
                for (int j = 0; j < col_num; j++) {
                        fprintf(stdout, "%c", arr[j]);
                }                
        }
        fprintf(stdout,"\n");
}
