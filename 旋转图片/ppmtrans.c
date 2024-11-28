/*
 *     ppmtrans.c
 * 
 *     Purpose: Implementation for ppmtrans. Defines image transformations that
 *              can be performed on a given ppm image. Utilizes the pnm
 *              interface to handle reading, writing, and freeing the image
 *              files. Supports row-major, column-major, and block-major mapping
 *              by utilizing A2Methods, which can represent a UArray2 or a
 *              UArray2b. Supports a timer function to determine how long a
 *              given transformation takes.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "assert.h"
#include "a2methods.h"
#include "a2plain.h"
#include "a2blocked.h"
#include "pnm.h"
#include "cputiming.h"

typedef A2Methods_UArray2 A2;

#define SET_METHODS(METHODS, MAP, WHAT) do {                    \
        methods = (METHODS);                                    \
        assert(methods != NULL);                                \
        map = methods->MAP;                                     \
        if (map == NULL) {                                      \
                fprintf(stderr, "%s does not support "          \
                                WHAT "mapping\n",               \
                                argv[0]);                       \
                exit(1);                                        \
        }                                                       \
} while (false)

static void
usage(const char *progname)
{
        fprintf(stderr, "Usage: %s [-rotate <angle>] "
                        "[-{row,col,block}-major] "
                        "[-time time_file] "
                        "[filename]\n",
                        progname);
        exit(1);
}

enum flip {none = 0, horizontal = 1, vertical = 2, transpose = 3};
char* flip_type_name[4] = {"none", "horizontal", "vertical", "transpose"};

/*
 * struct to store necessary data which will be used in functions rotate
 * and flip. This struct is created in func rotate_image or flip_image and
 * passed into rotate and flip.
 */
struct my_closure {
        unsigned width, height;
        A2 pixels;
        A2Methods_T methods;
        int rotation;
        enum flip flip_type;
};
void rotate_image(Pnm_ppm pixmap, int rotation, A2Methods_mapfun map,
                  A2Methods_T methods);
void flip_image(Pnm_ppm pixmap, enum flip flip_type, A2Methods_mapfun map, 
                A2Methods_T methods);
void rotate(int i, int j, A2 array2, A2Methods_Object *ptr, void *cl);
void flip(int i, int j, A2 array2, A2Methods_Object *ptr, void *cl);
void swap(unsigned *a, unsigned *b);
void report(Pnm_ppm pixmap, A2Methods_mapfun map, enum flip flip_type,
            FILE *timings_file, int rotation, double time_used);

int main(int argc, char *argv[])
{
        char *time_file_name = NULL;
        int rotation = 0; /* default rotation */
        enum flip flip_type = none;
        int i;

        /* default to UArray2 methods */
        A2Methods_T methods = uarray2_methods_plain; 
        assert(methods != NULL);

        /* default to best map */
        A2Methods_mapfun *map = methods->map_default; 
        assert(map != NULL);

        for (i = 1; i < argc; i++) {
                if (strcmp(argv[i], "-row-major") == 0) {
                        SET_METHODS(uarray2_methods_plain, map_row_major, 
                                    "row-major");
                } else if (strcmp(argv[i], "-col-major") == 0) {
                        SET_METHODS(uarray2_methods_plain, map_col_major, 
                                    "column-major");
                } else if (strcmp(argv[i], "-block-major") == 0) {
                        SET_METHODS(uarray2_methods_blocked, map_block_major,
                                    "block-major");
                } else if (strcmp(argv[i], "-rotate") == 0) {
                        if (!(i + 1 < argc)) {      /* no rotate value */
                                usage(argv[0]);
                        }
                        char *endptr;
                        rotation = strtol(argv[++i], &endptr, 10);
                        if (!(rotation == 0 || rotation == 90 ||
                            rotation == 180 || rotation == 270)) {
                                fprintf(stderr, 
                                        "Rotation must be 0, 90 180 or 270\n");
                                usage(argv[0]);
                        }
                        if (!(*endptr == '\0')) {    /* Not a number */
                                usage(argv[0]);
                        }
                } else if (strcmp(argv[i], "-flip") == 0) {
                        if (!(i + 1 < argc)) {      /* No flip type */
                                usage(argv[0]);
                        }
                        i++;
                        if (strcmp(argv[i], "horizontal") == 0) {
                                flip_type = horizontal;
                        } else if (strcmp(argv[i], "vertical") == 0) {
                                flip_type = vertical;
                        } else {
                                usage(argv[0]);
                        }
                } else if (strcmp(argv[i], "-transpose") == 0) {
                        flip_type = transpose;
                } else if (strcmp(argv[i], "-time") == 0) {
                        if (!(i + 1 < argc)) {      /* no time file */
                                usage(argv[0]);
                        }
                        time_file_name = argv[++i];
                } else if (*argv[i] == '-') {
                        fprintf(stderr, "%s: unknown option '%s'\n", argv[0],
                                argv[i]);
                        usage(argv[0]);
                } else if (argc - i > 1) {
                        fprintf(stderr, "Too many arguments\n");
                        usage(argv[0]);
                } else {
                        break;
                }
        }
        FILE *fp;
        Pnm_ppm pixmap;
        if (i == argc) {
                pixmap = Pnm_ppmread(stdin, methods);
        } else {
                fp = fopen(argv[i], "r");
                if (fp == NULL) {
                        fprintf(stderr, "Unable to open %s\n", argv[i]);
                        exit(EXIT_FAILURE);
                }
                pixmap = Pnm_ppmread(fp, methods);
                fclose(fp);
        }
        CPUTime_T timer;
        double time_used; 
        FILE *timings_file = NULL;
        if (time_file_name != NULL) {
                timings_file = fopen(time_file_name, "a");
                if (timings_file != NULL) {
                        timer = CPUTime_New();
                        CPUTime_Start(timer);   
                }
        }

        /* Only one transformation is given at the command line, so we can
           check for either a flip transformation or a rotation */
        if (flip_type != none) {
                flip_image(pixmap, flip_type, map, methods);
        } else {
                rotate_image(pixmap, rotation, map, methods); 
        }
        if (timings_file != NULL) {
                time_used = CPUTime_Stop(timer);
                report(pixmap, map, flip_type, timings_file, rotation, 
                       time_used);
                CPUTime_Free(&timer);
                fclose(timings_file);
        }
        Pnm_ppmwrite(stdout, pixmap);
        Pnm_ppmfree(&pixmap);
        return 0;
}

