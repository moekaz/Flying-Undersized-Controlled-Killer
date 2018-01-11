#ifndef ROOM_H
#define ROOM_H

#include "scene_node.h"
#include "Collidable.h"
#include "wall.h"

//ROOM IS A COLLIDABLE (VECTOR OF WALLS)
namespace game
{
	class Room : public Collidable
	{
	public:
		Room();
		~Room();

		void addWall(Wall*);										//add wall to the vector
		bool collision(SceneNode*, float, float);					//collision dummy function
		bool collision(SceneNode*, float, float, glm::vec3*);		//collision detection for room
		void SetFloor(SceneNode*);									//the floor is the parent of the hierarchy 
		SceneNode* getFloor();										//get the floor or parent of the hierarchy

	private:
		std::vector<Wall*> walls;									//vector of walls
		SceneNode* floor;											//store the floor
	};
}
#endif ROOM_H