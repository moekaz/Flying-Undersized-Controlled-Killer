#include "room.h"

namespace game
{
	/* Constructor */
	Room::Room() {}
	/* Destructor */
	Room::~Room() {}

	/* Modifiers */
	void Room::addWall(Wall* myWall) { walls.push_back(myWall); }

	/* Collision */
	bool Room::collision(SceneNode*, float,float) { return false; }

	//check for collision on all walls in a room
	bool Room::collision(SceneNode* obj, float boundRad, float off, glm::vec3* norm)
	{
		for (int i = 0; i < walls.size(); i++)
		{
			if (walls[i]->collision(obj, boundRad, off, norm)) return true;
		}
		return false;
	}

	/* Setter */
	void Room::SetFloor(SceneNode *myFloor){ floor = myFloor; }

	/* Getter */
	SceneNode *Room::getFloor() { return floor; }
}