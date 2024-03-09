#pragma once
#include "dependencies.h"
#include <iostream>
#include <fstream>
#include <array>

// Structure representing a Camera
struct Camera {
    vec3 plane[2][2]; // 2x2 plane represented by vectors
    vec3 obs1;        // Observer 1 position
    vec3 obs2;        // Observer 2 position
    bool main_obs;    // Boolean indicating which observer is active
    std::array<Pixel, res * res >* pixels; // Pointer to an array of Pixels

    // Constructor to initialize the camera with observer positions
    Camera(vec3 obs1, vec3 obs2);

    // Sends rays from each pixel in the specified bounds
    void render(Scene& scene, size_t xLowerBound, size_t xUpperBound, size_t yLowerBound, size_t yUpperBound);

    // Creates the output RGB-image and saves it to a file
    void createImage(const char* file_path);

    // Accessor function to get a specific pixel by indices
    Pixel& getPixel(size_t i, size_t j);

    // Function to toggle between main and secondary observers
    void toggleObserver() {
        main_obs = !main_obs;
    }
};
