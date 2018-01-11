#ifndef WEB_H
#define WEB_H

#include <string>
#include <vector>
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#define GLM_FORCE_RADIANS
#include <glm/gtc/quaternion.hpp>

#include "scene_node.h"
#include "Projectile.h"
#include "particleNode.h"

// Web 
namespace game
{
	class Web : public Projectile
	{
	public:
		Web(SceneNode*, ParticleNode*, glm::vec3);
		~Web();

		void update();									// update the position of the rocket
		bool collision(SceneNode*, float, float);		// collision Detection

	private:
	protected:
	};
}
#endif // WEB_H