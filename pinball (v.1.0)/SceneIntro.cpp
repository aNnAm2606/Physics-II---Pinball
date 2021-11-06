#include "SceneIntro.h"

#include "Application.h"
#include "ModuleTextures.h"
#include "ModuleRender.h"
#include "ModuleAudio.h"
#include "ModuleInput.h"
#include "FadeToBlack.h"

SceneIntro::SceneIntro(Application* app, bool start_enabled) : Module(app, start_enabled)
{
}

SceneIntro::~SceneIntro()
{
}

// Load assets
bool SceneIntro::Start()
{
	LOG("Loading background assets");


	App->renderer->camera.x = App->renderer->camera.y = 0;

	bool ret = true;
	// Load the textures example: Title = App->textures->Load("");
	startBackground = App->textures->Load("pinball/start screen.png");
	cat = App->textures->Load("pinball/catstogether.png");

	catAnimation.PushBack({ 0,0,500,667 });
	catAnimation.PushBack({ 500, 0, 500, 667 });
	catAnimation.PushBack({ 1000, 0, 500, 667 });
	catAnimation.PushBack({ 0, 667, 500, 667 });
	catAnimation.PushBack({ 500, 667, 500, 667 });
	catAnimation.PushBack({ 1000,667,500,667 });
	catAnimation.speed = 0.05f;
	catAnimation.loop = true;

	return ret;
}

update_status SceneIntro::Update()
{
	catAnimation.Update();
	// fade to black to game scene
	if (App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN)
	{
		App->fadeToBlack->Fade_To_Black(this, (Module*)App->gameScene, 90);
	}

	// Put images, example: App->renderer->Blit(Title, 0, 0, NULL);
	App->renderer->Blit(startBackground, 0, 0, NULL);

	SDL_Rect rect = catAnimation.GetCurrentFrame();
	App->renderer->Blit(cat, 0, 0, &rect);
	
	return UPDATE_CONTINUE;
}

bool SceneIntro::CleanUp()
{
	return true;
}