#ifndef SCENE_NODE_H_
#define SCENE_NODE_H_

#include <string>
#include <vector>
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#define GLM_FORCE_RADIANS
#include <glm/gtc/quaternion.hpp>

#include "resource.h"
#include "camera.h"
#include "shader_attribute.h"

namespace game {

    // Class that manages one object in a scene 
    class SceneNode {

        public:
			SceneNode(const std::string name, const Resource *geometry, const Resource* material, const Resource *texture = 0);		// Create scene node from given resources
            ~SceneNode();	// Destructor
            
			bool visible;	// Check visibility
			bool del;		// Check whether to delete or not

			// Get node attributes
            const std::string GetName(void) const;		// Get name of node
            virtual glm::vec3 GetPosition(void) const;
			virtual glm::vec3 getAbsolutePosition(void) const;
			virtual glm::vec3 getPrevAbsolutePosition(void) const;
            virtual glm::quat GetOrientation(void) const;
			virtual glm::quat getAbsoluteOrientation(void) const;
            glm::vec3 GetScale(void) const; 

			// OpenGL variables
			GLenum GetMode(void) const;
			GLuint GetArrayBuffer(void) const;
			GLuint GetElementArrayBuffer(void) const;
			GLsizei GetSize(void) const;
			GLuint GetMaterial(void) const;
			bool GetBlending(void) const;

            // Set node attributes
            void SetPosition(glm::vec3 position);
            void SetOrientation(glm::quat orientation);
            void SetScale(glm::vec3 scale);
			void SetBlending(bool blending);
			void SetVisible(bool visible);
            
            // Perform transformations on node
            void Translate(glm::vec3 trans);
            void Rotate(glm::quat rot);
            void Scale(glm::vec3 scale);

            virtual glm::mat4 Draw(Camera *camera, glm::mat4 parent_transf);	 // Draw the node according to scene parameters in 'camera'
            virtual void update(void);		// Update the node

			//for starting the animation
			void updateTime(void);

            // Hierarchy-related methods
            void AddChild(SceneNode *node);
			void AppendChild(SceneNode *node);
			void RemoveChild(SceneNode *node);
            std::vector<SceneNode *>::const_iterator children_begin() const;
            std::vector<SceneNode *>::const_iterator children_end() const;

			void AddShaderAttribute(std::string name, DataType type, int size, GLfloat *data);
			void RemoveShaderAttribute(std::string name);
			void ClearShaderAttributes(void);
        private:
            std::string name_; // Name of the scene node
            GLuint array_buffer_; // References to geometry: vertex and array buffers
            GLuint element_array_buffer_;
            GLenum mode_; // Type of geometry
            GLsizei size_; // Number of primitives in geometry
            GLuint material_; // Reference to shader program
			GLuint texture_; // Reference to texture
            glm::vec3 position_; // Relative Position of node
			glm::vec3 absolutePosition; // Absolute position of node 
			glm::vec3 prevAbsolutePosition; // Absolute position of node 
            glm::quat orientation_; // Orientation of node
			glm::quat absoluteOrientation; // Absolute orientation
            glm::vec3 scale_; // Scale of node
			bool blending_; //blending
			bool visible_; //draw or not
			double start_time_; //start time for effects
			
            // Hierarchy
            SceneNode *parent_;						//parent of a sceneNode
            std::vector<SceneNode *> children_;		//children of the sceneNode

			std::vector<ShaderAttribute> shader_att_; // Shader attributes
            // Set matrices that transform the node in a shader program
            // Return transformation of current node combined with
            // parent transformation, without including scaling
            glm::mat4 SetupShader(GLuint program, glm::mat4 parent_transf);
			void maintainChildren();				//deletes nodes that need to be deleted from the graph before drawing them
    }; // class SceneNode
} // namespace game
#endif // SCENE_NODE_H_
