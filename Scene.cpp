#pragma once
#include "dependencies.h"

// The Scene struct manages objects and lights in the scene.
struct Scene {

    std::vector<Object*> objects;      // List of objects in the scene
    std::vector<Object*> area_lights; // List of area lights in the scene

    // Add an object to the scene
    void addObject(Object* obj);

    // Add an area light to the scene
    void addAreaLight(Object* light);

    // Add triangles of the box to the scene
    void addBox(Box* box);

    // Find the closest object that the ray intersects with
    void rayTarget(Ray& ray) const;

    // Calculate local lighting contribution for a ray
    dvec3 localLighting(Ray& ray) const;

    // Trace rays in the scene and calculate radiance
    void traceRay(std::shared_ptr<Ray>& root) const;
};

// Add an object to the scene
void Scene::addObject(Object* obj) {
    objects.push_back(obj);
}

// Add an area light to the scene
void Scene::addAreaLight(Object* light) {
    area_lights.push_back(light);
    // Add lights to the scene to see them
    objects.push_back(light);
}

// Add triangles of the box to the scene
void Scene::addBox(Box* box) {
    for (int i = 0; i < box->triangles.size(); ++i) {
        addObject(&box->triangles[i]);
    }
}

// Find the closest object that the ray intersects with
void Scene::rayTarget(Ray& ray) const {
    float closest_obj = std::numeric_limits<double>::max();

    for (Object* obj : this->objects) {
        float hitX = obj->rayIntersection(&ray);
        if (hitX > 0.0f && hitX < closest_obj) {
            closest_obj = hitX;
            ray.target = obj;
            ray.setEnd(closest_obj);
        }
    }
}

// Calculate local lighting contribution for a ray
dvec3 Scene::localLighting(Ray& ray) const {
    dvec3 final_color = black;

    if (ray.target == nullptr) return final_color;

    if (ray.target->material->emittance != 0.0) {
        return ray.target->material->color;
    }

    for (Object* light_source : this->area_lights) {

        dvec3 this_light = black;

        vec3 end_offset = ray.end + ray.target->getNormal(ray.end) * ray_offset;
        std::vector<Ray> shadow_rays = light_source->generateShadowRays(end_offset);

        for (Ray& sr : shadow_rays) {

            bool occluded = false;
            float target_length = glm::length(sr.end - sr.start);

            vec3 targetNormal = ray.target->getNormal(ray.end);
            vec3 lightNormal = light_source->getNormal(sr.end);

            double alpha = glm::dot(targetNormal, sr.direction);
            double beta = glm::dot(-sr.direction, lightNormal);
            double cos_term = alpha * beta;
            cos_term = glm::max(cos_term, 0.0);

            for (Object* shadow_object : this->objects) {

                Ray temp = sr;

                if (shadow_object->material->emittance == 0.0) {
                    float hit_x = shadow_object->rayIntersection(&sr);

                    temp.setEnd(hit_x);

                    if (hit_x > epsilon && glm::length(temp.end - temp.start) < target_length) {
                        occluded = true;
                        break;
                    }
                }
            }

            if (!occluded) {
                double dropoff = glm::pow(glm::length(sr.end - sr.start), 2);
                this_light += light_source->material->emittance * cos_term * light_source->material->color / (dropoff * area_lights.size());
            }
        }
        final_color += this_light / static_cast<double>(shadow_rays.size());
    }
    return final_color * ray.target->material->color;
}

// Trace rays in the scene and calculate radiance
void Scene::traceRay(std::shared_ptr<Ray>& root) const {
    std::shared_ptr<Ray> current = root;

    while (true) {
        if (current->importance < treshold_importance) {
            this->rayTarget(*current);

            if (current->target == nullptr) {
                break;
            }
            current->is_leaf = true;

            dvec3 local_color = this->localLighting(*current);
            current->radiance = glm::length(local_color);
            current->color = local_color;
            current = current->parent;
        }
        else if (current->children.size() == 0) {
            this->rayTarget(*current);

            if (current->target == nullptr) {
                break;
            }
            std::vector<Ray> child_rays = current->target->material->brdf(current);

            for (Ray& r : child_rays) {
                r.parent = current;
                current->children.push_back(std::make_shared<Ray>(r));
            }
        }
        else {
            bool evaluate = true;
            for (std::shared_ptr<Ray>& child : current->children) {
                if (!child->is_leaf) {
                    current = child;
                    evaluate = false;
                    break;
                }
            }

            if (evaluate) {
                dvec3 local_color = this->localLighting(*current);
                current->radiance = glm::length(local_color);

                for (std::shared_ptr<Ray>& child : current->children) {
                    current->radiance += child->radiance * child->importance / current->importance;

                }
                current->color = local_color * current->radiance;
                for (std::shared_ptr<Ray>& child : current->children) {
                    current->color += child->color * g_color_contribution;
                    child.reset();
                }

                if (current->parent == nullptr) {
                    current.reset();
                    break;
                }
                current->is_leaf = true;
                current = current->parent;
            }
        }
    }
}
