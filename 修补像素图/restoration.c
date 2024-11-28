/**************************************************************
 *
 *                     restoration.c
 *
 *
 *     Summary:
 *     This file is the main function for a program aimming to convert a 
 *     corrupted P2 PGM to P5 PGM. It contains the implementation of the
 *     main entry point for the program, which handles file processing 
 *     based on user input or command line arguments. It opens a file, 
 *     processes its contents, and handles errors related to file access 
 *     and argument validation.
 *
 **************************************************************/
#include <stdlib.h>
#include <stdio.h>
#include "except.h"
#include "readaline.h"
#include "process.h"
#include <assert.h>

extern const Except_T Read_Error;
extern const Except_T Mem_Failed;
const Except_T Arg_Error = {NULL};

/********** open_close_file **********
 *
 * Opens a file with the given name for reading, processes its content, and 
 * then closes the file.
 *
 * Parameters:
 *      char *file_name: Pointer to a string containing the name of the file
 *                       to be opened and processed.
 *
 * Return:
 *      None.
 *
 * Excepts:
 *      - Raises Read_Error if the file cannot be opened.
 *      - Exits with EXIT_FAILURE if fopen fails to open the file.
 ***************************************/
void open_close_file(char* file_name);

/********** main **********
 *
 * Entry point of the program that handles command line arguments to open and
 * process a file. It either opens a file specified as a command line argument 
 * or prompts the user for a file name via standard input.
 *
 * Parameters:
 *      int argc:       The number of command line arguments passed to the 
 *                      program, including the program name.
 *      char *argv[]:   An array of pointers to the arguments, where argv[0] 
 *                      is the program name and argv[1] (if provided) is the 
 *                      name of the file to be opened.
 *
 * Return:
 *      EXIT_SUCCESS upon successful execution. May exit with an error if 
 *      incorrect arguments are provided or other conditions are raised.
 *
 * Notes:
 *      - If argc is 2, the program attempts to open the file specified by 
 *        argv[1] using the open_close_file function.
 *      - If argc is 1, the program prompts the user to input a file name, 
 *        then attempts to open the specified file using open_close_file.
 *      - If argc is neither 1 nor 2, Arg_Error is raised.
 *
 * Excepts:
 *      - Raises Arg_Error if more than one argument besides the program name
 *        is provided.
 *      - May exit the program with an error if open_close_file raises an error
 ********************************/
int main(int argc, char *argv[]) 
{       
        if (argc == 2) {
                open_close_file(argv[1]);
        } 
        else if (argc == 1) {
                char file_name[100];
                /* get input from terminal and check if the num of input */
                if (fscanf(stdin, "%s", file_name) == 1) {
                        open_close_file(file_name);
                }
        } 
        else {
                RAISE(Arg_Error);
        }
        
        return EXIT_SUCCESS;
}

void open_close_file(char* file_name) 
{
        FILE* fp = NULL;
        fp = fopen(file_name, "r");
        if (fp == NULL) {
                RAISE(Read_Error);
                exit(EXIT_FAILURE);
        }
        
        data_processing(fp, file_name);
        fclose(fp);
}