/********** rotate_image ********
 *
 * Purpose: Perform a given rotation on an image.
 *
 * Parameters: 
 *      Pnm_ppm pixmap: The ppm representing the original image.
 *      int rotation: The degree of rotation to be performed.
 *      A2Methods_mapfun map: Mapping function to be called on every pixel.
 *      A2Methods_T methods: Interface containing all the methods needed.
 * 
 * Return: None.
 *      
 * Expects: The rotation is either 0, 90, 180, or 270.
 * 
 * Notes: Swaps the width and height dimensions if the rotation is 90 degrees
 *        or 270 degrees. Utilizes a rotate apply function to be called on every
 *        pixel.
 *      
 ************************/
void rotate_image(Pnm_ppm pixmap, int rotation, A2Methods_mapfun map, 
                A2Methods_T methods)
{
        assert(pixmap != NULL);
        if (rotation == 0) {
                return;
        } else if (rotation == 90 || rotation == 270) {
                A2 new_pixels = methods->new(pixmap->height, pixmap->width,
                                             sizeof(struct Pnm_rgb));
                struct my_closure cl = {pixmap->width, pixmap->height, 
                                        new_pixels, methods, rotation, none};
                map(pixmap->pixels, rotate, &cl);
                methods->free(&(pixmap->pixels));
                pixmap->pixels = new_pixels;
                swap(&pixmap->width, &pixmap->height);

        } else if (rotation == 180) {
                A2 new_pixels = methods->new(pixmap->width, pixmap->height,
                                             sizeof(struct Pnm_rgb));
                struct my_closure cl = {pixmap->width, pixmap->height, 
                                        new_pixels, methods, rotation, none};
                map(pixmap->pixels, rotate, &cl);
                methods->free(&(pixmap->pixels));
                pixmap->pixels = new_pixels;
        }
}

/********** flip_image ********
 *
 * Purpose: Perform a flip transformation on an image.
 *
 * Parameters: 
 *      Pnm_ppm pixmap: The ppm representing the original image.
 *      enum flip flip_type: The flip transformation to be performed.
 *      A2Methods_mapfun map: Mapping function to be called on every pixel.
 *      A2Methods_T methods: Interface containing all the methods needed.
 * 
 * Return: None.
 *      
 * Expects: The image is being flipped horizontally, vertically, or transposed
 *          across the UL-to-LR axis.
 * 
 * Notes: Swaps the width and height dimensions if the image is being transposed
 *        across the UL-to-LR axis. Utilizes a flip apply function to be called
 *        on every pixel.
 *      
 ************************/
void flip_image(Pnm_ppm pixmap, enum flip flip_type, A2Methods_mapfun map, 
                A2Methods_T methods)
{
        assert(pixmap != NULL);
        if (flip_type == none) {
                return;
        } else if (flip_type == horizontal || flip_type == vertical) {
                A2 new_pixels = methods->new(pixmap->width, pixmap->height,
                                             sizeof(struct Pnm_rgb));
                struct my_closure cl = {pixmap->width, pixmap->height, 
                                        new_pixels, methods, 0, flip_type};
                map(pixmap->pixels, flip, &cl);
                methods->free(&(pixmap->pixels));
                pixmap->pixels = new_pixels;
        } else if (flip_type == transpose) {
                A2 new_pixels = methods->new(pixmap->height, pixmap->width,
                                             sizeof(struct Pnm_rgb));
                struct my_closure cl = {pixmap->width, pixmap->height, 
                                        new_pixels, methods, 0, flip_type};
                map(pixmap->pixels, flip, &cl);
                methods->free(&(pixmap->pixels));
                pixmap->pixels = new_pixels;
                swap(&pixmap->width, &pixmap->height);
        }
}

