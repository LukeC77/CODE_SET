#include <stdbool.h>
#include <stdio.h>
#include "assert.h"
#include "a2methods.h"
#include "a2plain.h"
#include "a2blocked.h"

//Three constants are defined
#define W 7
#define H 8
#define BS 3
// Static variables, can only be accessed inside the a2methods.c file
static A2Methods_T methods;
typedef A2Methods_UArray2 A2;

// This function is a mapping function that checks 
// if the elements in a 2D array are in increasing order 
// and increments the counter if the assertion succeeds.
static void check_and_increment(int i, int j, A2 a, void *elem, void *cl) 
{
        (void)i;
        (void)j;
        (void)a;
        int *p = elem;
        int *counter = cl;

        assert(*p == *counter);// Check if the element value is equal to the counter value
        *counter += 1;   /* NOT *counter++! */
}



//is similar to check_and_increment, 
//but it does not require the row and column indices of the 2D array, 
//only the elements themselves.
static void small_check_and_increment(void *elem, void *cl)
{
        int *p = elem;
        int *counter = cl;

        assert(*p == *counter);
        *counter += 1;   // NOT *counter++!
}

static void print(int i, int j, A2 a, void *elem, void *cl) 
{
        (void)i;
        (void)j;
        (void)a;
        int *p = elem;
        int *counter = cl;
        
        printf("elem = %d, cl = %d\n", *p, *counter);
        *counter += 1;
}

static void small_print(void *elem, void *cl)
{
        int *p = elem;
        int *counter = cl;
        
        printf("elem = %d, cl = %d\n", *p, *counter);
        *counter += 1;
}

static void double_row_major_plus()
{
        /* store increasing integers in row-major order */
        A2 array = methods->new_with_blocksize(W, H, sizeof(int), BS);


        
        int counter = 1;
        for (int j = 0; j < H; j++) { 
                for (int i = 0; i < W; i++) { /* col index varies faster */
                        int *p = methods->at(array, i, j);
                        *p = counter++;
                }
        }
        counter = 1;
        for (int j = 0; j < H; j++) {
                for (int i = 0; i < W; i++) {
                        int *p = methods->at(array, i, j);
                        assert(*p == counter);
                        counter++;
                }
        }
        if (methods->map_row_major) {
                counter = 1;
                methods->map_row_major(array, check_and_increment, &counter);
        }
        if (methods->small_map_row_major) {
                counter = 1;
                methods->small_map_row_major(array,
                                             small_check_and_increment,
                                             &counter);
        }
        methods->free(&array);
}

static void double_col_major_plus()
{
        /* store increasing integers in row-major order */
        A2 array = methods->new_with_blocksize(W, H, sizeof(int), BS);


        
        int counter = 1;
        for (int j = 0; j < H; j++) { 
                for (int i = 0; i < W; i++) { /* col index varies faster */
                        int *p = methods->at(array, i, j);
                        *p = counter++;
                }
        }
        counter = 1;
        for (int j = 0; j < H; j++) {
                for (int i = 0; i < W; i++) {
                        int *p = methods->at(array, i, j);
                        assert(*p == counter);
                        counter++;
                }
        }
        printf("map_col_major:\n");
        if (methods->map_col_major) {
                counter = 1;
                methods->map_col_major(array, print, &counter);
        }
        printf("small_map_col_major:\n");
        if (methods->small_map_col_major) {
                counter = 1;
                methods->small_map_col_major(array,
                                             small_print,
                                             &counter);
        }
        methods->free(&array);
}

static void double_block_major_plus()
{
        /* store increasing integers in row-major order */
        A2 array = methods->new_with_blocksize(W, H, sizeof(int), BS);

        int counter = 1;
        for (int j = 0; j < H; j++) { 
                for (int i = 0; i < W; i++) { /* col index varies faster */
                        int *p = methods->at(array, i, j);
                        *p = counter++;
                }
        }
        counter = 1;
        for (int j = 0; j < H; j++) {
                for (int i = 0; i < W; i++) {
                        int *p = methods->at(array, i, j);
                        assert(*p == counter);
                        counter++;
                }
        }
        printf("map_block_major:\n");
        if (methods->map_block_major) {
                counter = 1;
                methods->map_block_major(array, print, &counter);
        }

        printf("small_map_block_major:\n");
        if (methods->small_map_block_major) {
                counter = 1;
                methods->small_map_block_major(array, small_print, &counter);
        }
        methods->free(&array);
}

#if 0
static void show(int i, int j, A2 a, void *elem, void *cl) 
{
        (void)a; (void)cl;
        printf("A[%02d][%02d] == %05u -- should be %05d\n",
               i, j, *(unsigned *)elem, 1000 * i + j);
}
#endif

