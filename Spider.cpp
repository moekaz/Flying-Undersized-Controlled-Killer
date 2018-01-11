#include "Spider.h"

// Spider is fixed
namespace game
{
	/* Constructor */
	Spider::Spider(SceneNode* spiderBody, SceneNode* spiderLeftLeg, SceneNode* spiderRightLeg)
	{
		targetPos = glm::vec3(0, 0, 0);				// Store target position
		targetOrientation = glm::quat(0, 0, 0, 0);	// Store target orientation
		state = 0;									// Machine state
		speed = 0.3;								// Speed of movement the spider
		fireRate = 0;								// FireRate of the shooting webs
		maxFireRate = 120;							// Maximum fire rate of shooting
		maxHealth = 30;								// Maximum health
		health = maxHealth;							// Health
		firing = false;								// Controls if the enemy is shooting
		timer = 5;									// Timer for leg movement
		legMovement = true;							// Check for leg movement
		isMoving = false;							// Check for movement
		boundingRadius = 0.8;						// Radius bounding
		offset = 0.7;								// Center offset
		onFloor = false;							// Check whether on floor or not
		gravity = -0.2f;							// Gravity
		body = spiderBody;							// Body of the spider node
		leftLeg = spiderLeftLeg;					// Left leg of the spider node
		rightLeg = spiderRightLeg;					// Right leg of the spider node
	}

	/* Destructor */
	Spider::~Spider() {}

	/* Update */
	void Spider::update()
	{
		// check if it is on the floor 
		if (!onFloor) { body->Translate(glm::vec3(0, gravity, 0)); }
		//decrement firerate
		fireRate--;

		glm::vec3 forward = glm::normalize(glm::vec3(0, 0, 1.0) * body->GetOrientation());
		glm::vec3 side = glm::normalize(glm::vec3(1.0, 0, 0.0) * body->GetOrientation());
		glm::vec3 up = glm::normalize(glm::cross(forward, side));

		//setup orientation
		direction = glm::normalize(targetPos - body->getAbsolutePosition());

		glm::vec3 zaxis = direction;
		zaxis.y = 0.0f;
		glm::vec3 xaxis = glm::normalize(glm::cross(up, zaxis));
		glm::vec3 yaxis = glm::vec3(0, 1, 0);

		body->SetOrientation(glm::quat(glm::mat3(xaxis, yaxis, zaxis)));
		body->Rotate(glm::angleAxis(glm::pi<float>() , up));

		//leg movement of spider
		if (isMoving)
		{
			if (legMovement)
			{
				timer--;
				leftLeg->Translate(glm::vec3(0, 0, 0.01));
				rightLeg->Translate(glm::vec3(0, 0, -0.01));
				if (timer <= 0) { legMovement = false; }
			}
			else
			{
				timer++;
				leftLeg->Translate(glm::vec3(0, 0, -0.01));
				rightLeg->Translate(glm::vec3(0, 0, 0.01));
				if (timer >= 10) { legMovement = true; }
			}
		}
		
		// State machine
		state = rand() % 3;

		if (state == 0) {}
	    else if (state == 1) 
		{ 
			//Move to player
			isMoving = true;
		}
		else if (state == 2) 
		{ 
			//Attack
			if (fireRate <= 0)
			{
				this->firing = true;
				fireRate = maxFireRate;
			}
			else { state = 0; }
			isMoving = true;
		}
		else if (state == 3) {} //Patrol
		else { std::cout << "Invalid state in Spider" << std::endl; }

		// Check Webs 
		for (int i = 0; i < projectiles.size(); i++)
		{
			// when timer is 0 delete the rocket
			if (projectiles[i]->timer <= 0) { projectiles.erase(projectiles.begin() + i); }
			else { projectiles[i]->update(); }
		}
	}

	/* Update target orientation */
	void Spider::updateTargetOrientation(glm::quat orient)
	{
		targetOrientation = orient;
		//glm::quat rotation = glm::angleAxis(glm::pi<float>(), glm::vec3(0, 1, 0));
		//targetOrientation = rotation * targetOrientation;
	}

	/* Collision */
	bool Spider::collision(SceneNode* object, float off, float boundRad)
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

	/* Deleting projectiles */
	void Spider::deleteProjectiles()
	{
		for (int i = 0; i < projectiles.size(); i++)
		{
			projectiles[i]->node->del = true;
			projectiles[i]->particle->getParticle()->del = true;
		}
	}
}