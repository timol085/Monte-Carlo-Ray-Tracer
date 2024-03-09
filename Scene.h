#pragma once
#include "dependencies.h"

struct Scene {

    // Vector to hold pointers to objects in the scene
    std::vector<Object*> objects;

    // Vector to hold pointers to area lights in the scene
    std::vector<Object*> area_lights;

    // Function to add an object to the scene
    void addObject(Object* obj);

    // Function to add an area light to the scene
    void addAreaLight(Object* light);

    // Function to add a box to the scene
    void addBox(Box* box);

    // Function to set the target of a ray in the scene
    void rayTarget(Ray& ray) const;

    // Function to trace a ray through the scene
    void traceRay(std::shared_ptr<Ray>& root) const;

    // Function to calculate local lighting for a ray in the scene
    dvec3 localLighting(Ray& ray) const;
};
