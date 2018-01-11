#ifndef WALL_H
#define WALL_H

#include "scene_node.h"
#include "Collidable.h"

// WALL IS A COLLIDABLE
namespace game
{
	class Wall : public Collidable
	{
	public:
		Wall(SceneNode*, glm::vec3, float, float);
		~Wall();
				
		SceneNode* node;		//node for the wall
		glm::vec3 normal;		//normal of the wall
		float length;			//length of rectangular wall
		float width;			//width of rectangular wall

		bool collision(SceneNode*, float, float);					//dummy function for collision
		bool collision(SceneNode*, float, float, glm::vec3*);		//collision detection check
	};
}
#endif // WALL_H