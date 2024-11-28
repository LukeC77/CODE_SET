/**************************************************************

 *     unblackedges.c
 *
 *
 *     This file contains the main function and supporting functions 
 *     to process PBM (Portable Bitmap) files. The program takes one 
 *     command-line argument, which is the name of the PBM file to 
 *     process. If no argument is provided, it reads from standard 
 *     input (stdin). The program verifies that the file has the 
 *     correct ".pbm" extension before processing. Errors related 
 *     to file handling, invalid arguments, or file format are 
 *     handled using custom exceptions.
 *     
 *
 **************************************************************/


#include <string.h>
#include <ctype.h>
#include <atom.h>
#include "process_black.h"

extern const Except_T Mem_Failed;
extern const Except_T Beyond_Bound;
const Except_T Arg_Num_Error = {"Number of arguments is greater than 2"};
const Except_T File_Failed = {"File cannot open"};
const Except_T Format_Error = {"File's extension is not pbm"};

void open_close_file(char* filename);
bool check_file_extension(const char* filename);


/********** main ********
 *
 * Entry point for the program. Processes a PBM file either from a file or 
 * from standard input depending on the number of arguments provided.
 *
 * Parameters:
 *      int argc:     The number of command-line arguments.
 *      char *argv[]: The array of command-line arguments.
 *
 * Return: Returns EXIT_SUCCESS if the program runs successfully.
 *
 * Expects:
 *      argc should be 1 (for stdin) or 2 (for file input).
 * Notes:
 *      Will raise Arg_Num_Error if argc > 2.
 ************************/
int main(int argc, char *argv[])
{
        if (argc == 2) {
                open_close_file(argv[1]);
        } else if (argc == 1) {
                process(stdin);
        } else {
                RAISE(Arg_Num_Error);
        }
        return EXIT_SUCCESS;
}

/********** open_close_file ********
 *
 * Opens the given file, checks its extension, and processes the file.
 *
 * Parameters:
 *      char *filename: The name of the file to open and process.
 *
 * Return: void
 *
 * Expects:
 *      filename must point to a valid file path.
 *
 * Notes:
 *      Will raise File_Failed if the file cannot be opened.
 *      Will raise Format_Error if the file extension is not PBM.
 ************************/
void open_close_file(char* filename)
{       
        FILE *fp = fopen(filename, "rb");
        if (fp == NULL) {
                RAISE(File_Failed);
        }

        if (!check_file_extension(filename)) {
                RAISE(Format_Error);
        }

        process(fp);

        fclose(fp);
}

/********** check_file_extension ********
 *
 * Checks if the given filename has a ".pbm" extension.
 *
 * Parameters:
 *      const char *filename: The name of the file to check.
 *
 * Return: Returns true if the file has a ".pbm" extension, false otherwise.
 *
 * Expects:
 *      filename must not be NULL.
 *
 * Notes:
 *      Returns false if the filename does not contain an extension.
 ************************/
bool check_file_extension(const char* filename)
{
        char* correct_ext = "pbm";
        /*search the last dot*/
        char *lastDot = strrchr(filename, '.');
        if  (lastDot == NULL ){
                return false;
        }
        /* the file extension string */
        char *extension = lastDot + 1;
        return strcmp(correct_ext, extension) == 0;
}