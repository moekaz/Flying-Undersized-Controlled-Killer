#include "CameraNode.h"

namespace game
{
	/* Constrcutor */
	CameraNode::CameraNode(Camera* cam) : SceneNode("Camera" , 0 , 0, 0)
	{
		this->camera = cam;
		visible = false;
	}

	/* Destructor */
	CameraNode::~CameraNode() {}

	/* Getters */
	glm::vec3 CameraNode::GetPosition(void) const
	{
		if (camera->firstPerson) { return camera->GetPosition() - (camera->GetForward() * camera->distance); }
		else { return camera->GetPosition(); }
	}

	glm::quat CameraNode::GetOrientation(void) const { return camera->GetOrientation(); }
}// Game namespace 