/********** rotate ********
 *
 * Purpose: Given a pixel of the original image, set its position in the
 *          rotated image.
 *
 * Parameters: 
 *      int i: Column index of the pixel.
 *      int j: Row index of the pixel.
 *      A2 array2: Instance of a two-dimensional unboxed array.
 *      A2Methods_Object *ptr: Pointer to the original pixel.
 *      void *cl: Closure representing a struct that contains information 
 *                needed to perform the transformation.
 * 
 * Return: None.
 *      
 * Expects: Called as an apply function for a mapping function.
 * 
 * Notes: array2 is voided at the start.
 *      
 ************************/
void rotate(int i, int j, A2 array2, A2Methods_Object *ptr, void *cl)
{
        struct my_closure *mycl = cl;
        A2Methods_T methods = mycl->methods;
        A2 new_pixels = mycl->pixels;
        unsigned h = mycl->height;
        unsigned w = mycl->width;
        int rotation = mycl->rotation;
        (void) array2;
        
        Pnm_rgb new_pixel;
        if (rotation == 90) {
                new_pixel = methods->at(new_pixels, h - j - 1, i);
        } else if (rotation == 180) {
                new_pixel = methods->at(new_pixels, w - i - 1, h - j - 1);
        } else if (rotation == 270) {
                new_pixel = methods->at(new_pixels, j, w - i - 1);
        }
        *new_pixel = *(Pnm_rgb)ptr;
}

/********** flip ********
 *
 * Purpose: Given a pixel of the original image, set its position in the
 *          flipped image.
 *
 * Parameters: 
 *      int i: Column index of the pixel.
 *      int j: Row index of the pixel.
 *      A2 array2: Instance of a two-dimensional unboxed array.
 *      A2Methods_Object *ptr: Pointer to the original pixel.
 *      void *cl: Closure representing a struct that contains information needed
 *                to perform the transformation.
 * 
 * Return: None.
 * 
 * Expects: Called as an apply function for a mapping function.
 * 
 * Notes: array2 is voided at the start.
 *      
 ************************/
void flip(int i, int j, A2 array2, A2Methods_Object *ptr, void *cl)
{
        struct my_closure *mycl = cl;
        A2Methods_T methods = mycl->methods;
        A2 new_pixels = mycl->pixels;
        unsigned h = mycl->height;
        unsigned w = mycl->width;
        enum flip flip_type = mycl->flip_type;
        (void) array2;

        Pnm_rgb new_pixel;
        if (flip_type == horizontal) {
                new_pixel = methods->at(new_pixels, w - i - 1, j);
        } else if (flip_type == vertical) {
                new_pixel = methods->at(new_pixels, i, h - j - 1);
        } else if (flip_type == transpose) {
                new_pixel = methods->at(new_pixels, j, i);
        }
        *new_pixel = *(Pnm_rgb)ptr;
}

/********** swap ********
 *
 * Purpose: Swap the width and height dimensions of an image.
 *
 * Parameters: 
 *      unsigned *w: Width of the image.
 *      unsigned *h: Height of the image.
 * 
 * Return: None.
 * 
 * Expects:
 * 
 * Notes: Dereferences pointers to change the values of the parameters.
 *      
 ************************/
void swap(unsigned *w, unsigned *h) 
{
        unsigned temp = *w;
        *w = *h;
        *h = temp;        
}

/********** report ********
 *
 * Purpose: Used to write timer information into a file.
 *
 * Parameters: 
 *      Pnm_ppm pixmap: Pnm_ppm struct representing transformed image
 *      A2Methods_mapfun map: Map function to be used
 *      enum flip flip_type: Represents a potential flip transformation
 *      FILE *timings_file: Pointer to the file being written to
 *      int rotation: Represents the degree of a potential rotation
 *      double time-used: Represents the amount of time passed since timer was
 *                        started
 * 
 * Return: None.
 *      
 * Expects: Input file has already been opened to append to.
 * 
 * Notes: 
 *      
 ************************/
void report(Pnm_ppm pixmap, A2Methods_mapfun map, enum flip flip_type,
            FILE *timings_file, int rotation, double time_used)
{       
        assert(pixmap != NULL);
        assert(timings_file != NULL);
        unsigned h = pixmap->height;
        unsigned w = pixmap->width;
        double num_pixels = w * h;
        const struct A2Methods_T *methods = pixmap->methods;
        fprintf(timings_file, "width: %u height: %u\n", w, h);
        if (flip_type != none) {
                fprintf(timings_file, "flip %s ", 
                flip_type_name[flip_type]);
        } else {
                fprintf(timings_file, "rotate %d ", rotation);
        }

        if (map == methods->map_row_major) {
                fprintf(timings_file, "row-major\n");
        } else if (map == methods->map_col_major) {
                fprintf(timings_file, "col-major\n");
        } else if (map == methods->map_block_major) {
                fprintf(timings_file, "block-major\n");
        }

        fprintf(timings_file, "TIME: %.0f nanoseconds\n", time_used);
        fprintf(timings_file, 
                "time per input pixel: %f nanoseconds\n\n", 
                time_used / num_pixels);
}
