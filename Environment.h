#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include "scene_node.h"
#include "Collidable.h"
#include "room.h"

// SPIDERS ARE CHARACTERS COLLIDABLES 
namespace game
{
	class Environment : Collidable
	{
	public:
		Environment();
		Environment(SceneNode*);
		~Environment();
		
		bool collision(SceneNode* , float,float);					//dummy function for the collidables interface
		bool collision(SceneNode*, float, float, glm::vec3*);		//collision function with all rooms of environment
		void addRoom(Room*);										//add a room to the vector in the environment

	private:
		std::vector<Room*> rooms;									//store vector of rooms
		SceneNode* floor;											//sceneNode for the floor
		SceneNode *sky;												//sceneNode for a sky

	protected:
	};
}

#endif ENVIRONMENT_H// ENVIRONMENT_H