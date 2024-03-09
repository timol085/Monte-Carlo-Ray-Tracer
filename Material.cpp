#include "dependencies.h"


// Material constructor with default color and emittance
Material::Material()
    : color{ black }, emittance{ 0.0 }
{
    this->absorption = minimum_absorp + (maximum_absorp - minimum_absorp) * glm::length(color) / glm::length(white);
}

// Material constructor with specified color and default emittance
Material::Material(const dvec3& color)
    : color{ color }, emittance{ 0.0 }
{
    this->absorption = minimum_absorp + (maximum_absorp - minimum_absorp) * glm::length(color) / glm::length(white);
}


// Material constructor with specified color and emittance
Material::Material(const dvec3& color, double emittance)
    : color{ color }, emittance{ emittance }
{
    this->absorption = minimum_absorp + (maximum_absorp - minimum_absorp) * glm::length(color) / glm::length(white);
}


// Constructor for Mirror material
Mirror::Mirror()
{
    this->absorption = 0.0;
}

// BRDF function for Mirror material
std::vector<Ray> Mirror::brdf(const std::shared_ptr<Ray>& incoming) const
{
	double reflected_importance = 0.0;

	if (incoming->depth < maximum_ray_depth)
	{
		reflected_importance = incoming->importance;
	}

	Ray reflected_ray{ incoming->end + incoming->target->getNormal(incoming->end) * ray_offset,	// Start with offset
				glm::normalize(glm::reflect(incoming->end, incoming->target->getNormal(incoming->end))),	// Perfect reflection
				reflected_importance };

	reflected_ray.depth = incoming->depth + 1;

	std::vector<Ray> reflected;
	reflected.push_back(reflected_ray);

	return reflected;
}

// Constructor for DiffuseLambertian material
DiffuseLambertian::DiffuseLambertian(dvec3 color, double reflectance)
    : Material(color), reflectance{ reflectance } {
        
}

// BRDF function for DiffuseLambertian material
std::vector<Ray> DiffuseLambertian::brdf(const std::shared_ptr<Ray>& incoming) const
{

	vec3 Z = incoming->target->getNormal(incoming->end);
	vec3 X = glm::normalize(incoming->direction - glm::dot(incoming->direction, Z) * Z);
	vec3 Y = normalize(cross(-X, Z));

	// Local coordinates
	glm::mat4 M{ X.x, X.y, X.z, 0.0f,
				Y.x, Y.y, Y.z, 0.0f,
				Z.x, Z.y, Z.z, 0.0f,
				0.0f, 0.0f, 0.0f, 1.0f };

	std::vector<Ray> reflected;

	for (int i = 0; i < diff_bounces; ++i)
	{
		double phi = (static_cast<double>(rand()) / RAND_MAX) * 2 * M_PI;	// [0, 2*PI] around the normal
		double theta = (static_cast<double>(rand()) / RAND_MAX)* M_PI_2;	// [0, PI/2] towards the normal

		// Cartesian coordinates
		float x = cos(phi) * sin(theta);
		float y = sin(phi) * sin(theta);
		float z = cos(theta);
		vec3 local_dir = vec3(x, y, z);

		// Transform direction to world coordinates
		vec3 world_dir = glm::inverse(glm::transpose(M)) * glm::vec4(local_dir, 1.0);
		//vec3 worldDir = glm::inverse(M) * glm::vec4(localDir, 1.0); 

		// Russian Roulette for ref
		double reflected_importance = 0.0;
		if (static_cast<double>(rand()) / RAND_MAX < this->absorption)
		{
			reflected_importance = incoming->importance * this->reflectance / (this->absorption * diff_bounces);
		}

		Ray reflected_ray{ incoming->end + incoming->target->getNormal(incoming->end) * ray_offset, world_dir, reflected_importance };

		reflected_ray.depth = incoming->depth + 1;

		reflected.push_back(reflected_ray);
	}

	return reflected;
}

// Constructor for Light material
Light::Light(dvec3 color, double emittance)
	: Material(color, emittance) {};

// BRDF function for Light material
std::vector<Ray> Light::brdf(const std::shared_ptr<Ray>& incoming) const
{
	double reflected_importance = 0.0;

	Ray stopped{ incoming->end, glm::normalize(glm::reflect(incoming->end, incoming->target->getNormal(incoming->end))), reflected_importance };

	std::vector<Ray> result;

	result.push_back(stopped);

	return result;
}


// transparent object with perfect reflection aswell. no loss in energy (importance)
// Constructor for Glass material
Glass::Glass(const dvec3& color, float index)
	:Material(color), reflected_index(index) {};

// BRDF function for Glass material
std::vector<Ray> Glass::brdf(const std::shared_ptr<Ray>& incoming) const
{
	double reflected_importance{ 0.0 };
	double refracted_importance{ 0.0 };

	vec3 N = incoming->inside_object ? -incoming->target->getNormal(incoming->end) : incoming->target->getNormal(incoming->end);
	vec3 I = incoming->direction;

	double cos_in_angle = glm::dot(-I, N);
	double in_angle = glm::acos(cos_in_angle);

	double max_angle = glm::asin(Air_ref_index / reflected_index);

	if (incoming->depth < maximum_ray_depth)
	{

		if (incoming->inside_object && in_angle > max_angle)
		{
			reflected_importance = incoming->importance;
		}
		else
		{
			// Schlick's approximation
			double R0 = incoming->inside_object ?
				glm::pow(((reflected_index - Air_ref_index) / (Air_ref_index + reflected_index)), 2.0)
				: glm::pow(((Air_ref_index - reflected_index) / (Air_ref_index + reflected_index)), 2.0);

			reflected_importance = (R0 + (1.0 - R0) * glm::pow((1.0 - cos_in_angle), 5.0)) * incoming->importance; //Too reflective with 5.0? (according to Schlick's)
			refracted_importance = (1.0 - reflected_importance) * incoming->importance;
		}
	}

	// Start with offset
	Ray reflected_ray{ incoming->end + N * ray_offset,	
				glm::normalize(glm::reflect(incoming->end, N)),
				reflected_importance };

	//Lecture 5
	float reflective_ratio = incoming->inside_object ? reflected_index / Air_ref_index : Air_ref_index / reflected_index;
	auto R_d0 = reflective_ratio * I;
	auto in_r = -reflective_ratio * glm::dot(N, I);
	auto in_sqrt = glm::sqrt(1.0f - glm::pow(reflective_ratio, 2.0f) * (1.0f - glm::pow(glm::dot(N, I), 2.0f)));
	vec3 refracted_dir = R_d0 + N * (in_r - in_sqrt);

	vec3 refracted_start = incoming->end - N * ray_offset;

	Ray refracted_ray{ refracted_start, refracted_dir, refracted_importance };

	reflected_ray.inside_object = incoming->inside_object;
	refracted_ray.inside_object = !incoming->inside_object;

	reflected_ray.depth = incoming->depth + 1;
	refracted_ray.depth = incoming->depth + 1;

	std::vector<Ray> result;
	result.push_back(reflected_ray);
	result.push_back(refracted_ray);

	return result;
}













