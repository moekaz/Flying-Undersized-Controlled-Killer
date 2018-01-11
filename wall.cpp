#include "wall.h"

namespace game
{
	Wall::Wall(SceneNode* wallNode, glm::vec3 myNormal , float length , float width)
	{
		boundingRadius = 10000000.f;			// useless 
		node = wallNode;						// sceneNode for the wall
		normal = glm::normalize(myNormal);		// normal vector of the wall
		this->length = length;					// length value of the rectangular wall
		this->width = width;					// width value of the rectangular wall
	}

	Wall::~Wall() {}

	// Not needed
	bool Wall::collision(SceneNode* obj, float boundRad, float) { return false; }

	//plane sphere collision with min max bounds checks
	bool Wall::collision(SceneNode* obj, float boundRad, float off, glm::vec3* norm)
	{
		*norm = normal;			//return the normal of the plane

		//obj up vector
		glm::vec3 objUpVec = glm::normalize (glm::vec3(0, 1, 0) * obj->getAbsoluteOrientation());
		glm::vec3 objRealCenter = obj->getAbsolutePosition() + objUpVec * off;

		glm::vec3 wallObjVec = node->getAbsolutePosition() - objRealCenter;
		float prod = abs(glm::dot(wallObjVec, normal));

		glm::vec3 myPos = node->getAbsolutePosition();
		
		// SIDE WALLS , FRONT WALLS , FLOOR
		if (normal == glm::vec3(1, 0, 0) || normal == glm::vec3(-1, 0, 0)) 
		{ return prod <= boundRad && objRealCenter.y >= myPos.y - length/2 && objRealCenter.y <= myPos.y + length/2 && objRealCenter.z >= myPos.z - width/2 && objRealCenter.z <= myPos.z + width/2; }

		else if (normal == glm::vec3(0, 0, 1) || normal == glm::vec3(0, 0, -1)) 
		{ return prod <= boundRad && objRealCenter.y >= myPos.y - length/2 && objRealCenter.y <= myPos.y + length/2 && objRealCenter.x >= myPos.x - width/2 && objRealCenter.x <= myPos.x + width/2; }

		else 
		{ return prod <= boundRad && objRealCenter.x >= myPos.x - width/2 && objRealCenter.x <= myPos.x + width/2 && objRealCenter.z >= myPos.z - length/2 && objRealCenter.z <= myPos.z + length/2; }
	}
}