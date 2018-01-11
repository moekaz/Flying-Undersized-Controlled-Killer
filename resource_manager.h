#ifndef RESOURCE_MANAGER_H_
#define RESOURCE_MANAGER_H_

#include <string>
#include <vector>
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "resource.h"

// Default extensions for different shader source files
#define VERTEX_PROGRAM_EXTENSION "_vp.glsl"
#define FRAGMENT_PROGRAM_EXTENSION "_fp.glsl"
#define GEOMETRY_PROGRAM_EXTENSION "_gp.glsl"

namespace game 
{
    // Class that manages all resources
    class ResourceManager 
	{
	public:
		ResourceManager(void);
		~ResourceManager(); 

		void AddResource(ResourceType type, const std::string name, GLuint resource, GLsizei size);	// Add a resource that was already loaded and allocated to memory
		void AddResource(ResourceType type, const std::string name, GLuint array_buffer, GLuint element_array_buffer, GLsizei size);
		void AddResource(ResourceType type, const std::string name, GLfloat *data, GLsizei size);// Load a resource from a file, according to the specified type
		void LoadResource(ResourceType type, const std::string name, const char *filename, int num_particles = 20000);	// Load a resource from a file, according to the specified type
		Resource *GetResource(const std::string name) const;	// Get the resource with the specified name

        // Methods to create Geometry
		void CreateTorus(std::string object_name, float loop_radius = 0.6, float circle_radius = 0.2, int num_loop_samples = 90, int num_circle_samples = 30);
		void CreateSphere(std::string object_name, float radius = 0.6, int num_samples_theta = 90, int num_samples_phi = 45);
		void CreateWall(std::string object_name);
		void CreateCylinder(std::string object_name, float height = 0.1, float loop_radius = 0.6, float circle_radius = 0.35, int num_loop_samples = 90, int num_circle_samples = 30, glm::vec3 color = glm::vec3(0, 1, 0));
		void CreateCube(std::string object_name);
		void CreateSphereParticles(std::string object_name, int num_particles = 20000);
		void CreateTorusParticles(std::string object_name, int num_particles = 20000, float loop_radius = 0.6, float circle_radius = 0.2);
		void CreateConeParticles(std::string object_name, int num_particles = 20000, glm::vec3 color = glm::vec3(-1, -1, -1));
	
		void CreateControlPoints(std::string object_name, int num_control_points);
	private:
		std::vector<Resource*> resource_;	// List storing all resources
 
        // Methods to load specific types of resources
		void LoadMaterial(const std::string name, const char *prefix);	// Load shaders programs
		std::string LoadTextFile(const char *filename);	 // Load a text file into memory (could be source code)
		void LoadTexture(const std::string name, const char *filename);	// Load a texture
		void LoadMesh(const std::string name, const char *filename);	// Loads a mesh in obj format
		void LoadMeshParticles(std::string name, const char *filename, int num_particles = 20000); //Load a mesh with particles only

    };// class ResourceManager
}// namespace game
#endif // RESOURCE_MANAGER_H_
