#ifndef CAMERANODE_H
#define CAMERANODE_H

#include <string>
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#define GLM_FORCE_RADIANS
#include <glm/gtc/quaternion.hpp>
#include "camera.h"
#include "scene_node.h"

// Encapsulation of camera as a sceneNode
namespace game
{
	class CameraNode : public SceneNode
	{
	public:
		CameraNode(Camera*);
		~CameraNode();

		glm::vec3 GetPosition(void)const;		//An overload of the getPosition of sceneNode
		glm::quat GetOrientation(void)const;	//An overload of the getOrientation of sceneNode

	private:
		Camera* camera;
	};
}
#endif //CAMERANODE_H