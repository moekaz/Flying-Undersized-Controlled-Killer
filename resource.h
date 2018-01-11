#ifndef RESOURCE_H_
#define RESOURCE_H_

#include <string>
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

namespace game 
{
    // Possible resource types
    typedef enum Type { Material, PointSet, Mesh, Texture, Data } ResourceType;

    // Class that holds one resource
    class Resource 
	{
        private:
            ResourceType type_;		// Type of resource
            std::string name_;		// Reference name
			GLsizei size_;			// Number of primitives in geometry
            union {
                struct {
                    GLuint resource_; // OpenGL handle for resource
                };
                struct {
                    GLuint array_buffer_; // Buffers for geometry
                    GLuint element_array_buffer_; // element buffer
                };
				struct {
					GLfloat *data_;		//data for particle systems
				};
            };

        public:
            Resource(ResourceType type, std::string name, GLuint resource, GLsizei size);
            Resource(ResourceType type, std::string name, GLuint array_buffer, GLuint element_array_buffer, GLsizei size);
			Resource(ResourceType type, std::string name, GLfloat *data, GLsizei size); 
			~Resource();

            ResourceType GetType(void) const;			//get resource type
            const std::string GetName(void) const;		//get name of resource
            GLuint GetResource(void) const;				//get resource 
            GLuint GetArrayBuffer(void) const;			//get array buffer
            GLuint GetElementArrayBuffer(void) const;	//get element array buffer
            GLsizei GetSize(void) const;				//get size 
			GLfloat *GetData(void) const;
    }; // class Resource
} // namespace game

#endif // RESOURCE_H_
