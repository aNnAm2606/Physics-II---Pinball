#ifndef __SCENEINTRO_H__
#define __SCENEINTRO_H__

#include "Module.h"
#include "Animation.h"
#include "Globals.h"

struct SDL_Texture;

class SceneIntro : public Module
{
public:
	// Constructor
	SceneIntro(Application* app, bool start_enabled = true);

	// Destructor
	~SceneIntro();

	// Called when the module is activated
	// Loads the necessary textures for the map background
	bool Start() override;

	// Called at the middle of the application loop
	// Updates the scene's background animations
	update_status Update();

	bool CleanUp();
public:
	// The scene sprite sheet loaded into an SDL_Texture
	SDL_Texture* startBackground;
	SDL_Texture* cat;

	
};

#endif	// __SCENEINTRO_H__