/*
 *     imageIO.c
 *     Yifan Cao(ycao12), Wesly Zhou (wzhou05)
 *     10/16/2024
 *     CS40 HW4 arith
 * 
 *     Purpose: Implementation for imageIO. Utilizes the pnm interface to handle
 *              reading and writing a PPM image. Compressed images are
 *              represented with a 2D array in order to store each pixel and
 *              the information related to each pixel. Utilizes Hanson's
 *              sequences as a dynamic array to store code words and information
 *              related to code words.
 */

#include "imageIO.h"
#include "assert.h"

/********** readPPM ********
 *
 * Purpose: Read in a PPM image from a given file.
 *
 * Parameters: 
 *      FILE *input: A pointer to a file to read from.
 * 
 * Return: A Pnm_ppm object, containing the pixmap.
 *      
 * Expects: The given file contains a valid PPM image or the PPM image passed
 *          through standard inpupt is valid.
 * 
 * Notes: Utilizes the pnm interface to handle reading in the file and rely on
 *        it to catch any exceptions.
 *      
 ************************/
Pnm_ppm readPPM(FILE *input)
{
        assert(input != NULL);
        A2Methods_T methods = uarray2_methods_blocked;
        assert(methods != NULL);
        Pnm_ppm pixmap = Pnm_ppmread(input, methods);
        return pixmap;
}

/********** writePPM ********
 *
 * Purpose: Write an uncompressed image to standard output.
 *
 * Parameters: 
 *      Pnm_ppm pixmap: The pixmap with uncompressed pixels.
 * 
 * Return: None.
 *      
 * Expects: The uncompressed image written to standard output should not have
 *          a large difference with the original image.
 * 
 * Notes: Utilizes the pnm interface to write the pixmap to standard output.
 *      
 ************************/
void writePPM(Pnm_ppm pixmap)
{   
        Pnm_ppmwrite(stdout, pixmap);
}

/********** writeCompressed ********
 *
 * Purpose: Write a compressed binary image to standard output, consisting of
 *          a header and a sequence of 32-bit code words.
 *
 * Parameters: 
 *      A2 uarray2b: A 2D array representing the original image.
 *      Seq_T seq: A sequence storing all of the code words.
 * 
 * Return: None.
 *      
 * Expects: The code words stored in the sequence are stored in row-major order.
 * 
 * Notes: The width and height included in the header are the width and height
 *        of the original image after trimming off any odd column or row. The
 *        code words are written in row-major order, and each code word is
 *        written to disk in big-endian order.
 *      
 ************************/
void writeCompressed(A2 uarray2b, Seq_T seq)
{
        assert(uarray2b != NULL);
        assert(seq != NULL);
        A2Methods_T methods = uarray2_methods_blocked;
        assert(methods != NULL);
        int width = methods->width(uarray2b);
        int height = methods->height(uarray2b);
        printf("COMP40 Compressed image format 2\n%u %u", width, height);
        printf("\n");
        for (int i = 0; i < Seq_length(seq); i++) {
                uint64_t codeword = (uint64_t) Seq_get(seq, i);
                uint8_t byte0, byte1, byte2, byte3;
                uint8_t mask = 0xFF;
                byte0 = (codeword >> 24) & mask;
                byte1 = (codeword >> 16) & mask;
                byte2 = (codeword >> 8) & mask;
                byte3 = codeword & mask;
                putchar(byte0);
                putchar(byte1);
                putchar(byte2);
                putchar(byte3);
        }
}

/********** readCompressed ********
 *
 * Purpose: Read in a compressed binary image.
 *
 * Parameters: 
 *      FILE *input: A pointer to a file to read from.
 *      Seq_T seq: A sequence to store all of the code words read in.
 * 
 * Return: A struct Pnm_ppm pixmap for the decompressed image.
 *      
 * Expects: The given file should contain a compressed binary image or the
 *          image passed through standard input should be valid.
 * 
 * Notes: The code words read in are stored in big-endian order. Raises a CRE
 *        if the supplied file is too short (number of codewords is too low
 *        for stated width and height or last codeword is incomplete). Memory
 *        is allocated for an A2 that gets stored in struct Pnm_ppm pixmap;
 *        this must be freed in the decompress40 function.
 *      
 ************************/
struct Pnm_ppm readCompressed(FILE *input, Seq_T seq)
{
        assert(seq != NULL);
        unsigned height, width;
        int read = fscanf(input, "COMP40 Compressed image format 2\n%u %u", 
                          &width, &height);
        assert(read == 2);
        int c = getc(input);
        assert(c == '\n');
        A2Methods_T methods = uarray2_methods_blocked;
        assert(methods != NULL);
        A2 array = methods->new_with_blocksize(width, height, 
                                               sizeof(struct Pnm_rgb), 2);
        struct Pnm_ppm pixmap = { .width = width, .height = height
                                , .denominator = 255, .pixels = array
                                , .methods = methods
                                };
        int total_codewords = (width / 2) * (height / 2);
        int total_bytes = 4 * total_codewords;
        int bytes_read = 0;
        uint64_t codeword = 0;
        uint8_t byte;
        int byte_int;
        while ((byte_int = getc(input)) != EOF) {
                byte = (uint8_t) byte_int;
                codeword = (codeword << 8) | byte;
                bytes_read += 1;
                if (bytes_read % 4 == 0) {
                        Seq_addhi(seq, (void *) codeword);
                        codeword = 0;
                }
        }
        assert(bytes_read == total_bytes);
        assert(bytes_read % 4 == 0);
        return pixmap;
}