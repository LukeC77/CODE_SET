/**************************************************************
 *
 *                     readaline.c
 *
 *     Assignment:      filesofpix
 *     Authors:         Yifan Cao
 *                      Xiaoyan Xie
 *     Date:            9/15/2024
 *
 *     Summary:
 *     This file implements the `readaline` function, which reads a line from 
 *     a file stream into dynamically allocated memory. It handles memory 
 *     allocation and reallocation as needed and manages errors during the 
 *     reading process.
 *
 **************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "except.h"
#include "readaline.h"

extern const Except_T Mem_Failed;
const Except_T Null_Arg = { NULL };
const Except_T Read_Error = { NULL };

/********** readaline **********
 *
 * Reads a line from the specified file stream into dynamically allocated
 * memory and returns the number of characters read, including the newline 
 * character.
 *
 * Parameters:
 *      FILE *inputfd:  Pointer to the input file stream from which the line 
 *                      is read.
 *      char **datapp:  Pointer to the memory location where the address of
 *                      the dynamically allocated buffer will be stored.
 *
 * Return:
 *      The number of characters read, including the newline character. 
 *      Returns 0 if EOF is reached before reading any characters and
 *      *datapp will be set to NULL.
 *
 * Excepts:
 *      - Raises Null_Arg if inputfd is NULL.
 *      - Raises Mem_Failed if memory allocation or reallocation fails.
 *      - Raises Read_Error if there is an error while reading the input.
 ********************************/
size_t readaline(FILE *inputfd, char **datapp) 
{
        //Check if FILE pointer argument is NULL
        if (inputfd == NULL) {
                RAISE(Null_Arg);
        }

        //allocate mem for an array in heap, check if malloc failed
        size_t arr_size = 1000;
        *datapp = (char *)malloc(sizeof(char) * arr_size);

        if (*datapp == NULL) {
                RAISE(Mem_Failed);
        }

        //Read a line and count the bytes that reads in
        char c;
        size_t count = 0;
        
        while ((c = getc(inputfd)) != '\n' && c != EOF) {
                //check capacity of array, resize if necessary
                if (count >= arr_size - 1) {
                        arr_size *= 2;
                        *datapp = (char *)realloc(*datapp, 
                                                  sizeof(char) * arr_size);
                        if (*datapp == NULL) {
                                RAISE(Mem_Failed);
                        }
                }

                //check if error exists when reading the bytes
                if (ferror(inputfd)) {
                        RAISE(Read_Error);//TODO: test
                }

                (*datapp)[count] = c;
                count++;
        }
        
        (*datapp)[count++] = '\n';
        
        //check if EOF, if so, set datapp to NULL and return 0
        if (feof(inputfd)) {
                free(*datapp);
                *datapp = NULL;
                return 0;
        }
        
        return count;
}