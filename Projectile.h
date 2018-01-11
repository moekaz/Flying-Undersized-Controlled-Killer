#pragma once

#include <string>
#include <vector>
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#define GLM_FORCE_RADIANS
#include <glm/gtc/quaternion.hpp>

#include "Collidable.h"
#include "scene_node.h"
#include "particleNode.h"

//PROJECTILES
namespace game
{
	class Projectile : public Collidable
	{
	public:
		glm::vec3 direction;			// direction of the rocket
		float speed;					// speed of the rocket
		int timer;						// time after which rocket gets deleted 
		SceneNode* node;				// scene node for projectiles
		ParticleNode* particle;			// particle system

		virtual void update() = 0;		// update projectiles
	private:
	protected:
	};
}