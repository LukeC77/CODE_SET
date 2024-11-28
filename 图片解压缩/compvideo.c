/*
 *     compvideo.c
 * 
 *     Purpose: Implementation for compvideo. Defines two functions that allow
 *              for transformation between RGB color space and component video
 *              color space. On the compression side, information is lost in
 *              converting to floats and performing floating point divisoin. On
 *              the decompression side, RGB values are bound within a certain
 *              range.
 */

#include "compvideo.h"

typedef A2Methods_UArray2 A2;

struct mycl {
        Pnm_ppm pixmap; /* Pixmap of the original image with RGB values */
        A2Methods_T methods; /* Methods to act on a UArray2 */
};

void convertCV(int col, int row, A2 array2, A2Methods_Object *ptr, void *cl);
void convertRGB(int col, int row, A2 array2, A2Methods_Object *ptr, void *cl);
float roundRGB(float x, unsigned denom);

/********** rgbToCV ********
 *
 * Purpose: Convert the RGB values of each pixel in the pixmap to component
 *          video.
 *
 * Parameters: 
 *      Pnm_pixmap pixmap: Pixmap of the original image.
 * 
 * Return: A 2D array with all of the pixels of the image in component video.
 *      
 * Expects: The pixmap is not empty and all of the pixels have an unsigned red,
 *          green, and blue value.
 * 
 * Notes: Initializes a 2D blocked array with blocksize 2. Uses the height and
 *        width of the pixmap but trims off any odd column or row. Utilizes an
 *        apply function to get all of the pixels in the pixmap to component
 *        video form. Information may be lost at this compression step from 
 *        trimming off an odd column or row, and casting the RGB values from
 *        ints to floats.
 *      
 ************************/
A2 rgbToCV(Pnm_ppm pixmap)
{
        assert(pixmap != NULL);
        A2Methods_T methods = uarray2_methods_blocked;
        assert(methods != NULL);
        int w = pixmap->width;
        int h = pixmap->height;
        if (w % 2 == 1) {
                w -= 1;
        }
        if (h % 2 == 1) {
                h -= 1;
        }
        A2 new_pixels = methods->new_with_blocksize(w, h, 
                                                    sizeof(struct Component_T), 
                                                    2);
        assert(new_pixels != NULL);
        struct mycl cl = {pixmap, methods};
        methods->map_block_major(new_pixels, convertCV, &cl);
        return new_pixels;
}

/********** cvToRGB ********
 *
 * Purpose: Convert each pixel of a 2D array from component video back to
 *          RGB values.
 *
 * Parameters: 
 *      A2 arrayCV: 2D array of which each element contains the component video
 *                  values of a pixel.
 *      Pnm_ppm pixmap: The decompressed image pixmap to hold RGB values.
 * 
 * Return: None.
 *      
 * Expects: The 2D array is not empty and the elements have component video
 *          color values.
 * 
 * Notes: Only the average of the chroma elements is known, so the RGB values
 *        for the four pixels of a given 2-by-2 block will be the same. The RGB
 *        values are stored in pixmap->pixels. Utilizes an apply function to
 *        visit each element in arrayCV in block-major order and fill in the
 *        pixels of pixmap in the same order.
 *      
 ************************/
void cvToRGB(A2 arrayCV, Pnm_ppm pixmap)
{
        assert(arrayCV != NULL && pixmap != NULL);
        A2Methods_T methods = uarray2_methods_blocked;
        assert(methods != NULL);
        struct mycl cl = {pixmap, methods};
        methods->map_block_major(arrayCV, convertRGB, &cl);
}

/********** convertCV ********
 *
 * Purpose: For a given element of the 2D array, set the component video values
 *          (Y/Pb/Pr) based on the RGB values of the pixel at the corresponding
 *          indices in the pixmap of the original image.
 *
 * Parameters: 
 *      int col: Column index of the element
 *      int row: Row index of the element.
 *      A2 array2: The 2D array to hold component video objects. Set to void.
 *      A2Methods_Object *ptr: Pointer to an element in the 2D array.
 *      void *cl: Pointer to the mycl struct.
 *      
 * Return: None.
 *      
 * Expects: Using methods->at, the pixel at the corresponding position in pixmap
 *          can be obtained. For the component video values calculated, Y will
 *          be in the range 0 to 1 and Pb/Pr will be in the range -0.5 to 0.5.
 * 
 * Notes: A2 array2 is set to void at the start because only one specific pixel
 *        is needed. Converts the RGB values to be floats and then divide by the
 *        denominator of the pixmap to get scaled RGB values. Convert to
 *        component video by using the given transformations.
 *      
 ************************/
