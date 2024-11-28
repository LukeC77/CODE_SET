/*
 *     codeword.c
 *     Yifan Cao(ycao12), Wesly Zhou (wzhou05)
 *     10/16/2024
 *     CS40 HW4 arith
 * 
 *     Purpose: Implementation for codeword. Utilizes the bitpack interface to
 *              handle the packing and unpacking of code words. Takes
 *              information from one sequence (eithr Codeinfo_T structs or the
 *              code words themselves) to populate the other sequence.
 */

#include "codeword.h"
#include "assert.h"

/********** packWords ********
 *
 * Purpose: For each block, pack the gathered information into a 32-bit code
 *          word and push the word onto a new sequence.
 *
 * Parameters: 
 *      Seq_T code_info: Sequence containing CodeInfo_T structs.
 *      Seq_T code_words: Sequencec to hold all of the code words.
 * 
 * Return: None.
 *      
 * Expects: The code_info sequence contains CodeInfo_T structs whose values have
 *          all been defined in the previous compression steps. Bitpack
 *          functions are working properly.
 * 
 * Notes: The bitpack interface is utilized to handle the packing of the code
 *        words.
 *      
 ************************/
void packWords(Seq_T code_info, Seq_T code_words)
{
        assert(code_info != NULL);
        assert(code_words != NULL);
        for (int i = 0; i < Seq_length(code_info); i++) {
                CodeInfo_T ct = Seq_get(code_info, i);
                uint64_t codeword = 0;
                codeword = Bitpack_newu(codeword, 4, 0, ct->Pr_index);
                codeword = Bitpack_newu(codeword, 4, 4, ct->Pb_index);
                codeword = Bitpack_news(codeword, 6, 8, ct->d);
                codeword = Bitpack_news(codeword, 6, 14, ct->c);
                codeword = Bitpack_news(codeword, 6, 20, ct->b);
                codeword = Bitpack_newu(codeword, 6, 26, ct->a);
                Seq_addhi(code_words, (void *)codeword);
        }
}

/********** unpackWords ********
 *
 * Purpose: For each code word, unpack the information represented in the code
 *          word. Initialize and update a CodeInfo_T struct for each word.
 *
 * Parameters: 
 *      Seq_T code_words: Sequencec containing all of the code words.
 *      Seq_T code_info: Sequence to hold CodeInfo_T structs.
 * 
 * Return: None.
 *      
 * Expects: The code_words sequence contains the code words read in from the
 *          file in big-endian order. Bitpack functions are working properly.
 * 
 * Notes: The bitpack interface is utilized to handle the unpacking of the code
 *        words. Allocates memory for CodeInfo_T structs that must be freed in
 *        the decompress40 function.
 *      
 ************************/
void unpackWords(Seq_T code_words, Seq_T code_info)
{
        assert(code_info != NULL);
        assert(code_words != NULL);
        for (int i = 0; i < Seq_length(code_words); i++) {
                uint64_t codeword = (uint64_t) Seq_get(code_words, i);
                CodeInfo_T ct;
                NEW(ct);
                ct->a = Bitpack_getu(codeword, 6, 26);
                ct->b = Bitpack_gets(codeword, 6, 20);
                ct->c = Bitpack_gets(codeword, 6, 14);
                ct->d = Bitpack_gets(codeword, 6, 8);
                ct->Pb_index = Bitpack_getu(codeword, 4, 4);
                ct->Pr_index = Bitpack_getu(codeword, 4, 0);
                Seq_addhi(code_info, ct);
        }  
}