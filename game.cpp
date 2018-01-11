 #include <iostream>
#include <time.h>
#include <sstream>

#include "game.h"
#include "bin/path_config.h"

// Spacebar to shoot rocket
// W,A,S,D for movement
// Right click to move from first to third person
// Up,down,left,right arrow keys for camera movement 
// Z and C to roll the camera
// Ascent and Descent are Q and E respectively
// G to get a draggable object when close enough to it
// Esc to quit 
namespace game 
{
	// Some configuration constants
	// They are written here as global variables, but ideally they should be loaded from a configuration file

	// Main window settings
	const std::string window_title_g = "FLYING UNDERSIZED CONTROLLED KILLER";
	const unsigned int window_width_g = 1024;
	const unsigned int window_height_g = 768;
	const bool window_full_screen_g = false;

	// Viewport and camera settings
	float camera_near_clip_distance_g = 0.01;
	float camera_far_clip_distance_g = 1000.0;
	//float camera_fov_g = 20.0; // Field-of-view of camera
	float camera_fov_g = 70.0; // Field-of-view of camera
	const glm::vec3 viewport_background_color_g(0.0, 0.0, 0.0);
	glm::vec3 camera_position_g(0.0, 0.0, 10.0);
	glm::vec3 camera_look_at_g(0.0, 0.0, 0.0);
	glm::vec3 camera_up_g(0.0, 1.0, 0.0);

	const std::string material_directory_g = MATERIAL_DIRECTORY;	// Materials 

	Game::Game(void) {}
	Game::~Game() { glfwTerminate(); }

	void Game::Init(void) 
	{
		// Run all initialization steps
		InitWindow();
		InitView();
		InitEventHandlers();

		// Set variables
		animating_ = true;
		gamestart_ = false;
		world = new SceneNode("world", 0, 0, 0);	// Dummy Node
		scene_.SetRoot(world);						// Set dummy as Root of Heirarchy
		world->AddChild(camNode);					// Set the camera as a child of the world
	}

	void Game::InitWindow(void) 
	{
		// Initialize the window management library (GLFW)
		if (!glfwInit()) { throw(GameException(std::string("Could not initialize the GLFW library"))); }

		// Create a window and its OpenGL context
		if (window_full_screen_g) { window_ = glfwCreateWindow(window_width_g, window_height_g, window_title_g.c_str(), glfwGetPrimaryMonitor(), NULL); }
		else { window_ = glfwCreateWindow(window_width_g, window_height_g, window_title_g.c_str(), NULL, NULL); }
		if (!window_) 
		{
			glfwTerminate();
			throw(GameException(std::string("Could not create window")));
		}

		// Make the window's context the current one
		glfwMakeContextCurrent(window_);

		// Initialize the GLEW library to access OpenGL extensions
		// Need to do it after initializing an OpenGL context
		glewExperimental = GL_TRUE;
		GLenum err = glewInit();
		if (err != GLEW_OK) { throw(GameException(std::string("Could not initialize the GLEW library: ") + std::string((const char *)glewGetErrorString(err)))); }
	}

	void Game::InitView(void)
	{
		// Set up z-buffer
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);

		// Set viewport
		int width, height;
		glfwGetFramebufferSize(window_, &width, &height);
		glViewport(0, 0, width, height);

