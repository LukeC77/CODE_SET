/**************************************************************
 *
 *     operation.c
 *
 *     Diwei Chen (dchen22)
 *     Yifan Cao (ycao12)
 *     Nov 20 2024
 *
 *     CS 40 HW6 - um
 *
 *     implementation for operation.h
 *
 **************************************************************/

#include "operation.h"
#include "read.h"
/********** ConMov ********
 *
 * Copies the value from register rb to register ra if register rc is not zero,
 * and increments the program counter. 
 * 
 *
 * Parameters:
 *      Table_T table:          not used
 *      Seq_T segid_bin:        not used
 *      uint32_t *arr:          array of register values
 *      uint32_t *ptr:          instruction pointer
 *      uint64_t *id_counter:   not used
 *      Um_instruction inst:    the instruction containing registers
 *      bool *notHalt:          not used
 *
 * Return: void
 *
 * Expects
 *      All pointers must not be NULL
 * Notes:
 *      May CRE if pointers are NULL
 ************************/
void ConMov(Table_T table, Seq_T segid_bin, uint32_t* arr, uint32_t* ptr, 
            uint64_t* id_counter, Um_instruction inst, bool* notHalt)
{
        assert(table != NULL && segid_bin != NULL && arr != NULL &&
               ptr != NULL && id_counter != NULL && notHalt != NULL);
        struct Register3_T reg3 = read_3Register(inst);
        if (arr[reg3.rc] != 0) {
                arr[reg3.ra] = arr[reg3.rb];
        }
        *ptr = *ptr + 1;

        (void)table;
        (void)segid_bin;
        (void)id_counter;
        (void)notHalt;
}

/********** SegLoad ********
 *
 * Loads a value from the segment at address rb and offset rc into register ra.
 * Also increments the program counter.
 *
 * Parameters:
 *      Table_T table:          table of segments
 *      Seq_T segid_bin:        not used
 *      uint32_t *arr:          array of register values
 *      uint32_t *ptr:          instruction pointer
 *      uint64_t *id_counter:   not used
 *      Um_instruction inst:    the instruction containing registers
 *      bool *notHalt:          not used
 *
 * Return: void
 *
 * Expects
 *      All pointers must not be NULL
 * Notes:
 *      May CRE if pointers are NULL or memory allocation fails
 ************************/
void SegLoad(Table_T table, Seq_T segid_bin, uint32_t* arr, uint32_t* ptr, 
             uint64_t* id_counter, Um_instruction inst, bool* notHalt)
{
        assert(table != NULL && segid_bin != NULL && arr != NULL &&
               ptr != NULL && id_counter != NULL && notHalt != NULL);
        struct Register3_T reg3 = read_3Register(inst);
        uint64_t id = arr[reg3.rb];
        /* avoid null ptr */
        if (id == 0) {
                id = 0x100000000;
        }
        void *key = (void *)(uintptr_t)id;
        Seq_T target_seg = Table_get(table, key);
        void *value = Seq_get(target_seg, arr[reg3.rc]);
        arr[reg3.ra] = (uint32_t)(uintptr_t)value;
        *ptr = *ptr + 1;

        (void)segid_bin;
        (void)id_counter;
        (void)notHalt;
}

/********** SegStore ********
 *
 * Store a value from register rc into the segment at address ra and offset rb,
 * and increments the program counter.
 *
 * Parameters:
 *      Table_T table:          table of segments
 *      Seq_T segid_bin:        not used
 *      uint32_t *arr:          array of register values
 *      uint32_t *ptr:          instruction pointer
 *      uint64_t *id_counter:   not used
 *      Um_instruction inst:    the instruction containing registers
 *      bool *notHalt:          not used
 *
 * Return: void
 *
 * Expects
 *      All pointers must not be NULL
 * Notes:
 *      May CRE if pointers are NULL or memory allocation fails
 ************************/
void SegStore(Table_T table, Seq_T segid_bin, uint32_t* arr, uint32_t* ptr, 
              uint64_t* id_counter, Um_instruction inst, bool* notHalt)
{
        assert(table != NULL && segid_bin != NULL && arr != NULL &&
               ptr != NULL && id_counter != NULL && notHalt != NULL);
        struct Register3_T reg3 = read_3Register(inst);
        uint64_t id = arr[reg3.ra];
        /* avoid null ptr */
        if (id == 0) {
                id = 0x100000000;
        }
        void *key = (void *)(uintptr_t)id;
        Seq_T target_seg = Table_get(table, key);
        void *value = (void *)(uintptr_t)arr[reg3.rc];
        Seq_put(target_seg, arr[reg3.rb], value);
        *ptr = *ptr + 1;
        
        (void)segid_bin;
        (void)id_counter;
        (void)notHalt;
}

