#ifndef FLY_H
#define FLY_H

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
#include "Rocket.h"
#include "Character.h"
#include "Block.h"
#include "particleNode.h"

// FLIES ARE CHARACTERS
namespace game 
{
    class Fly : public Character
	{
	public:
		Fly(SceneNode*,SceneNode*,SceneNode*);
		~Fly();

		std::vector<ParticleNode *> rockets_particles;					//Particles of the rockets 
		std::vector<Rocket*> rockets;									//store the rockets
		SceneNode* healthBar;											//GUI element for displaying the player's health
		SceneNode* body;												//body sceneNode
		SceneNode* wings;												//wings sceneNode
		SceneNode* legs;												//legs sceneNode
		Block* myBlock;													//block that player is dragging
		int timer;														//timer for animation of fly's wings
		bool upWingMovement;											//check for up or down wing movement
		bool dragging;													//if we want to disallow shooting while dragging

		void update();													//update fly and its bullets
		bool collision(SceneNode*, float, float);						//collision detection

	private:
	protected:
    }; 
} // namespace game
#endif //FLY_H
