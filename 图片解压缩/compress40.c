/*
 *     compress40.c
 * 
 *     Purpose: Implementation for compress40. The two functions, compress40 and
 *              decompress40, are called by the main function in 40image to
 *              convert between portable pixmap images and compressed binary
 *              image files. The details of the compression and decompression
 *              process are hidden away in other modules, choosing to instead 
 *              rely on their interfaces.
 */

#include "compress40.h"
#include "imageIO.h"
#include "compvideo.h"
#include "chroma.h"
#include "dct.h"
#include "codeword.h"
#include <stdbool.h>

void clearSeq(Seq_T seq);

/********** compress40 ********
 *
 * Purpose: Read in a PPM and write out a compressed image.
 *
 * Parameters: 
 *      FILE *input: A pointer to a file to read from.
 * 
 * Return: None.
 *      
 * Expects: The given file is a valid PPM image.
 * 
 * Notes: Utilizes functions hidden in other modules to handle the compression
 *        process.
 *      
 ************************/
void compress40(FILE *input)
{
        A2Methods_T methods = uarray2_methods_blocked;
        assert(methods != NULL);
        Seq_T code_info =  Seq_new(0);
        Seq_T code_words = Seq_new(0);
        Pnm_ppm origImg = readPPM(input);
        A2 arrayCV = rgbToCV(origImg);
        encodeChroma(arrayCV, code_info);
        psToDCT(arrayCV, code_info);
        packWords(code_info, code_words);
        writeCompressed(arrayCV, code_words);
        methods->free(&arrayCV);
        Pnm_ppmfree(&origImg);
        clearSeq(code_info);
        Seq_free(&code_words);
}

/********** decompress40 ********
 *
 * Purpose: Read in a compressed image and write out a PPM image.
 *
 * Parameters: 
 *      FILE *input: A pointer to a file to read from.
 * 
 * Return: None.
 *      
 * Expects: The given file is an image that has been compressed.
 * 
 * Notes: Utilizes functions hidden in other modules to handle the decompression
 *        process.
 *      
 ************************/
void decompress40(FILE *input)
{
        A2Methods_T methods = uarray2_methods_blocked;
        assert(methods != NULL);
        Seq_T code_words = Seq_new(0);
        Seq_T code_info =  Seq_new(0);
        struct Pnm_ppm pixmap = readCompressed(input, code_words);
        A2 arrayCV = methods->new_with_blocksize(pixmap.width, pixmap.height, 
                                                 sizeof(struct Component_T), 
                                                 2);
        assert(arrayCV != NULL);
        unpackWords(code_words, code_info);
        dctToPS(arrayCV, code_info);
        decodeChroma(arrayCV, code_info);
        cvToRGB(arrayCV, &pixmap);
        writePPM(&pixmap);
        methods->free(&arrayCV);
        methods->free(&(pixmap.pixels));
        clearSeq(code_info);
        Seq_free(&code_words);
}

/********** clearSeq ********
 *
 * Purpose: Deallocates and frees the memory for a sequence containing elements
 *          of the CodeInfo_T struct.
 *
 * Parameters: 
 *      Seq_T seq: The sequence to be freed.
 * 
 * Return: None.
 *      
 * Expects: The sequence contains pointers to CodeInfo_T objects.
 * 
 * Notes: Frees all of the inner CodeInfo_T objects before using the free
 *        function from the Sequence interface.
 *      
 ************************/
void clearSeq(Seq_T seq)
{
        for (int i = 0; i < Seq_length(seq); i++) {
                CodeInfo_T ct = Seq_get(seq, i);
                FREE(ct);
        }
        Seq_free(&seq);
}