/********** Add ********
 *
 * Adds the values of registers rb and rc modulo 2^32 and stores the result in
 * register ra. Increments the program counter.
 *
 * Parameters:
 *      Table_T table:          not used
 *      Seq_T segid_bin:        not used
 *      uint32_t *arr:          array of register values
 *      uint32_t *ptr:          instruction pointer
 *      uint64_t *id_counter:   not used
 *      Um_instruction inst:    the instruction containing registers
 *      bool *notHalt:          not used
 *
 * Return: void
 *
 * Expects
 *      All pointers must not be NULL
 * Notes:
 *      May CRE if pointers are NULL
 ************************/
void Add(Table_T table, Seq_T segid_bin, uint32_t* arr, uint32_t* ptr, 
         uint64_t* id_counter, Um_instruction inst, bool* notHalt)
{
        assert(table != NULL && segid_bin != NULL && arr != NULL &&
               ptr != NULL && id_counter != NULL && notHalt != NULL);
        struct Register3_T reg3 = read_3Register(inst);
        arr[reg3.ra] = (arr[reg3.rb] + arr[reg3.rc]) % 0x100000000;
        *ptr = *ptr + 1;

        (void)table;
        (void)segid_bin;
        (void)id_counter;
        (void)notHalt;
}

/********** Mul ********
 *
 * Multiplies the values of registers rb and rc modulo 2^32 and stores the 
 * result in register ra. Increments the program counter.
 *
 * Parameters:
 *      Table_T table:          not used
 *      Seq_T segid_bin:        not used
 *      uint32_t *arr:          array of register values
 *      uint32_t *ptr:          instruction pointer
 *      uint64_t *id_counter:   not used
 *      Um_instruction inst:    the instruction containing registers
 *      bool *notHalt:          not used
 *
 * Return: void
 *
 * Expects
 *      All pointers must not be NULL
 * Notes:
 *      May CRE if pointers are NULL
 ************************/
void Mul(Table_T table, Seq_T segid_bin, uint32_t* arr, uint32_t* ptr, 
         uint64_t* id_counter, Um_instruction inst, bool* notHalt)
{
        assert(table != NULL && segid_bin != NULL && arr != NULL &&
               ptr != NULL && id_counter != NULL && notHalt != NULL);
        struct Register3_T reg3 = read_3Register(inst);
        arr[reg3.ra] = (arr[reg3.rb] * arr[reg3.rc]) % 0x100000000;
        *ptr = *ptr + 1;

        (void)table;
        (void)segid_bin;
        (void)id_counter;
        (void)notHalt;
}

/********** Div ********
 *
 * Divides the value in register rb by the value in register rc and stores the
 * result in register ra. Increments the program counter.
 *
 * Parameters:
 *      Table_T table:          not used
 *      Seq_T segid_bin:        not used
 *      uint32_t *arr:          array of register values
 *      uint32_t *ptr:          instruction pointer
 *      uint64_t *id_counter:   not used
 *      Um_instruction inst:    the instruction containing registers
 *      bool *notHalt:          not used
 *
 * Return: void
 *
 * Expects
 *      All pointers must not be NULL
 * Notes:
 *      May CRE if pointers are NULL or rc is zero
 ************************/
void Div(Table_T table, Seq_T segid_bin, uint32_t* arr, uint32_t* ptr, 
         uint64_t* id_counter, Um_instruction inst, bool* notHalt)
{
        assert(table != NULL && segid_bin != NULL && arr != NULL &&
               ptr != NULL && id_counter != NULL && notHalt != NULL);
        struct Register3_T reg3 = read_3Register(inst);
        arr[reg3.ra] = (arr[reg3.rb] / arr[reg3.rc]);
        *ptr = *ptr + 1;

        (void)table;
        (void)segid_bin;
        (void)id_counter;
        (void)notHalt;
}

/********** NotAnd ********
 *
 * Performs a bitwise NOT of the bitwise AND of registers rb and rc, stores the
 * result in register ra, and increments the program counter.
 *
 * Parameters:
 *      Table_T table:          not used
 *      Seq_T segid_bin:        not used
 *      uint32_t *arr:          array of register values
 *      uint32_t *ptr:          instruction pointer
 *      uint64_t *id_counter:   not used
 *      Um_instruction inst:    the instruction containing registers
 *      bool *notHalt:          not used
 *
 * Return: void
 *
 * Expects
 *      All pointers must not be NULL
 * Notes:
 *      May CRE if pointers are NULL
 ************************/
