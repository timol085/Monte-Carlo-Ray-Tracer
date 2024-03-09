#include "dependencies.h"
#include "EasyBMP.hpp"

// Constructor: Initialize camera with observer positions
Camera::Camera(vec3 obs1, vec3 obs2)
    : obs1{ obs1 }, obs2{ obs2 }, main_obs{ true }
{
    // Define camera plane corners
    plane[0][0] = vec3(0.0f, -1.0f, -1.0f); // Left bottom corner
    plane[0][1] = vec3(0.0f, 1.0f, -1.0f);  // Right bottom corner
    plane[1][0] = vec3(0.0f, -1.0f, 1.0f);  // Left top corner
    plane[1][1] = vec3(0.0f, 1.0f, 1.0f);   // Right top corner

    // Initialize pixels array and fill with black color
    pixels = new std::array<Pixel, res * res>();
    pixels->fill(Pixel{ dvec3(0.0, 0.0, 0.0) });
}

// Render the scene within specified bounds
void Camera::render(Scene& scene, size_t xLowerBound, size_t xUpperBound, size_t yLowerBound, size_t yUpperBound) {
    vec3 start = main_obs ? obs1 : obs2;

    int pixel_dimensions = sqrt(samples);
    float pixel_size{ glm::distance(this->plane[0][0], this->plane[0][1]) / res };
    float pixel_sample_size{ pixel_size / pixel_dimensions };

    for (size_t j = yLowerBound; j < yUpperBound; ++j) {
        for (size_t i = xLowerBound; i < xUpperBound; ++i) {
            Pixel& p = getPixel(i, j);

            for (int k = 0; k < samples; ++k) {
                float xEnd = static_cast<float>(i)* pixel_size - (1.0f - pixel_size) + static_cast<float>(k % pixel_dimensions)* pixel_sample_size;
                float yEnd = static_cast<float>(j)* pixel_size - (1.0f - pixel_size) + static_cast<float>(floor((k / pixel_dimensions)))* pixel_sample_size;

                xEnd += static_cast<float>(rand() / RAND_MAX)* pixel_sample_size;
                yEnd += static_cast<float>(rand() / RAND_MAX)* pixel_sample_size;

                vec3 end{ 0.0f, xEnd, yEnd };

                Ray ray{ start, end };
                scene.rayTarget(ray);
                std::shared_ptr<Ray> ray_ptr = std::make_shared<Ray>(ray);

                scene.traceRay(ray_ptr);
                p.color += ray_ptr->color / static_cast<double>(samples);
            }
        }
    }
}

// Accessor function to get a specific pixel by indices
Pixel& Camera::getPixel(size_t i, size_t j) {
    return (*pixels)[i * res + j];
}

// Create and save the image to a file
void Camera::createImage(const char* filepath) {
    EasyBMP::RGBColor rgb(0, 0, 0);
    EasyBMP::Image image(res, res, filepath, rgb);

    double max_intensity = 0.0;

    for (size_t i = 0; i < res; ++i) {
        for (size_t j = 0; j < res; ++j) {
            Pixel& p = getPixel(i, j);

            p.color = glm::sqrt(p.color);

            max_intensity = std::max({ max_intensity, p.color.r, p.color.g, p.color.b });
        }
    }

    for (size_t i = 0; i < res; ++i) {
        for (size_t j = 0; j < res; ++j) {
            Pixel& p = getPixel(i, j);

            rgb = EasyBMP::RGBColor(
                static_cast<uint8_t>(p.color.r * 255.99 / max_intensity),
                static_cast<uint8_t>(p.color.g * 255.99 / max_intensity),
                static_cast<uint8_t>(p.color.b * 255.99 / max_intensity)
            );

            image.SetPixel(i, j, rgb);
        }
    }

    image.Write();
}
