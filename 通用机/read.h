/**************************************************************
 *
 *     read.h
 *
 *     Diwei Chen (dchen22)
 *     Yifan Cao (ycao12)
 *     Nov 20 2024
 *
 *     CS 40 HW6 - um
 *
 *     The `read.h` file defines functions and data structures 
 *     for reading UM instructions. It declares `readUM`, which reads UM 
 *     files, and several inline functions for extracting specific parts 
 *     of instructions, including operation codes (`readOP`), register 
 *     codes (`read_3Register`), and register values (`read_RegVal`). 
 *     
 *
 **************************************************************/

#ifndef READ_H
#define READ_H

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <assert.h>
#include <bitpack.h>
#include <stdint.h>
#include "type.h"
#include "table.h"
#include "seq.h"
#include "fmt.h"


void readUM(char* filename, Table_T table);

/********** readOP ********
 * read operation code from an instruction code
 * 
 * Parameters:
 *      Um_instruction inst: a 32-bit integer representing an instruction
 *      
 * Return: 
 *      Um_opcode integer represents an opeceration 
 *
 * Expects:
 *      - if operation code exceed range [0, 13], raise exception 
 *
 * Notes:
 *      None
 ************************/
static inline Um_opcode readOP(Um_instruction inst)
{
       Um_opcode code = Bitpack_getu(inst, 4, 28);
       assert(code >= 0 && code <= 13);
       return code;
}

/********** read_3Register ********
 * read three registers codes
 * 
 * Parameters:
 *      Um_instruction inst: a 32-bit integer representing an instruction
 *      
 * Return: 
 *      a struct Resister3_T object storing register codes
 *
 * Expects:
 *      None
 *
 * Notes:
 *      We use this function in 13 operations except for Load Value.
 *      Register code is in the range [0, 7] definitely because this
 *      is also the range that 3 bits can represent.
 ************************/
static inline struct Register3_T read_3Register(Um_instruction inst)
{
       Um_register ra = Bitpack_getu(inst, 3, 6);
       Um_register rb = Bitpack_getu(inst, 3, 3);
       Um_register rc = Bitpack_getu(inst, 3, 0);
       struct Register3_T reg = {ra, rb, rc}; 
       return reg;
}

/********** read_RegVal ********
 * if Load Value is triggered, use this function to read register code
 * and value.
 * 
 * Parameters:
 *      Um_instruction inst: a 32-bit integer representing an instruction
 *      
 * Return: 
 *      a struct ResVal_T object storing a register code and its value.
 *
 * Expects:
 *      None
 *
 * Notes:
 *      We use this function in Load Value.
 *      Register code is in the range [0, 7] definitely because this
 *      is also the range that 3 bits can represent.
 ************************/
static inline struct RegVal_T read_RegVal(Um_instruction inst)
{
       Um_register ra = Bitpack_getu(inst, 3, 25);
       uint32_t value = Bitpack_getu(inst, 25, 0);
       struct RegVal_T reg_val = {ra, value};
       return reg_val;
}

#endif