void NotAnd(Table_T table, Seq_T segid_bin, uint32_t* arr, uint32_t* ptr, 
            uint64_t* id_counter, Um_instruction inst, bool* notHalt)
{
        assert(table != NULL && segid_bin != NULL && arr != NULL &&
               ptr != NULL && id_counter != NULL && notHalt != NULL);
        struct Register3_T reg3 = read_3Register(inst);
        arr[reg3.ra] = ~(arr[reg3.rb] & arr[reg3.rc]);
        *ptr = *ptr + 1;

        (void)table;
        (void)segid_bin;
        (void)id_counter;
        (void)notHalt;
}

/********** Halt ********
 *
 * Stops execution and cleans up memory by freeing resources. 
 * Set notHalt false.
 *
 * Parameters:
 *      Table_T table:         memory mapping segments to sequences.
 *      Seq_T segid_bin:       sequence of available segment IDs.
 *      uint32_t *arr:         unused, can be NULL.
 *      uint32_t *ptr:         unused, can be NULL.
 *      uint64_t *id_counter:  unused, can be NULL.
 *      Um_instruction inst:   unused, can be NULL.
 *      bool *notHalt:         pointer to a boolean flag indicating 
 *                             program state.
 *
 * Return: void
 *
 * Expects:
 *      table, segid_bin, and notHalt must not be NULL.
 * Notes:
 *      Frees all resources in `table` and `segid_bin`, sets *notHalt to false.
 ************************/
void Halt(Table_T table, Seq_T segid_bin, uint32_t* arr, uint32_t* ptr, 
          uint64_t* id_counter, Um_instruction inst, bool* notHalt)
{
        assert(table != NULL && segid_bin != NULL && arr != NULL &&
               ptr != NULL && id_counter != NULL && notHalt != NULL);
        Table_map(table, vfree, NULL);
        Seq_free(&segid_bin);
        *notHalt = false;
        Table_free(&table);

        (void)arr;
        (void)ptr;
        (void)id_counter;
        (void)inst;
}

/********** Map ********
 *
 * Allocates a new memory segment and stores its ID in a register.
 * increment the program counter. May increment id counter.
 *
 * Parameters:
 *      Table_T table:         memory mapping segments to sequences.
 *      Seq_T segid_bin:       sequence of available segment IDs.
 *      uint32_t *arr:         array of registers.
 *      uint32_t *ptr:         pointer to the program counter.
 *      uint64_t *id_counter:  counter for generating new IDs.
 *      Um_instruction inst:   instruction containing operation encoding.
 *      bool *notHalt:         unused, can be NULL.
 *
 * Return: void
 *
 * Expects:
 *      table, segid_bin, arr, ptr, and id_counter must not be NULL.
 *      If id_counter reach 2^32, this means we run out of memory. Raise 
 *      exception.
 *      If new segment fail to allocate, this also means we run out of memory. 
 *      Raise exception.
 *      
 * Notes:
 *      A new segment of size `arr[reg3.rc]` is created and stored in `table`.
 *      ID is either reused from `segid_bin` or generated using `id_counter`.
 *      Updates `arr[reg3.rb]` with the new segment ID.
 ************************/
void Map(Table_T table, Seq_T segid_bin, uint32_t* arr, uint32_t* ptr, 
         uint64_t* id_counter, Um_instruction inst, bool* notHalt)
{
        assert(table != NULL && segid_bin != NULL && arr != NULL &&
               ptr != NULL && id_counter != NULL && notHalt != NULL);
        /* construct a new segment */
        struct Register3_T reg3 = read_3Register(inst);
        uint32_t size = arr[reg3.rc];
        Seq_T new_seg = Seq_new(size);
        assert(new_seg != NULL);
        for (uint32_t i = 0; i < size; i++) {
                Seq_addhi(new_seg, (void *)(uintptr_t)0);
        }

        /* find seg_id and insert new_seg into table */
        void *key;
        if (Seq_length(segid_bin) != 0) {
                /* pop up top id in segid_bin */
                key = Seq_remlo(segid_bin);
        } else {
                /* # of seg can't exceed 2^32-1; otherwise, run out of mem */
                assert(*id_counter < 0x100000000);
                key = (void *)(uintptr_t)(*id_counter);
                *id_counter = *id_counter + 1;
        }
        Table_put(table, key, new_seg);

        /* store new id into register $r[b]*/
        arr[reg3.rb] = (uint32_t)(uintptr_t)key;

        *ptr = *ptr + 1;
        
        (void)notHalt;
}

