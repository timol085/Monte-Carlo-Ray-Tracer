#pragma once
#include "dependencies.h"

// Base Material class
struct Material
{
    dvec3 color;       // Color of the material
    double emittance;  // Emittance of the material
    double absorption; // Absorption of the material

    Material(); // Default constructor
    Material(const dvec3& color); // Constructor with color
    Material(const dvec3& color, double emittance); // Constructor with color and emittance

    // Virtual function to calculate Bidirectional Reflectance Distribution Function (BRDF)
    virtual std::vector<Ray> brdf(const std::shared_ptr<Ray>& incoming) const = 0;
};

// Mirror Material
struct Mirror : Material
{
    Mirror(); // Constructor

    // Override BRDF function for mirror material
    std::vector<Ray> brdf(const std::shared_ptr<Ray>& incoming) const override;
};

// Diffuse Lambertian Material
struct DiffuseLambertian : Material
{
    double reflectance; // Reflectance of the material

    // Constructor with color and reflectance
    DiffuseLambertian(dvec3 color, double reflectance);

    // Override BRDF function for diffuse Lambertian material
    std::vector<Ray> brdf(const std::shared_ptr<Ray>& incoming) const override;
};

// Light Material
struct Light : Material
{
    Light(dvec3 color, double emittance); // Constructor

    // Override BRDF function for light material
    std::vector<Ray> brdf(const std::shared_ptr<Ray>& incoming) const override;
};

// Glass Material
struct Glass : Material
{
    float reflected_index; // Reflected index of the material

    // Constructor with color and reflected index
    Glass(const dvec3& color, float index);

    // Override BRDF function for glass material
    std::vector<Ray> brdf(const std::shared_ptr<Ray>& incoming) const override;
};