void convertCV(int col, int row, A2 array2, A2Methods_Object *ptr, void *cl)
{
        (void) array2;
        struct mycl *closure = cl;
        A2Methods_T methods = closure->methods;
        assert(methods != NULL);
        Pnm_ppm pixmap = closure->pixmap;
        unsigned denom = pixmap->denominator;
        struct Pnm_rgb *pixel = methods->at(pixmap->pixels, col, row);
        /* Convert RGB to scaled RGB by dividing by the denominator of the
           pixmap, Y is in the range 0 to 1, Pb/Pr in the range -0.5 to 0.5 */
        float r = (float) pixel->red / denom;
        float g = (float) pixel->green / denom;
        float b = (float) pixel->blue / denom;
        
        Component_T cp = ptr;
        cp->Y = 0.299 * r + 0.587 * g + 0.114 * b;
        cp->Pb = -0.168736 * r - 0.331264 * g + 0.5 * b;
        cp->Pr = 0.5 * r - 0.418688 * g - 0.081312 * b;
}

/********** convertRGB ********
 *
 * Purpose: For a given element of the 2D array, use the component video values
 *          to calculate the RGB values of the pixel at the corresponding
 *          indices in the pixmap.
 *
 * Parameters: 
 *      int col: Column index of the element
 *      int row: Row index of the element.
 *      A2 array2: The 2D array with component video objects. Set to void.
 *      A2Methods_Object *ptr: Pointer to an element in the 2D array.
 *      void *cl: Pointer to the mycl struct.
 *      
 * Return: None.
 *      
 * Expects: Using methods->at, the pixel at the corresponding position in pixmap
 *          can be obtained. The RGB values will range from 0 to the denominator
 *          of the pixmap.
 * 
 * Notes: A2 array2 is set to void at the start because only one specific pixel
 *        is needed. Convert from component video to RGB by using the given
 *        transformations. Using the helper functions, we bind any RGB values to
 *        be between 0 and the denominator.
 *      
 ************************/
void convertRGB(int col, int row, A2 array2, A2Methods_Object *ptr, void *cl)
{
        (void) array2;
        struct mycl *closure = cl;
        A2Methods_T methods = closure->methods;
        assert(methods != NULL);
        Pnm_ppm pixmap = closure->pixmap;
        unsigned denom = pixmap->denominator;
        Component_T cp = ptr;
        struct Pnm_rgb *pixel = methods->at(pixmap->pixels, col, row);
        /* multiply by denominator to scale up value of RGB */
        float r = (1.0 * cp->Y + 0.0 * cp->Pb + 1.402 * cp->Pr) * denom;
        float g = (1.0 * cp->Y - 0.344136 * cp->Pb - 0.714136 * cp->Pr) * denom;
        float b = (1.0 * cp->Y + 1.772 * cp->Pb + 0.0 * cp->Pr) * denom;

        pixel->red = (unsigned) roundRGB(r, denom);
        pixel->green = (unsigned) roundRGB(g, denom);
        pixel->blue = (unsigned) roundRGB(b, denom);
}

/********** roundRGB ********
 *
 * Purpose: Bind a given float to fit within a certain range.
 *
 * Parameters: 
 *      float x: The float being bound.
 *      unsigned denom: The upper bound of the range.
 *      
 * Return: None.
 *      
 * Expects: The denominator is not negative.
 * 
 * Notes: Sets x to be 0 if it is less than 0, and sets x to be the denominator
 *        if x is greater than the denominator.
 *      
 ************************/
float roundRGB(float x, unsigned denom)
{
        if (x >= 0 && x <= denom) {
                return x;
        } else if (x < 0) {
                return 0;
        } else {
                return denom;
        }
}