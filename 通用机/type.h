/**************************************************************
 *
 *     type.h
 *
 *     Diwei Chen (dchen22)
 *     Yifan Cao (ycao12)
 *     Nov 20 2024
 *
 *     CS 40 HW6 - um
 *
 *     type.h defines several types used in the context of the 
 *     Universal Machine (UM) architecture simulation.
 *
 **************************************************************/

#ifndef TYPE_H
#define TYPE_H

/* A typedef for a uint32_t type, representing a 32-bit instruction in 
 * the UM architecture. 
 */
typedef uint32_t Um_instruction;

/* An enumeration defining the various operation codes (opcodes) 
 * supported by the UM architecture, including operations 
 * such as CMOV, ADD, MUL, DIV, HALT, etc.
 */
typedef enum Um_opcode {
        CMOV = 0, SLOAD, SSTORE, ADD, MUL, DIV,
        NAND, HALT, ACTIVATE, INACTIVATE, OUT, IN, LOADP, LV
} Um_opcode;

/* An enumeration representing the UM's registers, labeled r0 to r7. */
typedef enum Um_register { r0 = 0, r1, r2, r3, r4, r5, r6, r7 } Um_register;

/* A structure that holds three registers (ra, rb, rc) of type Um_register. 
 * This is used to represent a set of three registers for specific operations.
 */
typedef struct Register3_T {
        Um_register ra, rb, rc;
} *Register3_T;

/* A structure containing a register (the ra of Um_register) and its 
 * associated value (25-bit, uses a uint32_t). 
 * This is used in opcode 13: load value
 */
typedef struct RegVal_T {
        Um_register ra;
        uint32_t value;
} *RegVal_T;

#endif