#include "Web.h"

#include <iostream>

// Web
namespace game
{
	/* Constructor */
	Web::Web(SceneNode* node, ParticleNode *particle, glm::vec3 direction)
	{
		offset = 1.0;									 // Center offset
		this->particle = particle;						 // Web particle
		this->node = node;		 						 // SceneNode for Web
		this->direction = glm::normalize(direction);	 // Direction of the Web
		this->speed = 0.5;								 // Speed of the Web
		this->timer = 300;			                     // Timer for lifespan of web
		boundingRadius = 0.5;
	}

	/* Destructor */
	Web::~Web() {}

	/* Update */
	void Web::update()
	{
		timer--;

		if (timer <= 0) 
		{ 
			node->del = true;		// delete sceneNode
			particle->getParticle()->del = true;   // delete the particle system
		}
		else 
		{ 
			//move the web
			node->Translate(glm::vec3(speed * direction));
		}
	}

	/* Collision */
	bool Web::collision(SceneNode* collidable, float off, float boundingRad)
	{
		//find real center of the object
		glm::vec3 objUpVec = glm::normalize(collidable->getAbsoluteOrientation() * glm::vec3(0, 1, 0));
		glm::vec3 objRealCenter = collidable->getAbsolutePosition() + objUpVec * off;

		//find my real center
		glm::vec3 myUpVec = glm::normalize(node->getAbsoluteOrientation() * glm::vec3(0, 1, 0));
		glm::vec3 myRealCenter = node->getAbsolutePosition() + myUpVec * offset;

		//find difference in positions
		glm::vec3 difference = myRealCenter - objRealCenter;

		//compare distances 
		return ((std::sqrt(std::pow(difference[0], 2) + std::pow(difference[1], 2) + std::pow(difference[2], 2))) <= boundingRad + boundingRadius);
	}
}