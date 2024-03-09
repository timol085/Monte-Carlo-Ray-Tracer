#pragma once
#include "dependencies.h"

// Define a struct named Ray to represent a ray in the scene.
struct Ray
{
    dvec3 color;             // Color of the ray.
    vec3 start;              // Starting point of the ray.
    vec3 end;                // End point of the ray.
    vec3 direction;          // Direction vector of the ray.

    double importance;       // Importance of the ray.
    double radiance;         // Radiance of the ray.

    int depth{ 0 };          // Depth of the ray.
    bool is_leaf{ false };   // Indicates if the ray is a leaf node.
    bool inside_object{ false }; // Indicates if the ray is inside an object.

    Object* target;          // Pointer to the target object that the ray intersects.

    std::shared_ptr<Ray> parent;                 // Pointer to the parent ray.
    std::vector<std::shared_ptr<Ray>> children;  // Vector of child rays.

    // Default constructor for Ray.
    Ray() = default;

    // Constructor with start and end points.
    Ray(vec3 start, vec3 end);

    // Constructor with start point and direction, along with importance.
    Ray(vec3 start, vec3 direction, double importance);

    // Set the end point of the ray based on a given parameter t.
    void setEnd(const float t);
};
