#include "Human.h"

namespace game
{
	/* Constructor */
	Human::Human(SceneNode* humanBody, SceneNode* humanLeftHand, SceneNode* humanRightHand, SceneNode* humanLeftLeg, SceneNode* humanRightLeg)
	{
		
		targetPos = glm::vec3(0, 0, 0);					// Store target position
		targetOrientation = glm::quat(0, 0, 0, 0);		// Store target orientation
		state = 0;										// State in state machine 
		speed = 0.6;									// Human speed of movement
		fireRate = 0;									// Human fireRate 
		maxFireRate = 60;								// Human maxFireRate
		maxHealth = 1000;								// Max health
		health = maxHealth;								// Health
		firing = false;									// Controls if the enemy is shooting
		boundingRadius = 7.0;							// Bounding Circle
		offset = 7;										// Offset from center 
		onFloor = false;								// Check whether enemy is on the floor
		gravity = -0.2f;								// Gravity
		body = humanBody;								// Body of the human node
		leftHand = humanLeftHand;						// Left hand of the human node
		rightHand = humanRightHand;						// Right hand of the human node
		leftLeg = humanLeftLeg;							// Left leg of the human node
		rightLeg = humanRightLeg;						// Right leg of the human node
	}

	/* Destructor */
	Human::~Human() {}

	/* Update */
	void Human::update() 
	{
		// find forward direction of enemy
		if (!onFloor) { body->Translate(glm::vec3(0, gravity, 0)); }

		direction = glm::normalize(targetPos - body->getAbsolutePosition());
		fireRate--;
		

		glm::vec3 forward = glm::normalize(glm::vec3(0 , 0 , 1.0) * body->GetOrientation());
		glm::vec3 side = glm::normalize(glm::vec3(1.0 , 0 , 0.0) * body->GetOrientation());
		glm::vec3 up = glm::normalize(glm::cross(forward , side));

		forwardBall->SetPosition(body->getAbsolutePosition() + forward);
		
		glm::vec3 zaxis = direction;
		zaxis.y = 0.0f;
		glm::vec3 xaxis = glm::normalize(glm::cross(up , zaxis));
		glm::vec3 yaxis = glm::vec3(0 , 1 , 0);
		
		body->SetOrientation(glm::quat(glm::mat3(xaxis , yaxis , zaxis)));

		state = rand() % 3;
	
		if (state == 0) {} //Idle
		else if (state == 1 || state == 3) 
		{
			//Move to player
			body->Translate(glm::vec3(direction.x, 0, direction.z) * speed);
		}
		else if (state == 2) { }
		else { std::cout << "Invalid state in Human" << std::endl; }

		// Check rockets when timer is 0 delete the rocket
		for (int i = 0; i < projectiles.size(); i++)
		{
			if (projectiles[i]->timer <= 0) { projectiles.erase(projectiles.begin() + i); }
			else { projectiles[i]->update(); }
		}
	}

	/* Collision */
	bool Human::collision(SceneNode * object, float off, float boundRad)
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
}
