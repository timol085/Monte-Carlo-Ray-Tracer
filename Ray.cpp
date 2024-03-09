#include "dependencies.h"

// Constructor with start and end points
Ray::Ray(vec3 start, vec3 end)
    : start{ start },
      end{ end },
      direction{ glm::normalize(end - start) }, // Calculate direction
      importance{ 1.0 },
      radiance{ 0.0 },
      target{ nullptr },
      color{ black },
      parent{ nullptr }
{
    // Constructor body is empty because everything is initialized in the initializer list.
}

// Constructor with start point, direction, and importance
Ray::Ray(vec3 start, vec3 direction, double importance)
    : start{ start },
      end{ vec3{0.0f, 0.0f, 0.0f} }, // Initialize end to zero vector (not used in this case)
      direction{ direction },
      importance{ importance },
      radiance{ 0.0 },
      target{ nullptr },
      color{ black }
{
    // Constructor body is empty because everything is initialized in the initializer list.
}

// Function to set end point based on a parameter t
void Ray::setEnd(const float t)
{
    end = start + direction * t; // Calculate end point
}
