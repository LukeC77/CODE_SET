/**************************************************************
 *
 *     operation.h
 *
 *
 *     operation.h contains functions for each opcode, from 0 to 13.
 *     Each function executes a operator, plus a helper for freeing
 *     sequence.
 *     
 *
 **************************************************************/

#ifndef OPERATION_H
#define OPERATION_H

#include <stdint.h>
#include <stdbool.h>
#include "table.h"
#include "seq.h"
#include "type.h"


void ConMov     (Table_T table, Seq_T segid_bin, uint32_t* arr, uint32_t* ptr, 
                 uint64_t* id_counter, Um_instruction inst, bool* notHalt);

void SegLoad    (Table_T table, Seq_T segid_bin, uint32_t* arr, uint32_t* ptr, 
                 uint64_t* id_counter, Um_instruction inst, bool* notHalt);

void SegStore   (Table_T table, Seq_T segid_bin, uint32_t* arr, uint32_t* ptr, 
                 uint64_t* id_counter, Um_instruction inst, bool* notHalt);

void Add        (Table_T table, Seq_T segid_bin, uint32_t* arr, uint32_t* ptr,
                 uint64_t* id_counter, Um_instruction inst, bool* notHalt);

void Mul        (Table_T table, Seq_T segid_bin, uint32_t* arr, uint32_t* ptr,
                 uint64_t* id_counter, Um_instruction inst, bool* notHalt);

void Div        (Table_T table, Seq_T segid_bin, uint32_t* arr, uint32_t* ptr,
                 uint64_t* id_counter, Um_instruction inst, bool* notHalt);

void NotAnd     (Table_T table, Seq_T segid_bin, uint32_t* arr, uint32_t* ptr, 
                 uint64_t* id_counter, Um_instruction inst, bool* notHalt);

void Halt       (Table_T table, Seq_T segid_bin, uint32_t* arr, uint32_t* ptr, 
                 uint64_t* id_counter, Um_instruction inst, bool* notHalt);

void Map        (Table_T table, Seq_T segid_bin, uint32_t* arr, uint32_t* ptr, 
                 uint64_t* id_counter, Um_instruction inst, bool* notHalt);

void UnMap      (Table_T table, Seq_T segid_bin, uint32_t* arr, uint32_t* ptr, 
                 uint64_t* id_counter, Um_instruction inst, bool* notHalt);

void Output     (Table_T table, Seq_T segid_bin, uint32_t* arr, uint32_t* ptr, 
                 uint64_t* id_counter, Um_instruction inst, bool* notHalt);

void Input      (Table_T table, Seq_T segid_bin, uint32_t* arr, uint32_t* ptr, 
                 uint64_t* id_counter, Um_instruction inst, bool* notHalt);

void LoadProgram(Table_T table, Seq_T segid_bin, uint32_t* arr, uint32_t* ptr,
                 uint64_t* id_counter, Um_instruction inst, bool* notHalt);

void LoadValue  (Table_T table, Seq_T segid_bin, uint32_t* arr, uint32_t* ptr, 
                 uint64_t* id_counter, Um_instruction inst, bool* notHalt);

void vfree(const void *key, void **value, void *cl);

#endif