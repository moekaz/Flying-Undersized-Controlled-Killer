#ifndef COLLIDABLE_H
#define COLLIDABLE_H

#include "scene_node.h"

// INTERFACE FOR ALL COLLIDABLES OBJECTS
namespace game
{
	class Collidable
	{
	public:
		float offset;													//offset value for the center of a collidable object for more accurate collision
		float boundingRadius;											//radius of the bounding sphere of the collidable object
		virtual bool collision(SceneNode*, float, float) = 0;			//enforcement of a collision function
	};
}
#endif // COLLIDABLE_H