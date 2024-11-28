/**************************************************************
 *
 *     read.c
 *
 *
 *     The `read.c` file implements the `readUM` function, which reads a UM 
 *     instruction file and stores the instructions in a table. 
 *     It handles file validation, reads data in 32-bit chunks, 
 *     and stores it in a sequence within segment 0, ensuring 
 *     proper error handling for file and input issues.     
 *
 **************************************************************/

#include "read.h"

/********** readUM ********
 * read UM instruction sets and store into segment 0
 * which is implemented by Hanson's Sequence
 * 
 * Parameters:
 *      char* filename：a string represents file that we want to read
 *      Table_t table: store all segments 
 *     
 * Return: 
 *      None
 * Expects:
 *      - If we fail to open UM file, raise exception 
 *      - If table is NULL, raise exception
 *
 * Notes:
 *      - filename should be a um file. Extension should be .um .
 *      - read each four byte and store into sequence
 *
 ************************/
void readUM(char* filename, Table_T table)
{
        struct stat sb;
        /* stat read info of file. 
           if 0 is returned, on success; 
           if -1 is returned, on error. */
        assert(stat(filename, &sb) == 0);
        assert(table != NULL);
        uint64_t inst_size = sb.st_size / 4;
        Seq_T inst_set = Seq_new(inst_size);
        assert(inst_set != NULL);

        FILE *fp = fopen(filename, "rb");
        assert(fp != NULL);

        uint32_t word;
        for(uint64_t i = 0; i < inst_size; i++) {
                word = 0;
                for (int j = 0; j < 4; j++) {
                        uint8_t c = getc(fp);
                        word = word << 8;
                        word |= c;
                }
                Seq_addhi(inst_set, (void *)(uintptr_t)word);
        }
        
        fclose(fp);

        uint64_t id = 0;
        uint64_t offset = 0x100000000;
        /* to avoid (void *)(uintptr_t)(id) to be null pointer, we add offset*/
        id = id + offset;
        Table_put(table, (void *)(uintptr_t)(id), inst_set);
}
