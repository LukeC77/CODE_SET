/*
 *     bitpack.c
 * 
 *     Purpose: Implementation for bitpack. Manipulates bit fields with a 64 
 *              bit (8 byte) value word.
 */

#include "bitpack.h"
#include "assert.h"
#include "except.h"
#include <stdio.h>

Except_T Bitpack_Overflow = { "Overflow packing bits" };

uint64_t negShiftRight(uint64_t word, unsigned width, unsigned lsb);

/********** Bitpack_fitsu ********
 *
 * Purpose: Determine if an unsigned integer can be represented in a certain
 *          number of bits.
 *
 * Parameters: 
 *      uint64_t n: An unsigned integer.
 *      unsigned width: Number of bits available.
 * 
 * Return: A boolean representing if n can be represented in width bits.
 *      
 * Expects: Width is less than or equal to 64 bits.
 * 
 * Notes: The bits are interpreted are unsigned integers.
 *      
 ************************/
bool Bitpack_fitsu(uint64_t n, unsigned width)
{
        assert(width <= 64);
        if (width == 64) {
                return true;
        } else {
                uint64_t one = 1;
                uint64_t max = (one << width) - 1;
                return n <= max;
        }
}

/********** Bitpack_fitss ********
 *
 * Purpose: Determine if an unsigned integer can be represented in a certain
 *          number of bits.
 *
 * Parameters: 
 *      uint64_t n: A signed integer.
 *      unsigned width: Number of bits available.
 * 
 * Return: A boolean representing if n can be represented in width bits.
 *      
 * Expects: Width is less than or equal to 64 bits.
 * 
 * Notes: The bits are interpreted are signed integers in the two's-complement
 *        representation.
 *      
 ************************/
bool Bitpack_fitss(int64_t n, unsigned width)
{
        assert(width <= 64);
        if (width == 64) {
                return true;
        } else {
                int64_t one = 1;
                int64_t min = -(one << (width - 1));
                int64_t max = (one << (width - 1)) - 1;
                return n >= min && n <= max;
        }
}

/********** Bitpack_getu ********
 *
 * Purpose: Extracts a field of length width from the word.
 *
 * Parameters: 
 *      uint64_t word: The word to extract from.
 *      unsigned width: The length of the field to extract.
 *      unsigned lsb: The location of the field's least significant byte.
 * 
 * Return: The unsigned value extracted.
 *      
 * Expects: Width is less than or equal to 64, and width + lsb is less than or
 *          equal to 64.
 * 
 * Notes: Negative widths passed in will be caught by the width <= 64 assertion
 *        because negative widths are cast to a very large unsigned value.
 *      
 ************************/
uint64_t Bitpack_getu(uint64_t word, unsigned width, unsigned lsb)
{
        assert(width <= 64);
        assert((width + lsb) <= 64);
        uint64_t mask;
        if (width == 64) {
                return word;
        } else {
                mask = ((uint64_t)1 << width) - 1;
                mask = mask << lsb;
                mask &= word;
                mask = mask >> lsb;
                return mask;
        }

}

/********** Bitpack_gets ********
 *
 * Purpose: Extracts a field of length width from the word.
 *
 * Parameters: 
 *      uint64_t word: The word to extract from.
 *      unsigned width: The length of the field to extract.
 *      unsigned lsb: The location of the field's least significant byte.
 * 
 * Return: The signed value extracted.
 *      
 * Expects: Width is less than or equal to 64, and width + lsb is less than or
 *          equal to 64.
 * 
 * Notes: Negative widths passed in will be caught by the width <= 64 assertion
 *        because negative widths are cast to a very large unsigned value. If
 *        the most significant bit of the field is a 1, then a helper function
 *        is used such that when shifting right, the bits filled in on the left
 *        are 1s. Otherwise, Bitpack_getu can be called.
 *      
 ************************/
int64_t Bitpack_gets(uint64_t word, unsigned width, unsigned lsb)
{
        assert(width <= 64);
        assert((width + lsb) <= 64);
        uint64_t mask;
        if (width == 64) {
                return word;
        } else {
                mask = ((uint64_t)1 << (width + lsb - 1));
                /* The field of the word is negative */
                if ((mask & word) == mask) {
                        return negShiftRight(word, width, lsb);
                } else {
                        return Bitpack_getu(word, width, lsb);
                }
        }  
}

