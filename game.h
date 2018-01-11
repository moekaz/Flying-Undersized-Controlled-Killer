#ifndef GAME_H_
#define GAME_H_

#include <exception>
#include <string>
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "scene_graph.h"
#include "resource_manager.h"
#include "camera.h"
#include "rocket.h"
#include "Web.h"
#include "CameraNode.h"
#include "fly.h"
#include "Human.h"
#include "Spider.h"
#include "Dragonfly.h"
#include "Environment.h"
#include "Block.h"
#include "Room.h"
#include "wall.h"
#include "particleNode.h"

// GAME
namespace game 
{
    // Exception type for the game
    class GameException: public std::exception
    {
        private:
            std::string message_;
        public:
            GameException(std::string message) : message_(message) {};
            virtual const char* what() const throw() { return message_.c_str(); };
            virtual ~GameException() throw() {};
    };

    // Game application
    class Game 
	{
        public:
            // Constructor and destructor
            Game(void);
            ~Game();

            void Init(void);								// Call Init() before calling any other method
            void SetupResources(void);						// Set up resources for the game
            void SetupScene(void);							// Set up resources for the game
            void MainLoop(void);							// Run the game: keep the application active

        private:
            GLFWwindow* window_;							// GLFW window
            SceneGraph scene_;								// Scene graph containing all nodes to render
			ResourceManager resman_;						// Resources available to the game
            Camera camera_;									// Camera abstraction
            bool animating_;								// Flag to turn animation on/off
			bool gamestart_;								// Checking for the gamestate for menu screen
			SceneNode *menuNode;							// Adding a sceneNode for the menu
			CameraNode* camNode;							// SceneNode for the camera to add to the hierarchy 
			Fly* player;									// Player fly
			SceneNode* target;								// Target for shooting
			SceneNode* world;								// Dummy for root of the heirarchy
			std::vector<Block*> blocks;						// list of draggable objs
			Environment* environment;						// Environment
			Room* room;										// A room (not used)
			Room* room2;									// Not using this
			ParticleNode *dragonFlyParticle;				// Particle for dragonfly death
			ParticleNode *flyParticle;						// Particle for fly rockets
			ParticleNode *humanParticle;					// Particle system for human death
			ParticleNode *spiderParticle;					// Particle system for spider death
			ParticleNode *humanParticleRing;				// Particle system for Human explosion  
	    	ParticleNode *ringParticle1;					// Particle system for ring particles
	    	ParticleNode *ringParticle2;					// Particle system for ring particles
			std::vector<Rocket*> rockets;					// All Rockets
			std::vector<Web*> webs;							// All webs
			std::vector<DragonFly*> dragonFlies;			// All dragonflies
			std::vector<Human*> humans;						// All humans
			std::vector<Spider*> spiders;					// All Spiders

            // Methods to initialize the game
            void InitWindow(void);
            void InitView(void);
            void InitEventHandlers(void);
 
            // Methods to handle events
            static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);		// Callback for key presses 
			static void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);			// Callback for mouse presses
            static void ResizeCallback(GLFWwindow* window, int width, int height);							// Callback for resizing the screen
			
			void checkInput();																				// Check for input
			void update();																					// Update everything in the game
			void gameCollisionDetection();																	// All game collision detection
			void projectileCollision();																		// All Projectile Collision detection
			void environmentCollision();																	// All Environment Collision detection
			void enemiesCollision();																		// Player collisions
			void blocksCollision();																			// collision detection for the blocks
			
			bool input_shift = false;;

			std::vector<Resource*> loadAssetResources(std::string, std::string, std::string);
			Rocket* createRocket(std::string, glm::vec3 direction, glm::vec3 pos);							// Create a rocket instance
			Web* createWeb(std::string, glm::vec3 direction, glm::vec3 pos);								// Create a web instance
			Fly* createFly(std::string entity_name);														// Create a fly instance
			Human* createHuman(std::string entity_name, glm::vec3 pos);										// Create a human instance
			SceneNode* createTarget(std::string entity_name);												// Create a target instance 
			SceneNode* createHealthBar(std::string entity_name);												// Create a target instance 
			Spider* createSpider(std::string entity_name, glm::vec3 pos);									// Create a spider instance
			DragonFly* createDragonFly(std::string entity_name, glm::vec3 pos);								// Create a dragonfly instance
			SceneNode* createSky();																			// Create a sky
			ParticleNode* createParticle(std::string entity_name, std::string geometry, std::string material, std::string texture, glm::vec3 scale, bool insertFlag = false);// Create particles
			Block* createBlock(std::string entity_name, glm::vec3 pos);										// Create a block
			Room* createRoom(std::string entity_name, int);													// Create a room with 4 walls and a floor
			SceneNode* createSceneNode(std::string, std::string, std::string, std::string);					// General SceneNode creator
	}; // class Game
} // namespace game
#endif // GAME_H_
