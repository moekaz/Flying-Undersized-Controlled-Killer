#ifndef ROCKET_H
#define ROCKET_H

#include <string>
#include <vector>
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#define GLM_FORCE_RADIANS
#include <glm/gtc/quaternion.hpp>

#include "scene_node.h"
#include "camera.h"
#include "Collidable.h"
#include "Projectile.h"


// Rockets 
namespace game
{
	class Rocket : public Projectile 
	{
	public:
		Rocket(SceneNode*, glm::vec3);
		~Rocket();

		virtual void update();							// update the position of the rocket
		bool collision(SceneNode*, float, float);		// collision detection between rocket and other collidables

	private:
	protected:
	};
}
#endif // !ROCKET_H