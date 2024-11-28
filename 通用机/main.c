/**************************************************************
 *
 *     main.c
 *
 *
 *     Entry point of the um program. 
 *
 **************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include "read.h"
#include "operation.h"

/********** hash ********
 *
 * Compute the hash value for a given key.
 * This hash function uses modular arithmetic to ensure the result fits
 * within a 32-bit integer range.
 *
 * Parameters:
 *      const void *key: Pointer to the key whose hash value is to be computed.
 *
 * Return:
 *      uint32_t: A 32-bit hash value computed based on the input key.
 *
 * Expects:
 *      The key must not exceed 2^32 - 1.
 *      Behavior is undefined if the key exceeds this limit.
 *
 * Notes:
 *      This function will CRE if the key is NULL.
 ************************/

unsigned hash(const void *key) 
{
        uint64_t size = 0x100000000;/* 2^32 */
        uint64_t slot_id = (uint64_t)(uintptr_t)key % size;
        return (uint32_t)slot_id;
}

/********** cmp ********
 *
 * Compare two keys represented as pointers and determine their order.
 *
 * Parameters:
 *      const void *x: Pointer to the first key to compare.
 *      const void *y: Pointer to the second key to compare.
 *
 * Return:
 *      int: -1 if the first key is less than the second key,
 *           0 if the keys are equal,
 *           1 if the first key is greater than the second key.
 *
 * Expects:
 *      x and y must not be NULL.
 *
 * Notes:
 *      The keys are expected to be 32-bit integers cast as pointers.
 ************************/
int cmp(const void *x, const void *y)
{
        uint32_t key1 = (uint32_t)(uintptr_t)x;
        uint32_t key2 = (uint32_t)(uintptr_t)y;

        if (key1 < key2) {
                return -1;
        } else if (key1 == key2) {
                return 0;
        } else {
                return 1;
        }
}

/********** operations ********
 *
 * Array of function pointers for operations supported by the um.
 *
 * Each function performs a specific operation on a table, sequence, registers,
 * and other parameters based on the provided instruction.
 *
 * Notes:
 *      Each function has the following prototype:
 *          void function(Table_T, Seq_T, uint32_t*, uint32_t*, uint64_t*,
 *                        Um_instruction, bool*)
 *
 *      The operations include: ConMov, SegLoad, SegStore, Add, Mul, Div,
 *      NotAnd, Halt, Map, UnMap, Output, Input, LoadProgram, LoadValue.
 ************************/
void (*operations[])(Table_T, Seq_T, uint32_t*, uint32_t*, uint64_t*,
                     Um_instruction, bool*) = {
        ConMov, SegLoad, SegStore, Add, Mul, Div, NotAnd, Halt, Map, UnMap,
        Output, Input, LoadProgram, LoadValue
};

/********** main ********
 *
 * Entry point for the program. It initializes and runs the um.
 *
 * Parameters:
 *      int argc: Number of command-line arguments.
 *      char* argv[]: Array of command-line argument strings.
 *
 * Return:
 *      int: EXIT_SUCCESS if the program runs successfully,
 *           EXIT_FAILURE if an error occurs (e.g., incorrect arguments).
 *
 * Expects:
 *      argc must be 2 (program name and filename argument).
 *      argv[1] must point to a valid file path.
 *
 * Notes:
 *      - Initializes registers, tables, and sequences for the um.
 *      - Reads input file using `readUM` and processes instructions until Halt.
 *      - Calls operation functions based on opcode extracted from instructions.
 ************************/
int main (int argc, char* argv[])
{
        if (argc != 2) {
                fprintf(stderr, "Usage: %s [filename]\n", argv[0]);
                exit(EXIT_FAILURE);
        }

        uint32_t regs[8] = {0, 0, 0, 0, 0, 0, 0, 0};
        Table_T address_table = Table_new(3, cmp, hash);
        Seq_T segid_bin = Seq_new(3);/* store used and freed id */
        assert(address_table != NULL && segid_bin != NULL);
        uint64_t id_counter = 1;/* keep track of # of id we have used */
        uint32_t prg_counter = 0;/* programer counter */
        bool notHalt = true;

        readUM(argv[1], address_table);

        while (notHalt == true) {
                /* 0 mod 0x100000000 == 0x100000000 mod 0x100000000  */
                /* 0x100000000 help us avoid null pointer which is 0 */
                uint64_t id = 0x100000000;
                Seq_T seg0 = Table_get(address_table, (void *)(uintptr_t)(id));
                void *value = Seq_get(seg0, prg_counter);
                Um_instruction inst = (uint32_t)(uintptr_t)value;
                Um_opcode op = readOP(inst);
                operations[op](address_table, segid_bin, regs, &prg_counter,
                               &id_counter, inst, &notHalt);
        }

        return EXIT_SUCCESS;
}
