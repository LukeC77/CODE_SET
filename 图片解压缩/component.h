/*
 *     component.h
 * 
 *     Purpose: Define a struct to represent a pixel in component video color
 *              space. Used in two separate steps of the compression and
 *              decompression processes.
 */
#ifndef COMPONENT_H
#define COMPONENT_H

typedef struct Component_T {
        /* Three values of component video color space
           Allows for converison to and from RGB color space */
        float Y, Pb, Pr;
} *Component_T;

#endif