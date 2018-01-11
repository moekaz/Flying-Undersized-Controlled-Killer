#ifndef ENEMY_H
#define ENEMY_H

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include <iostream>
#include <ctime>
#include "Character.h"
#include "Projectile.h"

// CREATE A GENERAL ENEMY WHICH IS A CHARACTER AND IS INHERITED BY ALL ENEMY TYPES
namespace game
{
	class Enemy : public Character
	{
	public:
		glm::vec3 getDirection() { return direction; }									//get direction vector from enemy to player

		virtual void updateTarget(glm::vec3 targPos)									//update targetPosition in enemy to follow or use AI 
		{
			targetPos = targPos; 
		}

		virtual	void updateTargetOrientation(glm::quat orient)							//update target's orientation also for Ai purposes
		{
			targetOrientation = orient;
			glm::quat rotation = glm::angleAxis(glm::pi<float>(), glm::vec3(0, 1, 0));
			targetOrientation = rotation * targetOrientation;
		}

		virtual bool getFiring() { return firing; }										//check if the enemy is firing according to the AI

		virtual void fire(Projectile* p)												//add a projectile to the projectile vector of the enemy
		{
			if (firing) 
			{
				firing = false;
				projectiles.push_back(p);
			}
		}

		std::vector<Projectile*> projectiles;											//store the projectiles of the enemy


	private:
	protected:
		glm::vec3 direction;				//vector between enemy and player
		glm::vec3 prevDirection;			//previous direction between enemy and player
		glm::vec3 targetPos;				//position of target for AI
		glm::quat targetOrientation;		//orientation of target also for AI
		int state;							//state variable for state machine
		bool firing;						//check whether enemy is firing										
	};
}
#endif // ENEMY_H