/*
 *     codeinfo.h
 *     Yifan Cao(ycao12), Wesly Zhou (wzhou05)
 *     10/16/2024
 *     CS40 HW4 arith
 * 
 *     Purpose: Define a struct to store all of the necessary elements for
 *              packing and unpacking a code word. This struct is utilized by
 *              multiple implementations.
 */
#ifndef CODEINFO_H
#define CODEINFO_H

typedef struct CodeInfo_T {
        /* LSB of each element packed into the code word is noted for the
           compression phase, not relevant for decompression*/
        /* a, b, c, and d are the cosine coefficients */
        unsigned a; /* Unsigned scaled integer, width of 9 bits, LSB at 23 */
        int b, c, d; /* Signed scaled integers, widths of 5 bits, LSBs at 18,
                        13, and 8 respectively */
        float Pb_avg, Pr_avg; /* Averages of the chroma elements */
        /* Averages of the chroma elements converted to 4 bit values to be
           packed, widths of 4 bits, LSBs at 4 and 0 respectively */
        unsigned Pb_index, Pr_index;
} *CodeInfo_T;

#endif