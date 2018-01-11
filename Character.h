#ifndef CHARACTER_H
#define CHARACTER_H

#include "Collidable.h"

// CREATE A GENERAL CHARACTER WHICH WILL BE INHERITED BY ALL CHARACTER TYPES 
namespace game
{
	class Character : public Collidable
	{
	public:
		float speed;					//Speed of the character entity
		int fireRate;					//FireRate timer for bullets of entity if it has any
		int maxFireRate;				//Value to reset fireRate to
		float health;						//Health of the character entity
		float maxHealth;					//Value to set health to

		virtual void update() = 0;		//Overload and enforcement of update function

	private:
	protected:
	};
}
#endif // CHARACTER_H