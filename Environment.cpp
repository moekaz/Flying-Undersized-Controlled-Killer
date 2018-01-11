#include "Environment.h"
#include <iostream>

namespace game
{
	/* Constructors */
	Environment::Environment() {}

	Environment::Environment(SceneNode *floor) 
	{
		this->floor = floor;
	}

	/* Destructor */
	Environment::~Environment() {}

	/* Collision */
	bool Environment::collision(SceneNode*, float, float) { return false; }

	//need normals of each plane I create 
	bool Environment::collision(SceneNode* object , float boundRad , float off, glm::vec3* norm)
	{
		for (int w = 0; w < rooms.size(); w++)
		{
			if (rooms[w]->collision(object, boundRad, off, norm)) return true;
		}
		return false;
	}

	/* Modifiers */
	void Environment::addRoom(Room *myRoom)
	{
		rooms.push_back(myRoom);
	}

}