/********** Bitpack_newu ********
 *
 * Purpose: Returns a new word after updating a field of the original word with
 *          a give value.
 *
 * Parameters: 
 *      uint64_t word: The word to update a field of.
 *      unsigned width: The length of the field to extract.
 *      unsigned lsb: The location of the field's least significant byte.
 *      uint64_t value: The value to replace the field of the original word.
 * 
 * Return: A new word identical to the original word except for the field that
 *         was replaced with a value.
 *      
 * Expects: Width is less than or equal to 64, and width + lsb is less than or
 *          equal to 64.
 * 
 * Notes: Negative widths passed in will be caught by the width <= 64 assertion
 *        because negative widths are cast to a very large unsigned value.
 *        Raises a CRE if the value cannot fit in width unsigned bits.
 *      
 ************************/
uint64_t Bitpack_newu(uint64_t word, unsigned width, unsigned lsb, 
                      uint64_t value)
{
        assert(width <= 64);
        assert((width + lsb) <= 64);
        if (Bitpack_fitsu(value, width) == false) {
                RAISE(Bitpack_Overflow);
        }
        uint64_t mask_h = ~0;
        uint64_t mask_l = ~0;
        uint64_t mask;
        if (width == 64) {
                mask = 0;
        } else {
                mask_h = mask_h << (width + lsb);
                mask_l = mask_l >> (64 - lsb);
                mask = mask_h | mask_l;
        }
        value = value << lsb;
        mask &= word;
        mask |= value;
        return mask;
}

/********** Bitpack_news ********
 *
 * Purpose: Returns a new word after updating a field of the original word with
 *          a give value.
 *
 * Parameters: 
 *      uint64_t word: The word to update a field of.
 *      unsigned width: The length of the field to extract.
 *      unsigned lsb: The location of the field's least significant byte.
 *      uint64_t value: The value to replace the field of the original word.
 * 
 * Return: A new word identical to the original word except for the field that
 *         was replaced with a value.
 *      
 * Expects: Width is less than or equal to 64, and width + lsb is less than or
 *          equal to 64.
 * 
 * Notes: Negative widths passed in will be caught by the width <= 64 assertion
 *        because negative widths are cast to a very large unsigned value.
 *        Raises a CRE if the value cannot fit in width signed bits.
 *      
 ************************/
uint64_t Bitpack_news(uint64_t word, unsigned width, unsigned lsb, 
                      int64_t value)
{
        assert(width <= 64);
        assert((width + lsb) <= 64);
        if (Bitpack_fitss(value, width) == false) {
                RAISE(Bitpack_Overflow);
        }
        if (width == 64) {
                return (uint64_t)value;
        }
        uint64_t mask_v = ~0;
        mask_v = mask_v >> (64 - width);
        mask_v &= value;
        uint64_t mask_h = ~0;
        uint64_t mask_l = ~0;
        uint64_t mask;
        mask_h = mask_h << (width + lsb);
        mask_l = mask_l >> (64 - lsb);
        mask = mask_h | mask_l;
        mask_v = mask_v << lsb;
        mask &= word;
        mask |= mask_v;
        return mask;
}

/********** negShiftRight ********
 *
 * Purpose: Perform the same shifts as Bitpack_getu but fill in the bits to the
 *          left of the rightmost width bits with 1s.
 *
 * Parameters: 
 *      uint64_t word: The word to extract from.
 *      unsigned width: The length of the field to extract.
 *      unsigned lsb: The location of the field's least significant byte.
 * 
 * Return: The unsigned value extracted.
 *      
 * Expects: Width is less than or equal to 64, and width + lsb is less than or
 *          equal to 64.
 * 
 * Notes: Sets a mask to have all 1s to the left of the rightmost width bits.
 *      
 ************************/
uint64_t negShiftRight(uint64_t word, unsigned width, unsigned lsb) {
        uint64_t mask;
        uint64_t mask2;
        mask = ((uint64_t)1 << width) - 1;
        mask = mask << lsb;
        mask &= word;
        mask = mask >> lsb;
        mask2 = ~(((uint64_t)1 << width) - 1);
        mask |= mask2;
        return mask;
}