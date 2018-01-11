#ifndef PARTICLENODE_H
#define PARTICLENODE_H

#include "scene_node.h"

// PARTICLE SYSTEM 
namespace game
{
	class ParticleNode
	{
	public:
		ParticleNode(SceneNode *);
		~ParticleNode();

		bool shouldDisappear;																// get rid of the particle system
				
		void update(void);																	// update the system's timer 
		void startAnimate(glm::vec3 position, glm::quat orientation, double duration);		// start particle system animation
		void updatePosition(glm::vec3 position);											// update particle system position
		SceneNode *getParticle(void);														// getter for the particle
		void deleteNode(void);																// delete the particle system by trying to delete the sceneNode

	private:
		double timer;																		// timer for the animations
		double lasttime;																	// storing previous time to subtract from timer for animations
		SceneNode *particle;																// SceneNode to store particle system
	};
}
#endif PARTICLENODE_H
