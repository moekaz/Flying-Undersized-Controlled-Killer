#include <stdexcept>
#define GLM_FORCE_RADIANS
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <time.h>

#include "scene_node.h"

namespace game
{
	/* Constructor */
	SceneNode::SceneNode(const std::string name, const Resource *geometry, const Resource *material, const Resource* texture)
	{
		// Set name of scene node
		name_ = name;
		position_ = glm::vec3(0, 0, 0);
		prevAbsolutePosition = glm::vec3(0, 0, 0);
		absolutePosition = glm::vec3(0, 0, 0);
		del = false;

		// Check for geometry
		if (geometry)
		{
			// Set geometry
			if (geometry->GetType() == PointSet)  { mode_ = GL_POINTS; }
			else if (geometry->GetType() == Mesh) { mode_ = GL_TRIANGLES; }
			else { throw(std::invalid_argument(std::string("Invalid type of geometry"))); }

			array_buffer_ = geometry->GetArrayBuffer();
			element_array_buffer_ = geometry->GetElementArrayBuffer();
			size_ = geometry->GetSize();
		}
		else { array_buffer_ = 0; }

		// Set material (shader program)
		if (material)
		{
			if (material->GetType() != Material) { throw(std::invalid_argument(std::string("Invalid type of material"))); }

			material_ = material->GetResource();
		}
		else { material_ = 0; }

		// Set texture
		if (texture) { texture_ = texture->GetResource(); }
		else { texture_ = 0;}

		// Other attributes
		scale_ = glm::vec3(1.0, 1.0, 1.0);
		blending_ = false;
		visible_ = true;

		// Hierarchy
		parent_ = NULL;

		start_time_ = glfwGetTime();
	}

	/* Destructor */
	SceneNode::~SceneNode() {}

	/* Getters */
	const std::string SceneNode::GetName(void) const	    { return name_; }
	glm::vec3 SceneNode::GetPosition(void) const		    { return position_; }
	glm::quat SceneNode::GetOrientation(void) const		    { return orientation_; }
	glm::vec3 SceneNode::GetScale(void) const			    { return scale_; }
	GLenum SceneNode::GetMode(void) const				    { return mode_; }
	GLuint SceneNode::GetArrayBuffer(void) const		    { return array_buffer_; }
	GLuint SceneNode::GetElementArrayBuffer(void) const     { return element_array_buffer_; }
	GLsizei SceneNode::GetSize(void) const				    { return size_; }
	GLuint SceneNode::GetMaterial(void) const			    { return material_; }
	bool SceneNode::GetBlending(void) const					{ return blending_;  }
	glm::vec3 SceneNode::getAbsolutePosition(void) const    { return absolutePosition; }
	glm::vec3 SceneNode::getPrevAbsolutePosition(void) const { return prevAbsolutePosition; }
	glm::quat SceneNode::getAbsoluteOrientation(void) const { return absoluteOrientation; }

	/* Setters */
	void SceneNode::SetPosition(glm::vec3 position) { position_ = position; }
	void SceneNode::SetOrientation(glm::quat orientation) { orientation_ = orientation; }
	void SceneNode::SetScale(glm::vec3 scale) { scale_ = scale; }
	void SceneNode::SetBlending(bool blending) { blending_ = blending; }
	void SceneNode::SetVisible(bool visible) { visible_ = visible; }

	/* Updaters */
	void SceneNode::Translate(glm::vec3 trans) { position_ += trans; }
	void SceneNode::Rotate(glm::quat rot) { orientation_ *= rot; }
	void SceneNode::Scale(glm::vec3 scale) { scale_ *= scale; }

	/* Update a SceneNode */
	void SceneNode::update(void) {}
	void SceneNode::updateTime(void) { start_time_ = glfwGetTime(); }

	/* Iterators */
	std::vector<SceneNode *>::const_iterator SceneNode::children_begin() const { return children_.begin(); }
	std::vector<SceneNode *>::const_iterator SceneNode::children_end() const { return children_.end(); }

	/* Adding a child */
	void SceneNode::AddChild(SceneNode *node)
	{
		children_.push_back(node);
		node->parent_ = this;
	}

	void SceneNode::AppendChild(SceneNode *node)
	{
		children_.insert(children_.begin(), node);
		//children_.push_back(node);
		node->parent_ = this;
	}

	/* Removing a child */
	void SceneNode::RemoveChild(SceneNode *node)
	{
		for (int i = 0; i < children_.size(); i++)
		{
			if (children_[i]->GetName() == node->GetName())
			{
				children_.erase(children_.begin() + i);
				node->parent_ = NULL;
				return;
			}
		}
		std::cout << "didn't find node" << std::endl;
	}

	/* Maintain children if a child needs to be deleted, delete it */
	void SceneNode::maintainChildren()
	{
		for (int i = 0; i < children_.size(); i++)
		{
			if (children_[i]->del) { children_.erase(children_.begin() + i); }
		}
	}

