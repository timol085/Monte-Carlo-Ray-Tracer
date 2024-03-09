#pragma once
#include "dependencies.h"

// Define a struct named Pixel to represent a pixel in an image.
struct Pixel {

    // A member variable to store the color of the pixel in double-precision floating point format.
    dvec3 color;

    // Default constructor initializes the color to black.
    Pixel()
        : color{ dvec3(0.0, 0.0, 0.0) } {};

    // Constructor that allows specifying the color.
    Pixel(dvec3 clr)
        : color{ clr } {};
};
