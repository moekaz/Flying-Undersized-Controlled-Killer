#ifndef BLOCK_H
#define BLOCK_H

#include <string>
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <stdexcept>
#define GLM_FORCE_RADIANS
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <time.h>

#include "resource.h"
#include "camera.h"
#include "scene_node.h"
#include "Draggable.h"

// Drag and Drop blocks
namespace game
{
	class Block : public Draggable
	{
	public:
		Block(SceneNode*);
		~Block();

		SceneNode* object;									//SceneNode for drawing 

		void update();										//Update block's position
		bool collision(SceneNode*, float , float);			//Check for collision with it

		bool hitFloor;										//bool to check for hitting the floor for particle animation
		bool dropped;										//check for dropping block
	private:
	protected:
	};
} // namespace game
#endif //BLOCK_H
