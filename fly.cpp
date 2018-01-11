#include "fly.h"

namespace game 
{
	/* Constructor */
	Fly::Fly(SceneNode* flyBody, SceneNode* flyWings, SceneNode* flyLegs)
	{
		offset = 0.17;
		speed = 0.5;					// hardcode speed
		maxFireRate = 30;				// maximum fire rate
		fireRate = 0;					// fire rate handler
		timer = 10;						// Timer for wing movement
		upWingMovement = true;			// Checking for upwards or downwards fly rotation
		maxHealth = 200;				// Maximum health
		health = maxHealth;				// Health of fly
		myBlock = NULL;					// Block when fly is dragging
		dragging = false;				// check for whether the fly is dragging
		boundingRadius = 1;				// Radius of bounds
		body = flyBody;					// Body of fly
		wings = flyWings;				// Wings of the fly
		legs = flyLegs;					// Legs of fly
	}

	/* Destructor */
	Fly::~Fly(){}

	/* Update */
	void Fly::update()
	{
		fireRate--;		
		

		// wing animation
		if (upWingMovement)
		{
			timer--;
			glm::quat rotation(glm::angleAxis(glm::pi<float>() / 120, glm::vec3(1, 0, 0)));
			wings->Rotate(rotation);
			if (timer <= 0) { upWingMovement = false; }
		}
		else
		{
			timer++;
			glm::quat rotation(glm::angleAxis(glm::pi<float>() / 120, glm::vec3(-1, 0, 0)));
			wings->Rotate(rotation);
			if (timer >= 10) { upWingMovement = true; }
		}
		
		// Check rockets 
		for (int i = 0; i < rockets.size(); i++)
		{
			// when timer is 0 delete the rocket
			if (rockets[i]->timer <= 0)
			{
				rockets[i]->node->del = true;
			}
			else {
				rockets[i]->update();
			}

			if (rockets[i]->node->del)
			{
				rockets_particles[i]->deleteNode();
				rockets_particles.erase(rockets_particles.begin() + i);
				rockets.erase(rockets.begin() + i);
			}
			else
			{
				rockets_particles[i]->updatePosition(rockets[i]->node->getAbsolutePosition());
			}
		}
	}

	/* Collision */
	bool Fly::collision(SceneNode * object, float off, float boundRad)
	{
		//find real center of the object
		glm::vec3 objUpVec = glm::normalize(object->getAbsoluteOrientation() * glm::vec3(0, 1, 0));
		glm::vec3 objRealCenter = object->getAbsolutePosition() + objUpVec * off;

		//find my real center
		glm::vec3 myUpVec = glm::normalize(body->getAbsoluteOrientation() * glm::vec3(0, 1, 0));
		glm::vec3 myRealCenter = body->getAbsolutePosition() + myUpVec * offset;

		//find difference in positions
		glm::vec3 difference = myRealCenter - objRealCenter;

		//compare distances 
		return ((std::sqrt(std::pow(difference[0], 2) + std::pow(difference[1], 2) + std::pow(difference[2], 2))) <= boundRad + boundingRadius);
	}
} // namespace game