/********** UnMap ********
 *
 * Frees a memory segment and makes its ID available for reuse. increment the 
 * program counter. Update segid_bin.
 *
 * Parameters:
 *      Table_T table:         memory mapping segments to sequences.
 *      Seq_T segid_bin:       sequence of available segment IDs.
 *      uint32_t *arr:         array of registers.
 *      uint32_t *ptr:         pointer to the program counter.
 *      uint64_t *id_counter:  unused, can be NULL.
 *      Um_instruction inst:   instruction containing operation encoding.
 *      bool *notHalt:         unused, can be NULL.
 *
 * Return: void
 *
 * Expects:
 *      table, segid_bin, arr, and ptr must not be NULL.
 * Notes:
 *      Frees memory associated with the segment ID in `arr[reg3.rc]` 
 *      and stores the ID back into `segid_bin`.
 ************************/
void UnMap(Table_T table, Seq_T segid_bin, uint32_t* arr, uint32_t* ptr, 
           uint64_t* id_counter, Um_instruction inst, bool* notHalt)
{
        assert(table != NULL && segid_bin != NULL && arr != NULL &&
               ptr != NULL && id_counter != NULL && notHalt != NULL);
        struct Register3_T reg3 = read_3Register(inst);
        uint64_t id = arr[reg3.rc];
        if (id == 0) {
                id = 0x100000000;
        }
        void *key = (void *)(uintptr_t)id;

        Seq_T seg_rm = Table_remove(table, key);/* remove seg from table */
        Seq_free(&seg_rm);
        Seq_addhi(segid_bin, key);/* add used key to segid_bin*/
        *ptr = *ptr + 1;

        (void)id_counter;
        (void)notHalt;
}

/********** Output ********
 *
 * Outputs a character stored in a register. increment the program counter.
 *
 * Parameters:
 *      Table_T table:         unused, can be NULL.
 *      Seq_T segid_bin:       unused, can be NULL.
 *      uint32_t *arr:         array of registers.
 *      uint32_t *ptr:         pointer to the program counter.
 *      uint64_t *id_counter:  unused, can be NULL.
 *      Um_instruction inst:   instruction containing operation encoding.
 *      bool *notHalt:         unused, can be NULL.
 *
 * Return: void
 *
 * Expects:
 *      arr and ptr must not be NULL.
 * Notes:
 *      Outputs `arr[reg3.rc]` as a character to standard output.
 ************************/
void Output(Table_T table, Seq_T segid_bin, uint32_t* arr, uint32_t* ptr, 
            uint64_t* id_counter, Um_instruction inst, bool* notHalt)
{
        assert(table != NULL && segid_bin != NULL && arr != NULL &&
               ptr != NULL && id_counter != NULL && notHalt != NULL);
        struct Register3_T reg3 = read_3Register(inst);
        putc(arr[reg3.rc], stdout);
        *ptr = *ptr + 1;

        (void)table;
        (void)segid_bin;
        (void)id_counter;
        (void)notHalt;
}


/********** Input ********
 *
 * Reads a character and stores its value in a register.
 * increment the program counter.
 *
 * Parameters:
 *      Table_T table:         unused, can be NULL.
 *      Seq_T segid_bin:       unused, can be NULL.
 *      uint32_t *arr:         array of registers.
 *      uint32_t *ptr:         pointer to the program counter.
 *      uint64_t *id_counter:  unused, can be NULL.
 *      Um_instruction inst:   instruction containing operation encoding.
 *      bool *notHalt:         unused, can be NULL.
 *
 * Return: void
 *
 * Expects:
 *      arr and ptr must not be NULL.
 * Notes:
 *      Read a character from stdin. Store its value in `arr[reg3.rc]`.
 *      Stores `0xFFFFFFFF` if EOF is encountered.
 ************************/
