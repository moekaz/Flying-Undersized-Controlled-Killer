#ifndef DRAGGABLE_H
#define DRAGGABLE_H

#include "scene_node.h"
#include "Collidable.h"
#include <glm/glm.hpp>

// INTERFACE FOR ALL DRAGGABLE OBJECTS
namespace game
{
	class Draggable : public Collidable
	{
	public:
		bool beingDragged;			//Check for whether the object is being dragged
		bool onFloor;				//Check for whether the object hit the floor after falling from gravity
		float gravity;				//Gravity value for the draggable object
	};
}
#endif // DRAGGABLE_H