	/* Draw */
	glm::mat4 SceneNode::Draw(Camera *camera, glm::mat4 parent_transf)
	{
		if (!visible_) return parent_transf;
		maintainChildren(); // Check children for deletion

		// Set absolute position and orientation
		if (parent_ != NULL) 
		{
			glm::vec4 temp = parent_transf * glm::vec4(GetPosition().x , GetPosition().y , GetPosition().z , 1.0);
			absolutePosition = glm::vec3(temp.x, temp.y, temp.z);		
			absoluteOrientation = parent_->getAbsoluteOrientation() * GetOrientation();
		}

		// Select blending or not
		if (blending_) {
			// Disable z-buffer
			glDisable(GL_DEPTH_TEST);
			
			// Enable blending
			glEnable(GL_BLEND);
			//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // Simpler form
			glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glBlendEquationSeparate(GL_FUNC_ADD, GL_MAX);
		}
		else {
			// Enable z-buffer
			glEnable(GL_DEPTH_TEST);
			glDisable(GL_BLEND);
			glDepthFunc(GL_LESS);
		}

		if ((array_buffer_ > 0) && (material_ > 0)) 
		{
			// Select proper material (shader program)
			glUseProgram(material_);

			// Set geometry to draw
			glBindBuffer(GL_ARRAY_BUFFER, array_buffer_);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_array_buffer_);

			// Set globals for camera
			camera->SetupShader(material_);

			// Set world matrix and other shader input variables
			glm::mat4 transf = SetupShader(material_, parent_transf);

			for (int i = 0; i < shader_att_.size(); i++){ shader_att_[i].SetupShader(material_); }

			// Draw geometry
			if (mode_ == GL_POINTS) { glDrawArrays(mode_, 0, size_); }
			else { glDrawElements(mode_, size_, GL_UNSIGNED_INT, 0); }

			return transf;
		}
		else
		{
			//glm::mat4 rotation = glm::mat4_cast(orientation_);
			//glm::mat4 translation = glm::translate(glm::mat4(1.0), position_);
			glm::mat4 rotation = glm::mat4_cast(GetOrientation());
			glm::mat4 translation = glm::translate(glm::mat4(1.0), GetPosition());
			glm::mat4 transf = parent_transf * translation * rotation;
			return transf;
		}
	}

	/* Setup for the shader */
	glm::mat4 SceneNode::SetupShader(GLuint program, glm::mat4 parent_transf)
	{
		// Set attributes for shaders
		GLint vertex_att = glGetAttribLocation(program, "vertex");
		glVertexAttribPointer(vertex_att, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), 0);
		glEnableVertexAttribArray(vertex_att);

		GLint normal_att = glGetAttribLocation(program, "normal");
		glVertexAttribPointer(normal_att, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (void *)(3 * sizeof(GLfloat)));
		glEnableVertexAttribArray(normal_att);

		GLint color_att = glGetAttribLocation(program, "color");
		glVertexAttribPointer(color_att, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (void *)(6 * sizeof(GLfloat)));
		glEnableVertexAttribArray(color_att);

		GLint tex_att = glGetAttribLocation(program, "uv");
		glVertexAttribPointer(tex_att, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (void *)(9 * sizeof(GLfloat)));
		glEnableVertexAttribArray(tex_att);

		// World transformation
		glm::mat4 scaling = glm::scale(glm::mat4(1.0), scale_);
		glm::mat4 rotation = glm::mat4_cast(orientation_);
		glm::mat4 translation = glm::translate(glm::mat4(1.0), position_);
		glm::mat4 transf = parent_transf * translation * rotation;
		glm::mat4 local_transf = transf * scaling;

		GLint world_mat = glGetUniformLocation(program, "world_mat");
		glUniformMatrix4fv(world_mat, 1, GL_FALSE, glm::value_ptr(local_transf));

		// Normal matrix
		glm::mat4 normal_matrix = glm::transpose(glm::inverse(transf));
		GLint normal_mat = glGetUniformLocation(program, "normal_mat");
		glUniformMatrix4fv(normal_mat, 1, GL_FALSE, glm::value_ptr(normal_matrix));

		// Texture
		if (texture_) 
		{
			GLint tex = glGetUniformLocation(program, "texture_map");
			glUniform1i(tex, 0);							// Assign the first texture to the map
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, texture_);			// First texture we bind
															// Define texture interpolation
			glGenerateMipmap(GL_TEXTURE_2D);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		}

		// Timer
		GLint timer_var = glGetUniformLocation(program, "timer");
		double current_time = glfwGetTime() - start_time_;
		glUniform1f(timer_var, (float)current_time);

		// Return transformation of node combined with parent, without scaling
		return transf;
	}

	void SceneNode::AddShaderAttribute(std::string name, DataType type, int size, GLfloat *data) {

		ShaderAttribute att(name, type, size, data);
		shader_att_.push_back(att);
	}


	void SceneNode::RemoveShaderAttribute(std::string name) {

		for (std::vector<ShaderAttribute>::iterator it = shader_att_.begin();
		it != shader_att_.end();
			it++) {
			if (it->GetName() == name) {
				shader_att_.erase(it);
				break;
			}
		}
	}


	void SceneNode::ClearShaderAttributes(void) {

		shader_att_.clear();
	}
}// namespace game;