void Input(Table_T table, Seq_T segid_bin, uint32_t* arr, uint32_t* ptr, 
           uint64_t* id_counter, Um_instruction inst, bool* notHalt)
{
        assert(table != NULL && segid_bin != NULL && arr != NULL &&
               ptr != NULL && id_counter != NULL && notHalt != NULL);
        struct Register3_T reg3 = read_3Register(inst);
        int c = getc(stdin);
        if (c != EOF) {
                arr[reg3.rc] = c;
        } else {
                arr[reg3.rc] = 0xFFFFFFFF;
        }
        *ptr = *ptr + 1;

        (void)table;
        (void)segid_bin;
        (void)id_counter;
        (void)notHalt;
}

/********** LoadProgram ********
 *
 * Copies a memory segment into segment 0 and sets the program counter.
 *
 * Parameters:
 *      Table_T table:         memory mapping segments to sequences.
 *      Seq_T segid_bin:       unused, can be NULL.
 *      uint32_t *arr:         array of registers.
 *      uint32_t *ptr:         pointer to the program counter.
 *      uint64_t *id_counter:  unused, can be NULL.
 *      Um_instruction inst:   instruction containing operation encoding.
 *      bool *notHalt:         unused, can be NULL.
 *
 * Return: void
 *
 * Expects:
 *      table, arr, and ptr must not be NULL.
 * Notes:
 *      Copies the segment `arr[reg3.rb]` into segment 0 if it isn't already.
 *      Updates `*ptr` to `arr[reg3.rc]`.
 ************************/
void LoadProgram(Table_T table, Seq_T segid_bin, uint32_t* arr, uint32_t* ptr, 
                 uint64_t* id_counter, Um_instruction inst, bool* notHalt)
{
        assert(table != NULL && segid_bin != NULL && arr != NULL &&
               ptr != NULL && id_counter != NULL && notHalt != NULL);
        struct Register3_T reg3 = read_3Register(inst);
        
        uint64_t id = arr[reg3.rb];
        /* if m[rb] is not m[0] */
        if (id != 0) {
                /* hard copy seg_rb to new segment seg_0 */
                void *key = (void*)(uintptr_t)id;
                Seq_T seg_rb = Table_get(table, key);
                int sizeRB = Seq_length(seg_rb);
                Seq_T new_0 = Seq_new(sizeRB);
                void *value;
                for (int i = 0; i < sizeRB; i++) {
                        value = Seq_get(seg_rb, i);
                        Seq_addhi(new_0, value);
                }
                /* replace old m[0] by new one*/
                key = (void*)(uintptr_t)0x100000000;
                Seq_T old_0 = Table_put(table, key, new_0);
                Seq_free(&old_0);
        }
        *ptr = (uint32_t)(uintptr_t)arr[reg3.rc];

        (void)table;
        (void)segid_bin;
        (void)id_counter;
        (void)notHalt;
}

/********** LoadValue ********
 *
 * Stores an immediate value in a register. increment the program counter.
 *
 * Parameters:
 *      Table_T table:         unused, can be NULL.
 *      Seq_T segid_bin:       unused, can be NULL.
 *      uint32_t *arr:         array of registers.
 *      uint32_t *ptr:         pointer to the program counter.
 *      uint64_t *id_counter:  unused, can be NULL.
 *      Um_instruction inst:   instruction containing operation encoding.
 *      bool *notHalt:         unused, can be NULL.
 *
 * Return: void
 *
 * Expects:
 *      arr and ptr must not be NULL.
 * Notes:
 *      Stores `rv.value` into `arr[rv.ra]`.
 ************************/
void LoadValue(Table_T table, Seq_T segid_bin, uint32_t* arr, uint32_t* ptr, 
               uint64_t* id_counter, Um_instruction inst, bool* notHalt)
{
        assert(table != NULL && segid_bin != NULL && arr != NULL &&
               ptr != NULL && id_counter != NULL && notHalt != NULL);
        struct RegVal_T rv = read_RegVal(inst);
        arr[rv.ra] = rv.value;
        *ptr = *ptr + 1;

        (void)table;
        (void)segid_bin;
        (void)id_counter;
        (void)notHalt;
}

/********** vfree ********
 * helper funtion for halt.
 * Frees a sequence in the memory table. 
 *
 * Parameters:
 *      const void *key:       unused, can be NULL.
 *      void **value:          pointer to the sequence to be freed.
 *      void *cl:              unused, can be NULL.
 *
 * Return: void
 *
 * Expects:
 *      value must not be NULL.
 * Notes:
 *      Frees the sequence pointed to by `value`.
 ************************/
void vfree(const void *key, void **value, void *cl)
{
        Seq_T seq = *value;
        Seq_free(&seq);
        (void)key;
        (void)cl;
}