static inline void check(A2 a, int i, int j, unsigned n) 
{
        unsigned *p = methods->at(a, i, j);
        assert(*p == n);
}

bool has_minimum_methods(A2Methods_T m)
{
        return m->new != NULL && m->new_with_blocksize != NULL
                && m->free != NULL && m->width != NULL && m->height != NULL
                && m->size != NULL && m->blocksize != NULL && m->at != NULL;
}

bool has_small_plain_methods(A2Methods_T m) 
{
        return m->small_map_default != NULL
                && m->small_map_row_major != NULL
                && m->small_map_col_major != NULL;
}

bool has_plain_methods(A2Methods_T m) 
{
        return m->map_default != NULL
                && m->map_row_major != NULL
                && m->map_col_major != NULL;
}

bool has_small_blocked_methods(A2Methods_T m) 
{
        return m->small_map_default != NULL
                && m->small_map_block_major != NULL;
}

bool has_blocked_methods(A2Methods_T m) 
{
        return m->map_default != NULL && m->map_block_major != NULL;
}

static inline void copy_unsigned(A2Methods_T methods, A2 a,
                                 int i, int j, unsigned n) 
{
        unsigned *p = methods->at(a, i, j);
        *p = n;
}
/*
Use methods_under_test to set the global variable methods.

Use assertions to check that the implementation contains the necessary methods.

Create a two-dimensional array and write and verify specific values ​​to it.

Then two nested loops are performed to write a value based on its row and 
column position to each element and check it immediately after writing.

Finally, the double_row_major_plus() function is called to 
further test the mapping function of the two-dimensional array.
*/
static void test_methods(A2Methods_T methods_under_test) 
{
        methods = methods_under_test;
        assert(methods != NULL);
        assert(has_minimum_methods(methods));
        //check if have either small_plain or small_block method
        assert(has_small_plain_methods(methods)
               || has_small_blocked_methods(methods));
        //if have both small_plain and small_block method, assert error
        assert(!(has_small_plain_methods(methods)
                 && has_small_blocked_methods(methods)));
        //if have both plain and block method, assert error
        assert(!(has_plain_methods(methods)
                 && has_blocked_methods(methods)));
        //if there is no plain and block method, assert error
        if (!(has_plain_methods(methods) || has_blocked_methods(methods)))
                fprintf(stderr, "Some full mapping methods are missing\n");
        (void)double_row_major_plus;
        
        A2 array = methods->new_with_blocksize(W, H, sizeof(unsigned), BS);
        /* testing: size < 1 
        A2 array = methods->new_with_blocksize(W, H, 0, BS);
        */
        /* testing: UArray2b_width
        assert(7 == methods->width(array));
        assert(7 == methods->width(NULL));
        */
        /* testing: UArray2b_height
        assert(8 == methods->height(array));
        assert(8 == methods->height(NULL));
        */
        /* testing: UArray2b_size
        assert(sizeof(unsigned) == methods->size(array));
        assert(sizeof(unsigned) == methods->size(NULL));
        */
        /* testing: UArray2b_blocksize
        assert(3 == methods->blocksize(array));
        assert(3 == methods->blocksize(NULL));

        For a2plain.c specially
        assert(1 == methods->blocksize(array));
        assert(1 == methods->blocksize(NULL));
        */    
        /* testing: UArray2b_new_64K_block
        sizeof(unsigned)) == 4
        A2 array5 = methods->new(W, H, sizeof(unsigned));
        assert(128 == methods->blocksize(array5));
        */
        /* testing: new in a2plain.c
        sizeof(unsigned)) == 4
        A2 array5 = methods->new(W, H, sizeof(unsigned));
        assert(128 == methods->blocksize(array5));
        (void)array5;
        */     

        printf("-----START: double_row_major_plus------\n");
        double_row_major_plus();
        printf("-----COMPLETE: double_row_major_plus------\n\n");

        printf("-----START: double_col_major_plus------\n");
        double_col_major_plus();
        printf("-----COMPLETE: double_col_major_plus------\n\n");

        printf("-----START: double_block_major_plus------\n");
        double_block_major_plus();
        printf("-----COMPLETE: double_block_major_plus------\n\n");
        methods->free(&array);
}

int main(int argc, char *argv[])
{
        assert(argc == 1);
        (void)argv;
        // test_methods(uarray2_methods_plain);
        /*  test_methods(uarray2_methods_blocked); */
        test_methods(uarray2_methods_blocked);
        printf("Passed.\n");  
        /* only if we reach this point without
        * assertion failure
        */
        return 0;
}