		/* Set up camera */
		camera_.SetView(camera_position_g, camera_look_at_g, camera_up_g);		// Set current view
		camera_.SetProjection(camera_fov_g, camera_near_clip_distance_g, camera_far_clip_distance_g, width, height);	// Set projection
		camNode = new CameraNode(&camera_);		//setup camera as a sceneNode
	}

	void Game::InitEventHandlers(void)
	{
		/* Set event callbacks */
		glfwSetKeyCallback(window_, KeyCallback);
		glfwSetMouseButtonCallback(window_, MouseButtonCallback);
		// Something
		glfwSetInputMode(window_, GLFW_STICKY_KEYS, 1);
		glfwSetFramebufferSizeCallback(window_, ResizeCallback);

		glfwSetWindowUserPointer(window_, (void *) this);	// Set pointer to game object, so that callbacks can access it
	}

	void Game::SetupResources(void)
	{
		/* Create Built-In Geometries */
		resman_.CreateCylinder("rocketMesh");
		resman_.CreateWall("wallMesh");
		resman_.CreateSphere("simpleSphereMesh");
		resman_.CreateCylinder("targetMesh", 0.1, 0.6, 0.35, 4, 4, glm::vec3(1,0,0));
		resman_.CreateCube("CubeMesh");
		resman_.CreateSphereParticles("SphereParticle");
		resman_.CreateTorusParticles("TorusParticle", 20000, 0.002, 0.002);
		resman_.CreateTorusParticles("RingParticle");
		resman_.CreateConeParticles("ConeParticle");
		resman_.CreateControlPoints("ControlPoints", 64);

		/* Loading Material for Particle System */
		std::string filename = std::string(MATERIAL_DIRECTORY) + std::string("/fire");
		resman_.LoadResource(Material, "FireMaterial", filename.c_str());
		filename = std::string(MATERIAL_DIRECTORY) + std::string("/particle");
		resman_.LoadResource(Material, "ExplosionMaterial", filename.c_str());
		filename = std::string(MATERIAL_DIRECTORY) + std::string("/death");
		resman_.LoadResource(Material, "deathMaterial", filename.c_str());
		filename = std::string(MATERIAL_DIRECTORY) + std::string("/bullet");
		resman_.LoadResource(Material, "bulletMaterial", filename.c_str());
		filename = std::string(MATERIAL_DIRECTORY) + std::string("/spline");
		resman_.LoadResource(Material, "splineMaterial", filename.c_str());
		filename = std::string(MATERIAL_DIRECTORY) + std::string("/ring");
		resman_.LoadResource(Material, "ringMaterial", filename.c_str());
		
		/* Loading PointSet for Particle System */
		filename = std::string(MATERIAL_DIRECTORY) + std::string("/assets/humanBody.obj");
		resman_.LoadResource(PointSet, "humanBodyParticle", filename.c_str(), 200000);
		filename = std::string(MATERIAL_DIRECTORY) + std::string("/assets/humanLeftHand.obj");
		resman_.LoadResource(PointSet, "humanLeftParticle", filename.c_str(), 200000);
		filename = std::string(MATERIAL_DIRECTORY) + std::string("/assets/humanRightHand.obj");
		resman_.LoadResource(PointSet, "humanRightParticle", filename.c_str(), 200000);
		filename = std::string(MATERIAL_DIRECTORY) + std::string("/assets/humanLeftLeg.obj");
		resman_.LoadResource(PointSet, "humanLeftLegParticle", filename.c_str(), 200000);
		filename = std::string(MATERIAL_DIRECTORY) + std::string("/assets/humanRightLeg.obj");
		resman_.LoadResource(PointSet, "humanRightLegParticle", filename.c_str(), 200000);

		filename = std::string(MATERIAL_DIRECTORY) + std::string("/assets/dragonflyfull.obj");
		resman_.LoadResource(PointSet, "dragonFlyParticle", filename.c_str());
		filename = std::string(MATERIAL_DIRECTORY) + std::string("/assets/flyfull.obj");
		resman_.LoadResource(PointSet, "flyParticle", filename.c_str());
		filename = std::string(MATERIAL_DIRECTORY) + std::string("/assets/humanfull.obj");
		resman_.LoadResource(PointSet, "humanParticle", filename.c_str());
		filename = std::string(MATERIAL_DIRECTORY) + std::string("/assets/spiderfull.obj");
		resman_.LoadResource(PointSet, "spiderParticle", filename.c_str());

		/* Create Resources */

		/* MATERIAL GLSL FILES */
		// OBJECT MATREIAL FOR GENERAL OBJECTS
		filename = std::string(MATERIAL_DIRECTORY) + std::string("/material");
		resman_.LoadResource(Material, "objectMaterial", filename.c_str());

		// TEXTURE MATERIAL FOR OBJECTS THAT HAVE TEXTURES
		filename = std::string(MATERIAL_DIRECTORY) + std::string("/texture");
		resman_.LoadResource(Material, "textureMaterial", filename.c_str());

		/* TEXTURES */

		// WEB TEXTURE
		filename = std::string(MATERIAL_DIRECTORY) + std::string("/textures/webTexture.png");
		resman_.LoadResource(Texture, "webTex", filename.c_str());

		// ROCKET TEXTURE
		filename = std::string(MATERIAL_DIRECTORY) + std::string("/textures/rocketTexture.png");
		resman_.LoadResource(Texture, "rocketTex", filename.c_str());

		// HUMAN TEXTURE
		filename = std::string(MATERIAL_DIRECTORY) + std::string("/textures/skin.png");
		resman_.LoadResource(Texture, "humanTex", filename.c_str());

		// FLY TEXTURES
		filename = std::string(MATERIAL_DIRECTORY) + std::string("/textures/flyBodyTexture.png");
		resman_.LoadResource(Texture, "flyBodyTex", filename.c_str());
		filename = std::string(MATERIAL_DIRECTORY) + std::string("/textures/flyWingsTexture.png");
		resman_.LoadResource(Texture, "flyWingsTex", filename.c_str());

		// SPIDER TEXTURES
		filename = std::string(MATERIAL_DIRECTORY) + std::string("/textures/spiderBodyTexture.png");
		resman_.LoadResource(Texture, "spiderBodyTex", filename.c_str());

		// DRAGONFLY TEXTURES
		filename = std::string(MATERIAL_DIRECTORY) + std::string("/textures/dragonFlyBodyTexture.png");
		resman_.LoadResource(Texture, "dragonFlyBodyTex", filename.c_str());
		filename = std::string(MATERIAL_DIRECTORY) + std::string("/textures/dragonFlyWingsTexture.png");
		resman_.LoadResource(Texture, "dragonFlyWingsTex", filename.c_str());
		filename = std::string(MATERIAL_DIRECTORY) + std::string("/textures/dragonFlyLegsTexture.png");
		resman_.LoadResource(Texture, "dragonFlyLegsTex", filename.c_str());

		// ENVIRONMENT TEXTURES
		filename = std::string(MATERIAL_DIRECTORY) + std::string("/textures/floorTexture.jpg");
		resman_.LoadResource(Texture, "floorTex", filename.c_str());
		filename = std::string(MATERIAL_DIRECTORY) + std::string("/textures/Cubes-3D-wall-panels-close-up-604x330.jpg");
		resman_.LoadResource(Texture, "wallTex", filename.c_str());
		filename = std::string(MATERIAL_DIRECTORY) + std::string("/textures/sky-texture.jpg");
		resman_.LoadResource(Texture, "skyTex", filename.c_str());

		// BLOCK TEXTURE
		filename = std::string(MATERIAL_DIRECTORY) + std::string("/textures/metalTexture.png");
		resman_.LoadResource(Texture, "blockTex", filename.c_str());

		// FIRE TEXTURE
		filename = std::string(MATERIAL_DIRECTORY) + std::string("/textures/flame4x4orig.png");
		resman_.LoadResource(Texture, "Flame", filename.c_str());

		// MENU TEXTURE 
		filename = std::string(MATERIAL_DIRECTORY) + std::string("/textures/menuscreen.jpg");
		resman_.LoadResource(Texture, "menuTex", filename.c_str());

		/* GEOMETRIES */

		// HUMAN BODY LEFT AND RIGHT HANDS AND LEGS
		filename = std::string(MATERIAL_DIRECTORY) + std::string("/assets/humanBody.obj");
		resman_.LoadResource(Mesh, "humanBodyMesh", filename.c_str());
		filename = std::string(MATERIAL_DIRECTORY) + std::string("/assets/humanLeftHand.obj");
		resman_.LoadResource(Mesh, "humanLeftHandMesh", filename.c_str());
		filename = std::string(MATERIAL_DIRECTORY) + std::string("/assets/humanRightHand.obj");
		resman_.LoadResource(Mesh, "humanRightHandMesh", filename.c_str());
		filename = std::string(MATERIAL_DIRECTORY) + std::string("/assets/humanLeftLeg.obj");
		resman_.LoadResource(Mesh, "humanLeftLegMesh", filename.c_str());
		filename = std::string(MATERIAL_DIRECTORY) + std::string("/assets/humanRightLeg.obj");
		resman_.LoadResource(Mesh, "humanRightLegMesh", filename.c_str());

		//healthBar->SetScale(healthBar->GetScale() - glm::vec3(health * (1.0 / 3.2), 0.0, 0.0));
		// SPIDER BODY and RIGHT AND LEFT LEGS
		filename = std::string(MATERIAL_DIRECTORY) + std::string("/assets/spiderBody.obj");
		resman_.LoadResource(Mesh, "spiderBodyMesh", filename.c_str());
		filename = std::string(MATERIAL_DIRECTORY) + std::string("/assets/spiderLeftLeg.obj");
		resman_.LoadResource(Mesh, "spiderLeftLegMesh", filename.c_str());
		filename = std::string(MATERIAL_DIRECTORY) + std::string("/assets/spiderRightLeg.obj");
		resman_.LoadResource(Mesh, "spiderRightLegMesh", filename.c_str());

		// DRAGONFLY BODY, RIGHT and LEFT WINGS and LEGS
		filename = std::string(MATERIAL_DIRECTORY) + std::string("/assets/dragonFlyBody.obj");
		resman_.LoadResource(Mesh, "dragonFlyBodyMesh", filename.c_str());
		filename = std::string(MATERIAL_DIRECTORY) + std::string("/assets/dragonFlyLeftWing.obj");
		resman_.LoadResource(Mesh, "dragonFlyLeftWingMesh", filename.c_str());
		filename = std::string(MATERIAL_DIRECTORY) + std::string("/assets/dragonFlyRightWing.obj");
		resman_.LoadResource(Mesh, "dragonFlyRightWingMesh", filename.c_str());
		filename = std::string(MATERIAL_DIRECTORY) + std::string("/assets/dragonFlyLegs.obj");
		resman_.LoadResource(Mesh, "dragonFlyLegsMesh", filename.c_str());

		// FLY BODY WINGS AND LEGS
		filename = std::string(MATERIAL_DIRECTORY) + std::string("/assets/flyBody.obj");
		resman_.LoadResource(Mesh, "flyBodyMesh", filename.c_str());
		filename = std::string(MATERIAL_DIRECTORY) + std::string("/assets/flyWings.obj");
		resman_.LoadResource(Mesh, "flyWingsMesh", filename.c_str());
		filename = std::string(MATERIAL_DIRECTORY) + std::string("/assets/flyLegs.obj");
		resman_.LoadResource(Mesh, "flyLegsMesh", filename.c_str());

		//loading material for the screen-space effect
		filename = std::string(MATERIAL_DIRECTORY) + std::string("/screen_space");
		resman_.LoadResource(Material, "ScreenSpaceMaterial", filename.c_str());

		scene_.SetupDrawToTexture();
	}

	/* Setup game elements */
	void Game::SetupScene(void)
	{
		scene_.SetBackgroundColor(viewport_background_color_g);		// Set background color for the scene
		//scene_.SetUpHealthData();                                   // Set up data for the screen space effect

		/* Menu Screen */
		menuNode = createSceneNode("menuInstance", "wallMesh", "textureMaterial", "menuTex");
		menuNode->Rotate(glm::angleAxis(glm::pi<float>(), glm::vec3(1, 0, 0)));
		menuNode->SetPosition(camera_.GetPosition());
		menuNode->Translate(glm::vec3(0, 0, -1.3));
		world->AddChild(menuNode);
		
		/* creating ParticleNode */
		dragonFlyParticle = createParticle("dragonFlyParticleInstance", "dragonFlyParticle", "ExplosionMaterial", "", glm::vec3(40, 40, 40));
		spiderParticle = createParticle("spiderParticleInstance", "spiderParticle", "deathMaterial", "", glm::vec3(0.02, 0.02, 0.02));
		humanParticle = createParticle("humanParticleInstance", "humanParticle", "FireMaterial", "Flame", glm::vec3(1, 1, 1));
		flyParticle = createParticle("flyParticleInstance", "flyParticle", "ExplosionMaterial", "", glm::vec3(1, 1, 1));
		humanParticleRing = createParticle("humanParticleInstance2", "humanParticle", "ringMaterial", "", glm::vec3(1, 1, 1));
		ringParticle1 = createParticle("ringInstance1", "RingParticle", "ringMaterial", "", glm::vec3(0.8, 0.8, 0.8));
		ringParticle2 = createParticle("ringInstance2", "RingParticle", "ringMaterial", "", glm::vec3(0.8, 0.8, 0.8));
		
		player = createFly("player");											
		player->body->SetVisible(false);
		target = createTarget("playerTarget");																			
		player->healthBar = createHealthBar("playerHealthBar");										

		int randomx;
		int randomz;

		//creating human
		for (int i = 0; i < 5; i++)
		{
			//choosing which room to go
			if (rand() % 2 == 0)
			{
				randomx = rand() % 500 - 250;
				randomz = rand() % 500 - 250;
				createHuman("human", glm::vec3(randomx, 0, randomz));
			}
			else
			{
				randomx = rand() % 500 + 230;
				randomz = rand() % 500 - 850;
				createHuman("human", glm::vec3(randomx, 0, randomz));
			}
		}

		//creating spider
		for (int i = 0; i < 5; i++)
		{
			//choosing which room to go
			if (rand() % 2 == 0)
			{
				randomx = rand() % 500 - 250;
				randomz = rand() % 500 - 250;
				createSpider("spider", glm::vec3(randomx, 0, randomz));
			}
			else
			{
				randomx = rand() % 500 + 230;
				randomz = rand() % 500 - 850;
				createSpider("spider", glm::vec3(randomx, 0, randomz));
			}
		}

		//creating dragonfly
		for (int i = 0; i < 5; i++)

		{
			//choosing which room to go
			if (rand() % 2 == 0)
			{
				randomx = rand() % 500 - 250;
				randomz = rand() % 500 - 250;
				createDragonFly("dragonfly", glm::vec3(randomx, 0, randomz));
			}
			else
			{
				randomx = rand() % 500 + 230;
				randomz = rand() % 500 - 850;
				createDragonFly("dragonfly", glm::vec3(randomx, 0, randomz));
			}
		}

		//creating block
		for (int i = 0; i < 7; i++)
		{
			//choosing which room to go
			if (rand() % 2 == 0)
			{
				randomx = rand() % 500 - 250;
				randomz = rand() % 500 - 250;
				createBlock("block", glm::vec3(randomx, -20.3, randomz));
			}
			else
			{
				randomx = rand() % 500 + 230;
				randomz = rand() % 500 - 850;
				createBlock("block", glm::vec3(randomx, -20.3, randomz));
			}
		}

		environment = new Environment();
		room = createRoom("Room1", 0);
		room2 = createRoom("Room2", 1);
		environment->addRoom(room);
		environment->addRoom(room2);

		SceneNode *room2Floor = room2->getFloor();
		room2Floor->Translate(glm::vec3(0, 0, -600));
		room2Floor->Rotate(glm::angleAxis(glm::pi<float>(), glm::vec3(0, 0, 1)));
		room2Floor->Translate(glm::vec3(480, 0, 0));

		//Since it is just a decoration, do we need to store the pointer of that?
		SceneNode *sky = createSky();
	}

	void Game::MainLoop(void)
	{
		// Loop while the user did not close the window
		while (!glfwWindowShouldClose(window_))
		{
			/* INPUT */
			checkInput(); 

			/* DRAW */
			scene_.Draw(&camera_);		// Draw the scene

			std::cout << "spiders: " << spiders.size() << std::endl;
			std::cout << "dragonflies: " << dragonFlies.size() << std::endl;
			std::cout <<  "humans: " << humans.size() << std::endl;

			//check if player health > 0 & gamestate & if we no longer have enemies 
			if (gamestart_ && !(humans.size() == 0 && spiders.size() == 0 && dragonFlies.size() == 0))
			{
				if (player->health > 0)
				{
					/* COLLISION DETECTION */
					gameCollisionDetection();

					/* UPDATE */
					update();
				}

				//scene_.UpdateHealthData(player->health, player->maxHealth);
				//scene_.DrawToTexture(&camera_);
				//scene_.DisplayTexture(resman_.GetResource("ScreenSpaceMaterial")->GetResource());
			}
			
			glfwSwapBuffers(window_);	// Push buffer drawn in the background onto the display
			glfwPollEvents();			// Update other events like input handling
		}
	}

	void Game::MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
	{
		// Get user data with a pointer to the game class
		void* ptr = glfwGetWindowUserPointer(window);
		Game *game = (Game *)ptr;
		
		if (game->gamestart_)
			// Mouse click checks for 1st or third person
			if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) 
			{
				//update health bar
				if (game->camera_.firstPerson)
				{
					game->player->healthBar->Translate(glm::vec3(0, 0, 1) * game->camera_.distance);
				}
				else { game->player->healthBar->Translate(glm::vec3(0, 0, -1) * game->camera_.distance); }

				game->camera_.firstPerson = !game->camera_.firstPerson; 
			}
	}

	void Game::KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
	{
		// Get user data with a pointer to the game class
		void* ptr = glfwGetWindowUserPointer(window);
		Game* game = (Game *)ptr;

		if (game->gamestart_)
		{

			// Quit game if ESC button is pressed
			if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) { glfwSetWindowShouldClose(window, true); }

			if (key == GLFW_KEY_G && action == GLFW_PRESS)
			{
				if (game->player->myBlock == NULL)
				{
					for (int i = 0; i < game->blocks.size(); i++)
					{
						// Drag add alittle to the bounding box for the block we want to drag
						if (game->player->collision(game->blocks[i]->object, game->blocks[i]->offset, game->blocks[i]->boundingRadius + 1.f))
						{
							std::cout << "dragging" << std::endl;
							game->world->RemoveChild(game->blocks[i]->object);
							game->player->myBlock = game->blocks[i];
							game->player->myBlock->beingDragged = true;
							game->player->myBlock->dropped = false;
							game->player->myBlock->hitFloor = true;
							game->blocks[i]->object->SetPosition(game->player->body->GetPosition() + (glm::vec3(0, -0.8, 0)));
							game->player->body->AddChild(game->blocks[i]->object);
						}
					}
				}
				// Drop
				else
				{
					std::cout << "dropping" << std::endl;
					game->player->myBlock->dropped = true;
					game->player->myBlock->beingDragged = false;
					game->player->body->RemoveChild(game->player->myBlock->object);
					game->player->myBlock->object->SetPosition(game->player->myBlock->object->getAbsolutePosition());
					game->world->AddChild(game->player->myBlock->object);
					game->player->myBlock = 0;
				}
			}
		}
	}

	/* Resize Screen */
	void Game::ResizeCallback(GLFWwindow* window, int width, int height)
	{
		// Set up viewport and camera projection based on new window size
		glViewport(0, 0, width, height);
		void* ptr = glfwGetWindowUserPointer(window);
		Game *game = (Game *)ptr;
		game->camera_.SetProjection(camera_fov_g, camera_near_clip_distance_g, camera_far_clip_distance_g, width, height);
	}

	// CHECKING WHETHER A BUTTON IS HELD DOWN OR NOT IS AN ISSUE???????? WITH THE CHECK INPUT FUNCTION
	void Game::checkInput()
	{
		if (gamestart_)
		{
			// View control
			float rot_factor(glm::pi<float>() / 360);

			// Move camera up, down, and to the sides
			if (glfwGetKey(window_, GLFW_KEY_UP)) { camera_.Pitch(rot_factor); }
			if (glfwGetKey(window_, GLFW_KEY_DOWN)) { camera_.Pitch(-rot_factor); }
			if (glfwGetKey(window_, GLFW_KEY_LEFT)) { camera_.Yaw(rot_factor); }
			if (glfwGetKey(window_, GLFW_KEY_RIGHT)) { camera_.Yaw(-rot_factor); }

			// Forward backward and side movements
			if (glfwGetKey(window_, GLFW_KEY_W)) { camera_.Translate(camera_.GetForward() * player->speed); }
			if (glfwGetKey(window_, GLFW_KEY_S)) { camera_.Translate(-camera_.GetForward() * player->speed); }
			if (glfwGetKey(window_, GLFW_KEY_A)) { camera_.Translate(-camera_.GetSide() * player->speed); }
			if (glfwGetKey(window_, GLFW_KEY_D)) { camera_.Translate(camera_.GetSide() * player->speed); }

			// Roll camera
			if (glfwGetKey(window_, GLFW_KEY_Z)) { camera_.Roll(-rot_factor); }
			if (glfwGetKey(window_, GLFW_KEY_C)) { camera_.Roll(rot_factor); }

			// TO BE CHANGED!!!!!!!!!!!!!! (movement up and down)
			if (glfwGetKey(window_, GLFW_KEY_Q)) { camera_.Translate(camera_.GetUp() * player->speed); }
			if (glfwGetKey(window_, GLFW_KEY_E)) { camera_.Translate(-camera_.GetUp() * player->speed); }

			// Shoot a rocket
			if (glfwGetKey(window_, GLFW_KEY_SPACE))
			{
				if (player->fireRate <= 0)
				{
					ParticleNode *particle = createParticle("rocketParticle1", "ConeParticle", "bulletMaterial", "", glm::vec3(1, 1, 1));
					//particle->getParticle()->SetBlending(false);
					particle->startAnimate(target->getAbsolutePosition() - player->body->getAbsolutePosition(), player->body->getAbsoluteOrientation(), 999);
					particle->getParticle()->Rotate(glm::angleAxis(glm::pi<float>() / 2, glm::vec3(1.0, 0.0, 0.0)));
					player->rockets_particles.push_back(particle);

					player->rockets.push_back(createRocket("Rocket1", target->getAbsolutePosition() - player->body->getAbsolutePosition(), player->body->getAbsolutePosition()));
					player->fireRate = player->maxFireRate;
				}
			}
		}
		else
		{
			if (glfwGetKey(window_, GLFW_KEY_SPACE))
			{
				gamestart_ = true;
				menuNode->SetVisible(false);
				player->body->SetVisible(true);
			}
		}
	}

	SceneNode *Game::createSky()
	{
		SceneNode* sky = createSceneNode("SkyInstance", "wallMesh", "textureMaterial", "skyTex");
		world->AddChild(sky);
		sky->Rotate(glm::angleAxis(glm::pi<float>() / 2, glm::vec3(1.0, 0.0, 0.0)));
		sky->Translate(glm::vec3(0, 600, 0));
		sky->SetScale(glm::vec3(1000, 1000, 1));
		return 0;
	}

	// (MAYBE MAKE IT INDEPENDENT OF CAMERA LATER ON) initial values are set in the constructor
	Fly* Game::createFly(std::string entity_name)
	{
		// Setup fly parts
		SceneNode* flyBody = createSceneNode(entity_name + "Body", "flyBodyMesh", "textureMaterial", "flyBodyTex");
		SceneNode* flyWings = createSceneNode(entity_name + "Wings", "flyWingsMesh", "textureMaterial", "flyWingsTex");
		SceneNode* flyLegs = createSceneNode(entity_name + "Legs", "flyLegsMesh", "textureMaterial", "flyWingsTex");

		// Setup heirarchy
		camNode->AddChild(flyBody);
		//world->AddChild(flyBody);			// testing
		flyBody->AddChild(flyWings);
		flyBody->AddChild(flyLegs);

		// Setup parts Scales
		flyBody->SetScale(glm::vec3(10, 10, 10));
		flyWings->SetScale(glm::vec3(10, 10, 10));
		flyLegs->SetScale(glm::vec3(10, 10, 10));

		// Setup fly Rotations
		flyBody->Rotate(glm::angleAxis(glm::pi<float>(), glm::vec3(0, 1, 0)));

		// Setup parts Positions
		flyBody->SetPosition(glm::vec3(0, -0.3, -0.6));

		return new Fly(flyBody, flyWings, flyLegs);
	}

	Block* Game::createBlock(std::string entity_name, glm::vec3 pos)
	{
		SceneNode* object = createSceneNode(entity_name, "simpleSphereMesh", "textureMaterial", "blockTex");

		world->AddChild(object);

		// Setup parts Scale, Setup parts Positions
		object->SetScale(glm::vec3(2, 2, 2));
		object->SetPosition(pos);

		Block* bl = new Block(object);
		blocks.push_back(bl);

		return bl;
	}

	// Create a Spider instance
	Spider* Game::createSpider(std::string entity_name, glm::vec3 pos)
	{
		// Setup spider parts 
		SceneNode* spiderBody = createSceneNode(entity_name + "Body", "spiderBodyMesh", "textureMaterial", "spiderBodyTex");
		SceneNode* spiderLeftLeg = createSceneNode(entity_name + "LeftLeg", "spiderLeftLegMesh", "textureMaterial", "spiderBodyTex");
		SceneNode* spiderRightLeg = createSceneNode(entity_name + "RightLeg", "spiderRightLegMesh", "textureMaterial", "spiderBodyTex");

		// Setup heirarchy
		world->AddChild(spiderBody);
		spiderBody->AddChild(spiderLeftLeg);
		spiderBody->AddChild(spiderRightLeg);

		// Setup parts Scaling
		spiderBody->SetScale(glm::vec3(glm::vec3(0.02, 0.02, 0.02)));
		spiderLeftLeg->SetScale(glm::vec3(0.02, 0.02, 0.02));
		spiderRightLeg->SetScale(glm::vec3(glm::vec3(0.02, 0.02, 0.02)));

		// Setup parts positions
		spiderBody->SetPosition(pos);
		spiderBody->Rotate(glm::angleAxis(glm::pi<float>(), glm::vec3(0, 1, 0)));

		Spider* spi = new Spider(spiderBody, spiderLeftLeg, spiderRightLeg);
		spiders.push_back(spi);

		return spi;
	}

	// Create a Dragonfly instance
	DragonFly* Game::createDragonFly(std::string entity_name, glm::vec3 pos)
	{
		// Setup dragonFly parts 
		SceneNode* dragonFlyBody = createSceneNode(entity_name + "Body", "dragonFlyBodyMesh", "textureMaterial", "dragonFlyBodyTex");
		SceneNode* dragonFlyLeftWing = createSceneNode(entity_name + "LeftWing", "dragonFlyLeftWingMesh", "textureMaterial", "dragonFlyWingsTex");
		SceneNode* dragonFlyRightWing = createSceneNode(entity_name + "RightWing", "dragonFlyRightWingMesh", "textureMaterial", "dragonFlyWingsTex");
		SceneNode* dragonFlyLegs = createSceneNode(entity_name + "Legs", "dragonFlyLegsMesh", "textureMaterial", "dragonFlyLegsTex");

		// Setup heirarchy
		world->AddChild(dragonFlyBody);
		dragonFlyBody->AddChild(dragonFlyLeftWing);
		dragonFlyBody->AddChild(dragonFlyRightWing);
		dragonFlyBody->AddChild(dragonFlyLegs);

		// Setup parts scale
		dragonFlyBody->SetScale(glm::vec3(40, 40, 40));
		dragonFlyLeftWing->SetScale(glm::vec3(40, 40, 40));
		dragonFlyRightWing->SetScale(glm::vec3(40, 40, 40));
		dragonFlyLegs->SetScale(glm::vec3(40, 40, 40));

		// Setup parts positions
		dragonFlyBody->SetPosition(pos);

		// Create dragon fly instance and add it to the character collidables
		DragonFly* dragon = new DragonFly(dragonFlyBody, dragonFlyLeftWing, dragonFlyRightWing, dragonFlyLegs);
		dragonFlies.push_back(dragon);

		return dragon;
	}

	// CREATE HUMAN
	Human* Game::createHuman(std::string entity_name, glm::vec3 pos)
	{
		
		SceneNode* humanBody = createSceneNode(entity_name + "Body", "humanBodyMesh", "textureMaterial", "humanTex");
		SceneNode* humanLeftHand = createSceneNode(entity_name + "LeftHand", "humanLeftHandMesh", "textureMaterial", "humanTex");
		SceneNode* humanRightHand = createSceneNode(entity_name + "RightHand", "humanRightHandMesh", "textureMaterial", "humanTex");
		SceneNode* humanLeftLeg = createSceneNode(entity_name + "LeftLeg", "humanLeftLegMesh", "textureMaterial", "humanTex");
		SceneNode* humanRightLeg = createSceneNode(entity_name + "RightLeg", "humanRightLegMesh", "textureMaterial", "humanTex");

		// Setup Heirarchy
		world->AddChild(humanBody);
		humanBody->AddChild(humanLeftHand);
		humanBody->AddChild(humanRightHand);
		humanBody->AddChild(humanLeftLeg);
		humanBody->AddChild(humanRightLeg);

		// Setup part scale
		humanBody->SetScale(glm::vec3(1, 1, 1));
		humanLeftHand->SetScale(glm::vec3(1, 1, 1));
		humanRightHand->SetScale(glm::vec3(1, 1, 1));
		humanLeftLeg->SetScale(glm::vec3(1, 1, 1));
		humanRightLeg->SetScale(glm::vec3(1, 1, 1));

		// Setup part Positions
		humanBody->SetPosition(pos);

		Human* hum = new Human(humanBody, humanLeftHand, humanRightHand, humanLeftLeg, humanRightLeg);
		humans.push_back(hum);

		hum->forwardBall = createSceneNode("meh" , "rocketMesh" , "objectMaterial" , "");
		hum->forwardBall->Rotate(glm::angleAxis(glm::pi<float>()/2 , glm::vec3(1,0,0)));
		hum->forwardBall->SetPosition(hum->body->GetPosition() + glm::vec3(0 , 0 , 1.0));
		hum->forwardBall->SetVisible(false);
		world->AddChild(hum->forwardBall);

		return hum;
	}

	// GENERALIZE TO RECEIVE DIRECTION INSTEAD OF CODING IT FOR ONLY THE FLY AND TEXTURE NAME FOR DIFFERENT TYPE OF ROCKETS
	Rocket* Game::createRocket(std::string entity_name, glm::vec3 direction, glm::vec3 pos)
	{
		SceneNode* rock = createSceneNode(entity_name, "simpleSphereMesh", "textureMaterial", "rocketTex");
		world->AddChild(rock);

		// Set initial values
		rock->SetScale(glm::vec3(0.1, 0.1, 0.1));
		rock->SetOrientation(player->body->getAbsoluteOrientation());
		rock->Rotate(glm::normalize(glm::angleAxis(glm::pi<float>() / 2, glm::vec3(1.0, 0.0, 0.0))));
		rock->SetPosition(pos + 2.f * glm::normalize(direction));
		
		//add rockets for collision detection
		Rocket* rocket = new Rocket(rock, direction);
		rockets.push_back(rocket);

		// Set the rocket node and the direction of the rocket
		return rocket;
	}

	// GENERALIZE TO RECEIVE DIRECTION INSTEAD OF CODING IT FOR ONLY THE FLY AND TEXTURE NAME FOR DIFFERENT TYPE OF ROCKETS
	Web* Game::createWeb(std::string entity_name, glm::vec3 direction, glm::vec3 pos)
	{
		SceneNode* webNode = createSceneNode(entity_name, "simpleSphereMesh", "textureMaterial", "webTex");
		world->AddChild(webNode);

		// Set initial values
		webNode->SetScale(glm::vec3(0.1, 0.1, 0.1));
		webNode->SetOrientation(player->body->getAbsoluteOrientation());
		webNode->Rotate(glm::normalize(glm::angleAxis(glm::pi<float>() / 2, glm::vec3(1.0, 0.0, 0.0))));
		webNode->SetPosition(pos + 4.f * glm::normalize(direction));

		ParticleNode* webParticle = createParticle(entity_name + "Particle", "TorusParticle", "splineMaterial", "", glm::vec3(0.2, 0.2, 0.2), true);
		Resource *cp = resman_.GetResource("ControlPoints");
		webParticle->getParticle()->AddShaderAttribute("control_point", Vec3Type, cp->GetSize(), cp->GetData());
		webParticle->startAnimate(pos + 2.f * glm::normalize(direction), player->body->getAbsoluteOrientation(), 999);
		webParticle->getParticle()->Rotate(glm::normalize(glm::angleAxis(glm::pi<float>() / 2, glm::vec3(1.0, 0.0, 0.0))));

		//add rockets for collision detection
		Web* web = new Web(webNode, webParticle, direction);
		webs.push_back(web);


		// Set the rocket node and the direction of the rocket
		return web;
	}
	
	// TARGET IS A CHILD OF CAMERA SINCE OUR TARGET IS BASED ON THE CAMERA POSITION AND MOVE WITH IT AND NOT THE PLAYER
	SceneNode* Game::createTarget(std::string entity_name)
	{
		// Create a Target instance and make it a child of camera 
		SceneNode* Target = createSceneNode(entity_name, "targetMesh", "objectMaterial", "");
		camNode->AddChild(Target);

		//Set initial values
		Target->SetScale(glm::vec3(1, 1, 1));
		Target->SetPosition(glm::vec3(0, 0, -30));

		return Target;
	}

	// TARGET IS A CHILD OF CAMERA SINCE OUR TARGET IS BASED ON THE CAMERA POSITION AND MOVE WITH IT AND NOT THE PLAYER
	SceneNode* Game::createHealthBar(std::string entity_name)
	{
		// Create a Target instance and make it a child of camera 
		SceneNode* Target = createSceneNode(entity_name, "targetMesh", "objectMaterial", "");
		camNode->AddChild(Target);

		//Set initial values
		Target->SetScale(glm::vec3(3.2, -0.2, 1.9));
		Target->SetPosition(glm::vec3(0, 1, -1.11));

		return Target;
	}
	//GENERALIZE THIS FUNCTION
	Room* Game::createRoom(std::string entity_name, int direction)
	{
		Room* myRoom = new Room();
		float dimensionsWalls = 300; // wall dimensions

		std::string meshname = std::string(entity_name) + std::string("floor");

		SceneNode* floor = createSceneNode(meshname.c_str(), "wallMesh", "textureMaterial", "floorTex");
		world->AddChild(floor);
		floor->SetScale(glm::vec3(300, 300, 1));
		floor->Translate(glm::vec3(0.0, -30.0, 0.0));
		floor->Rotate(glm::angleAxis(glm::pi<float>() / 2, glm::vec3(1.0, 0.0, 0.0)));

		glm::vec3 pos = floor->GetPosition();

		// y-axis is the normal of the floor and size is 600x600
		Wall* floorWall = new Wall(floor, glm::vec3(0, 1, 0), 600, 600);
		myRoom->addWall(floorWall);

		meshname = std::string(entity_name) + std::string("wall1");
		SceneNode* wall = createSceneNode(meshname.c_str(), "wallMesh", "textureMaterial", "wallTex");
		floor->AddChild(wall);
		
		wall->Translate(glm::vec3(-120, -300, -300));
		wall->Rotate(glm::angleAxis(glm::pi<float>() / 2, glm::vec3(-1.0, 0.0, 0.0)));
		wall->SetScale(glm::vec3(300, 300, 1));

		glm::vec3 pos1 = wall->GetPosition();

		//z-axis is the normal of this plane and size is 600x600
		//Wall* myWall1 = new Wall(wall , glm::vec3(0 , 0 , 1), 600, 600);
		Wall* myWall1;
		if (direction == 0)
			myWall1 = new Wall(wall, glm::vec3(0, 0, 1), 600, 600);
		else
			myWall1 = new Wall(wall, glm::vec3(0, 0, -1), 600, 600);
		myRoom->addWall(myWall1);
		
		meshname = std::string(entity_name) + std::string("wall2");
		SceneNode* wall2 = createSceneNode(meshname.c_str(), "wallMesh", "textureMaterial", "wallTex");
		floor ->AddChild(wall2);
		wall2->SetScale(glm::vec3(300, 300, 1));
		wall2->Translate(glm::vec3(300, 0, -300));
		wall2->Rotate(glm::angleAxis(glm::pi<float>() / 2, glm::vec3(-1.0, 0.0, 0.0)));
		wall2->Rotate(glm::angleAxis(glm::pi<float>() / 2, glm::vec3(0.0, 1.0, 0.0)));

		glm::vec3 pos2 = wall2->GetPosition();

		//-x-axis is the normal of this plane size is 600x600
		Wall* myWall2;
		if (direction == 0)
			myWall2 = new Wall(wall2, glm::vec3(-1, 0, 0), 600, 600);
		else
			myWall2 = new Wall(wall2, glm::vec3(1, 0, 0), 600, 600);
		myRoom->addWall(myWall2);
		
		meshname = std::string(entity_name) + std::string("wall3");
		SceneNode* wall3 = createSceneNode(meshname.c_str(), "wallMesh", "textureMaterial", "wallTex");
		floor->AddChild(wall3);
		wall3->SetScale(glm::vec3(300, 300, 1));
		wall3->Translate(glm::vec3(-300.0, 0, -300.0));
		wall3->Rotate(glm::angleAxis(glm::pi<float>() / 2, glm::vec3(-1.0, 0.0, 0.0)));
		wall3->Rotate(glm::angleAxis(glm::pi<float>() / 2, glm::vec3(0.0, 1.0, 0.0)));

		glm::vec3 pos3 = wall3->GetPosition();

		//x-axis is the normal of this plane and size 600x600
		Wall* myWall3;
		if (direction == 0)
			myWall3 = new Wall(wall3, glm::vec3(1, 0, 0), 600, 600);
		else
			myWall3 = new Wall(wall3, glm::vec3(-1, 0, 0), 600, 600);
		myRoom->addWall(myWall3);
		
		meshname = std::string(entity_name) + std::string("wall4");
		SceneNode* wall4 = createSceneNode(meshname.c_str(), "wallMesh", "textureMaterial", "wallTex");
		floor->AddChild(wall4);
		wall4->SetScale(glm::vec3(300, 300, 1));
		wall4->Translate(glm::vec3(0, 300, -300));
		wall4->Rotate(glm::angleAxis(glm::pi <float>() / 2, glm::vec3(-1.0, 0.0, 0.0)));

		glm::vec3 pos4 = wall4->GetPosition();

		//-z-axis is the normal of this plane and size is 600x600
		Wall* myWall4;
		if (direction == 0)
			myWall4 = new Wall(wall4, glm::vec3(0, 0, -1), 600, 600);
		else
			myWall4 = new Wall(wall4, glm::vec3(0, 0, 1), 600, 600);
		myRoom->addWall(myWall4);

		myRoom->SetFloor(floor);

		return myRoom;
	}

	// Function to create a new ParticleNode
	ParticleNode *Game::createParticle(std::string entity_name, std::string geometry, std::string material, std::string texture, glm::vec3 scale, bool insertFlag)
	{
		SceneNode *particle = createSceneNode(entity_name, geometry, material, texture);
		if (insertFlag)
			world->AppendChild(particle);
		else
			world->AddChild(particle);
		particle->SetScale(scale);
		particle->SetBlending(true);
		return new ParticleNode(particle);
	}

	// Function to create a new SceneNode
	SceneNode* Game::createSceneNode(std::string entity_name, std::string geometryName, std::string materialName, std::string textureName )
	{
		// Get resources
		Resource *geom = resman_.GetResource(geometryName);
		if (!geom) { throw(GameException(std::string("Could not find resource \"") + geometryName + std::string("\""))); }
		Resource *mat = resman_.GetResource(materialName);
		if (!mat) { throw(GameException(std::string("Could not find resource \"") + materialName + std::string("\""))); }
		Resource *tex = NULL;
		if (textureName != "") {
			tex = resman_.GetResource(textureName);
			if (!tex) {
				throw(GameException(std::string("Could not find resource \"") + textureName + std::string("\"")));
			}
		}

		//return a new sceneNode object
		return new SceneNode(entity_name, geom, mat, tex);
	}

	// LOADS GEOMETRY MATERIAL AND TEXTURE STORES THEM IN A VECTOR AND RETURN THEM
	std::vector<Resource*> Game::loadAssetResources(std::string geometryName, std::string materialName, std::string textureName)
	{
		std::vector<Resource*> resources;	// Holder of resources

		// Load each resource add it to the vector check if its null then return result
		resources.push_back(resman_.GetResource(geometryName));
		if (!resources[0]) { throw(GameException(std::string("Could not find resource \"") + geometryName + std::string("\""))); }
		resources.push_back(resman_.GetResource(materialName));
		if (!resources[1]) { throw(GameException(std::string("Could not find resource \"") + materialName + std::string("\""))); }
		resources.push_back(resman_.GetResource(textureName));
		if (!resources[2]) { std::cout << std::string("Could not find resource \"") + textureName + std::string("\"") << std::endl; }	// FOR OBJECTS THAT DO NOT HAVE TEXTURES

		return resources;
	}

	void Game::update()
	{
		/* CHECK THE PARTICLE SYSTEM TIME */
		dragonFlyParticle->update(); if (dragonFlyParticle->shouldDisappear) { dragonFlyParticle->shouldDisappear = false; dragonFlyParticle->getParticle()->SetVisible(false); }		
 		spiderParticle->update(); if (spiderParticle->shouldDisappear) { spiderParticle->shouldDisappear = false; spiderParticle->getParticle()->SetVisible(false); }		
 		humanParticle->update(); if (humanParticle->shouldDisappear) { 		
 			humanParticle->shouldDisappear = false; humanParticle->getParticle()->SetVisible(false); 		
 			humanParticleRing->startAnimate(humanParticle->getParticle()->getAbsolutePosition(), humanParticle->getParticle()->getAbsoluteOrientation(), 4);		
 		}		
 		flyParticle->update(); if (flyParticle->shouldDisappear) { flyParticle->shouldDisappear = false; flyParticle->getParticle()->SetVisible(false); }		
 		humanParticleRing->update(); if (humanParticleRing->shouldDisappear) { humanParticleRing->shouldDisappear = false; humanParticleRing->getParticle()->SetVisible(false); }		
 		ringParticle1->update(); if (ringParticle1->shouldDisappear) { ringParticle1->shouldDisappear = false; ringParticle1->getParticle()->SetVisible(false); }		
 		ringParticle2->update(); if (ringParticle2->shouldDisappear) { ringParticle2->shouldDisappear = false; ringParticle2->getParticle()->SetVisible(false); }

		/* UPDATE */
		// Check distances before updating
		// redo locking on to player
		if (player)
		{
			if (player->health <= 0)
			{
				player->body->del = true;					// Delete node from sceneGraph
				player->wings->del = true;					// Delete node from sceneGraph
				player->legs->del = true;					// Delete node from sceneGraph
				std::cout << "You are Dead" << std::endl;
				//delete player;
			}
			else
			{
				player->update();
			}
		}

		for (int i = 0; i < dragonFlies.size(); i++)
		{
			// Check if dragonfly has any leftover health if it does update else kill the dragonfly
			if (dragonFlies[i]->health <= 0)
			{
				dragonFlyParticle->startAnimate(dragonFlies[i]->body->getAbsolutePosition(), dragonFlies[i]->body->getAbsoluteOrientation(), 3);
				dragonFlies[i]->body->del = true;			// Delete node from sceneGraph
				dragonFlies[i]->leftWing->del = true;		// Delete node from sceneGraph
				dragonFlies[i]->rightWing->del = true;		// Delete node from sceneGraph
				dragonFlies[i]->legs->del = true;			// Delete node from sceneGraph
				dragonFlies[i]->deleteProjectiles();
				dragonFlies[i]->projectiles.clear();
				delete dragonFlies[i];
				dragonFlies.erase(dragonFlies.begin() + i); // Delete from dragonfly vector
			}
			else  // check distances before updating 
			{
				glm::vec3 distancce = player->body->getAbsolutePosition() - dragonFlies[i]->body->getAbsolutePosition();

				if (glm::length(distancce) < 500.0)
				{
					dragonFlies[i]->updateTarget(player->body->getAbsolutePosition());
					dragonFlies[i]->updateTargetOrientation(player->body->getAbsoluteOrientation());
					dragonFlies[i]->update();
					if (dragonFlies[i]->getFiring()) { dragonFlies[i]->fire(createRocket("Rocket4", dragonFlies[i]->getDirection(), dragonFlies[i]->body->getAbsolutePosition())); }
				}
			}
		}

		for (int j = 0; j < spiders.size(); j++)
		{
			if (spiders[j]->health <= 0)
			{
				spiderParticle->startAnimate(spiders[j]->body->getAbsolutePosition(), spiders[j]->body->getAbsoluteOrientation(), 3);
				spiders[j]->body->del = true;			// Delete node from sceneGraph
				spiders[j]->leftLeg->del = true;		// Delete node from sceneGraph
				spiders[j]->rightLeg->del = true;		// Delete node from sceneGraph
				spiders[j]->deleteProjectiles();
				spiders[j]->projectiles.clear();
				delete spiders[j];
				spiders.erase(spiders.begin() + j);		// Delete from spiders vector
			}
			else // check distance before updating
			{
				glm::vec3 distancce = player->body->getAbsolutePosition() - spiders[j]->body->getAbsolutePosition();

				if (glm::length(distancce) < 500.0)
				{
					spiders[j]->updateTarget(player->body->getAbsolutePosition());
					spiders[j]->updateTargetOrientation(player->body->getAbsoluteOrientation());
					spiders[j]->update();
					if (spiders[j]->getFiring()) {
						spiders[j]->fire(createWeb("Rocket3", spiders[j]->getDirection(), spiders[j]->body->getAbsolutePosition()));
					}
				}
			}
		}

		for (int k = 0; k < humans.size(); k++)
		{
			if (humans[k]->health <= 0)
			{
				humanParticle->startAnimate(humans[k]->body->getAbsolutePosition(), humans[k]->body->getAbsoluteOrientation(), 5);
				humans[k]->body->del = true;				// Delete node from sceneGraph
				humans[k]->leftLeg->del = true;				// Delete node from sceneGraph
				humans[k]->leftHand->del = true;			// Delete node from sceneGraph
				humans[k]->rightHand->del = true;			// Delete node from sceneGraph
				humans[k]->rightLeg->del = true;			// Delete node from sceneGraph
				delete humans[k];
				humans.erase(humans.begin() + k);			// Delete from human vector
			}
			else  // check distance before updating
			{
				glm::vec3 distancce = player->body->getAbsolutePosition() - humans[k]->body->getAbsolutePosition();

				if (glm::length(distancce) < 500.0)
				{
					humans[k]->updateTarget(player->body->getAbsolutePosition());
					humans[k]->updateTargetOrientation(player->body->getAbsoluteOrientation());
					humans[k]->update();
				}
			}
		}

		//UPDATE BLOCKS
		for (int i = 0; i < blocks.size(); i++)
		{
			blocks[i]->update();
		}

		//UPDATE PARTICLE SYSTEM
		for (int i = 0; i < webs.size(); i++)
		{
			if (!webs[i]->node->del)
			{
				webs[i]->particle->updatePosition(webs[i]->node->getAbsolutePosition());
			}
		}
	}

	void Game::gameCollisionDetection()
	{
		environmentCollision();
		projectileCollision();
		enemiesCollision();
		blocksCollision();
	}

	void Game::projectileCollision()
	{
		/* Rocket Collision detection */

		/* PLAYER ROCKET COLLISION DETECTION */
		bool collided = false;
		for (int i = 0; i < rockets.size(); i++)
		{
			if (rockets[i]->collision(player->body, player->offset, player->boundingRadius))
			{

				rockets[i]->node->del = true;
				rockets.erase(rockets.begin() + i);
				i--;

				player->health -= 10;
				player->healthBar->SetScale(glm::vec3((player->health / player->maxHealth) * 3.2f, player->healthBar->GetScale().y, player->healthBar->GetScale().z));
				collided = true;
				break;
			}

			if (collided) { continue; }

			/* DRAGONFLY COLLISION DETECTION WITH ROCKETS */
			for (int j = 0; j < dragonFlies.size(); j++)
			{
				if (rockets[i]->collision(dragonFlies[j]->body, dragonFlies[j]->offset, dragonFlies[j]->boundingRadius))
				{
					rockets[i]->node->del = true;
					rockets.erase(rockets.begin() + i);
					i--;

					dragonFlies[j]->health -= 10;
					collided = true;
					break;
				}
			}

			if (collided) { continue; }

			/* HUMAN COLLISION DETECTION WITH ROCKETS */
			for (int k = 0; k < humans.size(); k++)
			{
				if (!collided && (rockets[i]->collision(humans[k]->body, humans[k]->offset, humans[k]->boundingRadius)
					|| rockets[i]->collision(humans[k]->body, humans[k]->boundingRadius + humans[k]->offset, humans[k]->boundingRadius)))
				{
					rockets[i]->node->del = true;
					rockets.erase(rockets.begin() + i);
					i--;

					humans[k]->health -= 10;
					collided = true;
					break;
				}
			}

			if (collided) { break; }

			/* SPIDERS COLLISION DETECTION WITH ROCKETS */
			for (int l = 0; l < spiders.size(); l++)
			{
				if (!collided && rockets[i]->collision(spiders[l]->body, spiders[l]->offset, spiders[l]->boundingRadius))
				{
					rockets[i]->node->del = true;
					rockets.erase(rockets.begin() + i);
					i--;

					spiders[l]->health -= 10;
					break;
				}
			}
		}


		collided = false;

		/* PLAYER WEBS COLLISION */
		for (int i = 0; i < webs.size(); i++)
		{
			/* PLAYER COLLISION WITH WEBS */
			if (webs[i]->collision(player->body, player->offset, player->boundingRadius))
			{
				webs[i]->particle->deleteNode();
				webs[i]->node->del = true;
				webs.erase(webs.begin() + i);
				i--;

				player->health -= 10;
				player->healthBar->SetScale(glm::vec3((player->health / player->maxHealth) * 3.2f, player->healthBar->GetScale().y, player->healthBar->GetScale().z));
				collided = true;
				break;
			}

			if (collided) { continue; }

			/* DRAGONFLY COLLISION DETECTION WITH WEBS */
			for (int j = 0; j < dragonFlies.size(); j++)
			{
				if (!collided && webs[i]->collision(dragonFlies[j]->body, dragonFlies[j]->offset, dragonFlies[j]->boundingRadius))
				{
					webs[i]->particle->deleteNode();
					webs[i]->node->del = true;
					webs.erase(webs.begin() + i);
					i--;

					dragonFlies[j]->health -= 10;
					collided = true;
					break;
				}
			}

			if (collided) { continue; }

			/* HUMAN COLLISION DETECTION WITH WEBS */
			for (int k = 0; k < humans.size(); k++)
			{
				if (!collided && (webs[i]->collision(humans[k]->body, humans[k]->offset, humans[k]->boundingRadius)
					|| webs[i]->collision(humans[k]->body, humans[k]->boundingRadius + humans[k]->offset, humans[k]->boundingRadius)))
				{
					webs[i]->particle->deleteNode();
					webs[i]->node->del = true;
					webs.erase(webs.begin() + i);
					i--;

					humans[k]->health -= 10;
					collided = true;
					break;
				}
			}

			if (collided) { continue; }

			/* SPIDERS COLLISION DETECTION WITH WEBS */
			for (int l = 0; l < spiders.size(); l++)
			{
				if (!collided && webs[i]->collision(spiders[l]->body, spiders[l]->offset, spiders[l]->boundingRadius))
				{
					webs[i]->particle->deleteNode();
					webs[i]->node->del = true;
					webs.erase(webs.begin() + i);
					i--;

					spiders[l]->health -= 10;
					break;
				}
			}
		}
	}

	void Game::environmentCollision()
	{
		// PLAYER ROOM COLLISION 
		glm::vec3 norm;

		// BLOCK ROOM COLLISION DETECTION 
		for (int i = 0; i < blocks.size(); i++)
		{
			if (environment->collision(blocks[i]->object, blocks[i]->boundingRadius, blocks[i]->offset, &norm))
			{
				if (norm == glm::vec3(0, 1, 0)) 
				{
					if (blocks[i]->dropped) {}
					if (blocks[i]->hitFloor && blocks[i]->dropped)
					{
						ringParticle1->startAnimate(blocks[i]->object->getAbsolutePosition(), blocks[i]->object->getAbsoluteOrientation(), 4);
						ringParticle2->startAnimate(blocks[i]->object->getAbsolutePosition(), blocks[i]->object->getAbsoluteOrientation(), 4);
						ringParticle2->getParticle()->Rotate(glm::angleAxis(glm::pi<float>() / 2, glm::vec3(0.6, 0.6, 0.6)));
						blocks[i]->hitFloor= false;
					}
					blocks[i]->onFloor = true;

					if (blocks[i]->dropped) 
					{ 
						blocks[i]->object->del = true;
						blocks.erase(blocks.begin() + i);
					}
					//blocks[i]->dropped = false;
				}
			}
			else { blocks[i]->onFloor = false; }
		}

		// PROJECTILES ROOM COLLISION DETECTION 
		for (int k = 0; k < rockets.size(); k++)
		{
			if (environment->collision(rockets[k]->node, rockets[k]->boundingRadius, rockets[k]->offset, &norm))
			{
				rockets[k]->node->del = true;
				rockets.erase(rockets.begin() + k);
				k--;
			}
		}

		// WEBS ROOM COLLISION 
		for (int w = 0; w < webs.size(); w++)
		{
			if (environment->collision(webs[w]->node, webs[w]->boundingRadius, webs[w]->offset, &norm))
			{
				webs[w]->particle->deleteNode();
				webs[w]->node->del = true;
				webs.erase(webs.begin() + w);
				w--;
			}
		}

		if (environment->collision(player->body, player->boundingRadius, player->offset, &norm))
		{
			camera_.Translate(norm * 4.f * player->speed);
		}


		// DRAGONFLIES AND WALLS COLLISION 
		for (int i = 0; i < dragonFlies.size(); i++)
		{
			if (environment->collision(dragonFlies[i]->body, dragonFlies[i]->boundingRadius, dragonFlies[i]->offset, &norm))
			{
				dragonFlies[i]->body->Translate(norm * 2.f * dragonFlies[i]->speed);
			}
		}

		// SPIDERS WALL COLLISION 
		for (int j = 0; j < spiders.size(); j++)
		{
			if (environment->collision(spiders[j]->body, spiders[j]->boundingRadius, spiders[j]->offset, &norm))
			{
				if (norm == glm::vec3(0, 1, 0)) { spiders[j]->onFloor = true; }
				else { spiders[j]->onFloor = false; }
				spiders[j]->body->Translate(norm *spiders[j]->speed);
			}
		}

		// HUMANS WALL COLLISION 
		for (int h = 0; h < humans.size(); h++)
		{
			if (environment->collision(humans[h]->body, humans[h]->boundingRadius, humans[h]->offset, &norm)
				|| environment->collision(humans[h]->body, humans[h]->boundingRadius, humans[h]->offset + humans[h]->boundingRadius, &norm))
			{
				if (norm == glm::vec3(0, 1, 0)) { humans[h]->onFloor = true; }
				else { humans[h]->onFloor = false; }
				humans[h]->body->Translate(norm * 2.f * humans[h]->speed);
			}
		}
	}

	/* ENEMY COLLISION DETECTION */
	void Game::enemiesCollision()
	{
		/* DRAGONFLIES ENEMIES AND PLAYER COLLISION  */
		for (int i = 0; i < dragonFlies.size(); i++)
		{
			//player collision
			if (player->collision(dragonFlies[i]->body, dragonFlies[i]->offset, dragonFlies[i]->boundingRadius))
			{
				glm::vec3 direc = glm::normalize(dragonFlies[i]->body->getAbsolutePosition() - player->body->getAbsolutePosition());
				camera_.Translate(player->speed * 3.f * -direc);
			}

			//Other dragonflies collision
			for (int m = 0; m < dragonFlies.size(); m++)
			{
				if (i != m && dragonFlies[i]->collision(dragonFlies[m]->body, dragonFlies[m]->offset, dragonFlies[m]->boundingRadius))
				{
					dragonFlies[i]->body->Translate(-dragonFlies[i]->getDirection() * 3.f * dragonFlies[i]->speed);
				}
			}

			//spiders collision
			for (int w = 0; w < spiders.size(); w++)
			{
				if (dragonFlies[i]->collision(spiders[w]->body, spiders[w]->offset, spiders[w]->boundingRadius))
				{
					dragonFlies[i]->body->Translate(-dragonFlies[i]->getDirection() * 3.f * dragonFlies[i]->speed);
				}
			}

			//humans collision
			for (int q = 0; q < humans.size(); q++)
			{
				if (dragonFlies[i]->collision(humans[q]->body, humans[q]->offset, humans[q]->boundingRadius)
					|| dragonFlies[i]->collision(humans[q]->body, humans[q]->offset + humans[q]->boundingRadius, humans[q]->boundingRadius))
				{
					dragonFlies[i]->body->Translate(dragonFlies[i]->getDirection() * 2.f * dragonFlies[i]->speed);
				}
			}
		}

		/* SPIDERS PLAYER AND ENEMIES COLLISION */
		for (int j = 0; j < spiders.size(); j++)
		{
			//player collision
			if (player->collision(spiders[j]->body, spiders[j]->offset, spiders[j]->boundingRadius))
			{
				glm::vec3 direc = glm::normalize(player->body->getAbsolutePosition() - spiders[j]->body->getAbsolutePosition());
				camera_.Translate(player->speed * 3.f * direc);
			}

			//other spiders collision
			for (int m = 0; m < spiders.size(); m++)
			{
				if (j != m &&  spiders[j]->collision(spiders[m]->body, spiders[m]->offset, spiders[m]->boundingRadius))
				{
					spiders[j]->body->Translate(-spiders[j]->getDirection() * 2.f * spiders[j]->speed);
				}
			}

			//dragonfly collision
			for (int n = 0; n < dragonFlies.size(); n++)
			{
				if (spiders[j]->collision(dragonFlies[n]->body, dragonFlies[n]->offset, dragonFlies[n]->boundingRadius))
				{
					spiders[j]->body->Translate(-spiders[j]->getDirection() * 2.f * spiders[j]->speed);
				}
			}

			//human collision
			for (int m = 0; m < humans.size(); m++)
			{
				if (j != m && (spiders[j]->collision(humans[m]->body, humans[m]->offset, humans[m]->boundingRadius)
					|| spiders[j]->collision(humans[m]->body, humans[m]->offset + humans[m]->boundingRadius, humans[m]->boundingRadius)
					|| spiders[j]->collision(humans[m]->body, humans[m]->offset + 2 * humans[m]->boundingRadius, humans[m]->boundingRadius)))
				{
					spiders[j]->body->Translate(-spiders[j]->getDirection() * 3.f * spiders[j]->speed);
				}
			}
		}

		/* HUMANS PLAYER AND ENEMIES COLLISION */

		for (int k = 0; k < humans.size(); k++)
		{
			if (player->collision(humans[k]->body, humans[k]->offset, humans[k]->boundingRadius) ||
				player->collision(humans[k]->body, humans[k]->offset + humans[k]->boundingRadius, humans[k]->boundingRadius))
			{
				player->health -= 10;
				player->healthBar->SetScale(glm::vec3((player->health / player->maxHealth) * 3.2f, player->healthBar->GetScale().y, player->healthBar->GetScale().z));
				glm::vec3 direc = glm::normalize(player->body->getAbsolutePosition() - humans[k]->body->getAbsolutePosition());
				camera_.Translate(player->speed * 30.f * direc);
			}

			for (int m = 0; m < humans.size(); m++)
			{
				if (m != k && (humans[k]->collision(humans[m]->body, humans[m]->offset, humans[m]->boundingRadius) ||
					humans[k]->collision(humans[m]->body, humans[m]->offset + humans[m]->boundingRadius, humans[m]->boundingRadius)))
				{
					glm::vec3 direc = glm::normalize(humans[k]->body->getAbsolutePosition() - humans[m]->body->getAbsolutePosition());
					humans[k]->body->Translate(direc * humans[k]->speed * 3.f);
				}
			}
		}
	}

	/* Blocks Collision */ // DO WE DELETE BLOCK AFTER USING IT
	void Game::blocksCollision()
	{
		for (int w = 0; w < blocks.size(); w++)
		{
			if (blocks[w]->beingDragged) { return; }

			if (player->collision(blocks[w]->object, blocks[w]->offset, blocks[w]->boundingRadius))
			{
				glm::vec3 dir = glm::normalize(blocks[w]->object->getAbsolutePosition() - player->body->getAbsolutePosition());
				camera_.Translate(-dir * 2.f * player->speed);
			}

			//check if it is dropped or is on the floor since each is handled differently
			//if on the floor move the dragonfly back if dropped kill it
			for (int i = 0; i < dragonFlies.size(); i++)
			{
				if (dragonFlies[i]->collision(blocks[w]->object, blocks[w]->offset, blocks[w]->boundingRadius))
				{
					if (blocks[w]->onFloor) { dragonFlies[i]->body->Translate(-dragonFlies[i]->getDirection() * 2.f * dragonFlies[i]->speed); }
					else
					{
						dragonFlies[i]->health = 0;
					}

				}
			}

			for (int j = 0; j < spiders.size(); j++)
			{
				if (spiders[j]->collision(blocks[w]->object, blocks[w]->offset, blocks[w]->boundingRadius))
				{
					if (blocks[w]->onFloor) { spiders[j]->body->Translate(-spiders[j]->getDirection() * 2.f * spiders[j]->speed); }
					else 
					{ 
						spiders[j]->health = 0; 
					}
				}
			}

			for (int k = 0; k < humans.size(); k++)
			{
				if (blocks[w]->collision(humans[k]->body, humans[k]->offset, humans[k]->boundingRadius) ||
					blocks[w]->collision(humans[k]->body, humans[k]->boundingRadius + humans[k]->offset, humans[k]->boundingRadius))
				{
					if (blocks[w]->onFloor) { humans[k]->body->Translate(-humans[k]->getDirection() * 2.f * humans[k]->speed); }
					else 
					{

						humans[k]->health = 0; 
					}
				}
			}
		}
	}
}
	
// namespace game
