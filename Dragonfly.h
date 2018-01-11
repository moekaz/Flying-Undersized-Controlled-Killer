#ifndef DRAGONFLY_H
#define DRAGONFLY_H

#include "Enemy.h"
#include "scene_node.h"

// DRAGONFLIES ARE COLLIDABLES AND CHARACTERS
namespace game
{
	class DragonFly : public Enemy
	{
	public:
		DragonFly(SceneNode*, SceneNode*, SceneNode*, SceneNode*);
		~DragonFly();

		SceneNode* body;								//body node for dragonfly
		SceneNode* leftWing;							//leftwing node
		SceneNode* rightWing;							//rightwing node
		SceneNode* legs;								//leg node
		
		int timer;										//timer for the animation of the wings of the dragonfly
		bool upWingMovement;							//check for whether we want up or down wing movement(depends on the timer value)
	
		void update();									//update function to update position or AI of the dragonfly
		bool collision(SceneNode*, float, float);		//collision function of the dragonfly with other objects
		void deleteProjectiles();						//deletes all projectiles after killing the dragonfly

	private:
	protected:
	};
}
#endif //DRAGONFLY_H