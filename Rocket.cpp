#include "Rocket.h"
#include <iostream>

namespace game
{
	/* Constructor */
	Rocket::Rocket(SceneNode* node , glm::vec3 dir)
	{
		offset = 0.0;							// offset off center
		this->direction = glm::normalize(dir);	// direction of the rocket
		this->speed = 0.6;						// speed 
		this->timer = 200;						// timer to get rid of rocket
		this->node = node;						// SceneNode for drawing rocket
		boundingRadius = 0.1;					// radius of bounding sphere
	}

	/* Destructor */
	Rocket::~Rocket(){}

	/* Update */
	void Rocket::update() 
	{ 
		timer--;	// Decrement timer 
		// If timer is zero then we should delete the bullet else update the position of the bullet 
		if (timer <= 0) { node->del = true; }	
		else { node->Translate(glm::vec3(speed * direction)); }
	}

	/* Collision */
	bool Rocket::collision(SceneNode* object , float off, float boundRad)
	{
		//find real center of the object
		glm::vec3 objUpVec = glm::normalize(object->getAbsoluteOrientation() * glm::vec3(0, 1, 0));
		glm::vec3 objRealCenter = object->getAbsolutePosition() + objUpVec * off;

		//find my real center
		glm::vec3 myUpVec = glm::normalize(node->getAbsoluteOrientation() * glm::vec3(0, 1, 0));
		glm::vec3 myRealCenter = node->getAbsolutePosition() + myUpVec * offset;

		//find difference in positions
		glm::vec3 difference = myRealCenter - objRealCenter;

		//compare distances 
		return ((std::sqrt(std::pow(difference[0], 2) + std::pow(difference[1], 2) + std::pow(difference[2], 2))) <= boundRad + boundingRadius);
	}
}