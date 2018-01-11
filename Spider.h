#ifndef SPIDER_H
#define SPIDER_H

#include "Enemy.h"
#include "scene_node.h"

// SPIDERS ARE CHARACTERS COLLIDABLES 
namespace game
{
	class Spider : public Enemy
	{
	public:
		Spider(SceneNode*, SceneNode*, SceneNode*);
		~Spider();

		SceneNode* body;		// Spider Body
		SceneNode* leftLeg;		// Spider left leg
		SceneNode* rightLeg;	// Spider right leg
		bool isMoving;			// Check whether the spider is moving
		bool legMovement;		// Check for leg movement
		int timer;				// Timer for animations
		float gravity;			// Gravity value
		bool onFloor;			// Check whether spider is on the floor
			
		void update();												// Update spider position
		virtual	void updateTargetOrientation(glm::quat orient);		// Update targets Orientation overload
		bool collision(SceneNode*, float, float);					// Collision detection
		void deleteProjectiles();									// Deletes all projectiles after killing the spider

	private:
	protected:
	};
}

#endif // SPIDER_H