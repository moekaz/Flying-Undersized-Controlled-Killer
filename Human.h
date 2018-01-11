#ifndef HUMAN_H
#define HUMAN_H

#include "scene_node.h"
#include "Character.h"
#include "Enemy.h"

// HUMANS ARE ENEMIES WHICH ARE CHARACTERS AND COLLIDABLES
namespace game
{
	class Human : public Enemy
	{
	public:
		Human(SceneNode*, SceneNode*, SceneNode*, SceneNode*, SceneNode*);
		~Human();

		SceneNode* body;							// human body
		SceneNode* leftHand;						// left hand
		SceneNode* rightHand;						// right hand
		SceneNode* leftLeg;							// left leg
		SceneNode* rightLeg;						// right leg
		float gravity;								// gravity value
		bool onFloor;								// check whether human is on the floor

		SceneNode* forwardBall;

		void update();								// update human position
		bool collision(SceneNode*, float, float);	// collision detection for human


	private:
	protected:
	};
}
#endif // !HUMAN_H