#include "asteroid.h"

namespace game {
	//texture is empty???????????!!!!!!!!!!!!!
Asteroid::Asteroid(const std::string name, const Resource *geometry, const Resource *material) : SceneNode(name, geometry, material,0) {
}


Asteroid::~Asteroid(){
}


glm::quat Asteroid::GetAngM(void) const {

    return angm_;
}


void Asteroid::SetAngM(glm::quat angm){

    angm_ = angm;
}


void Asteroid::Update(void){

    Rotate(angm_);
}
            
} // namespace game