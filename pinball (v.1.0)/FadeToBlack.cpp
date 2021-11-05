#include "FadeToBlack.h"

#include "Application.h"
#include "ModuleRender.h"
#include "Globals.h"

#include "SDL/include/SDL_render.h"

FadeToBlack::FadeToBlack(Application* app, bool start_enabled) : Module(app, start_enabled)
{

}

FadeToBlack::~FadeToBlack()
{

}

bool FadeToBlack::Start()
{
	LOG("Preparing Fade Screen");

	// Enable blending mode for transparency
	SDL_SetRenderDrawBlendMode(App->renderer->renderer, SDL_BLENDMODE_BLEND);
	return true;
}

update_status FadeToBlack::Update()
{
	// Exit this function if we are not performing a fade
	if (currentStep == Fade_Step::NONE) return UPDATE_CONTINUE;

	if (currentStep == Fade_Step::TO_BLACK)
	{
		++frameCount;
		if (frameCount >= maxFadeFrames)
		{
			// L10: DONE 1: Enable / Disable the modules received when FadeToBlack() gets called
			moduleToDisable->Disable();
			moduleToEnable->Enable();

			currentStep = Fade_Step::FROM_BLACK;
		}
	}
	else
	{
		--frameCount;
		if (frameCount <= 0)
		{
			currentStep = Fade_Step::NONE;
		}
	}

	return UPDATE_CONTINUE;
}

update_status FadeToBlack::PostUpdate()
{
	// Exit this function if we are not performing a fade
	if (currentStep == Fade_Step::NONE) return UPDATE_CONTINUE;

	float fadeRatio = (float)frameCount / (float)maxFadeFrames;

	// Render the black square with alpha on the screen
	SDL_SetRenderDrawColor(App->renderer->renderer, 0, 0, 0, (Uint8)(fadeRatio * 255.0f));
	SDL_RenderFillRect(App->renderer->renderer, &screenRect);

	return UPDATE_CONTINUE;
}

bool FadeToBlack::Fade_To_Black(Module* moduleToDisable, Module* moduleToEnable, float frames)
{
	bool ret = false;

	// If we are already in a fade process, ignore this call
	if (currentStep == Fade_Step::NONE)
	{
		currentStep = Fade_Step::TO_BLACK;
		frameCount = 0;
		maxFadeFrames = frames;

		this->moduleToDisable = moduleToDisable;
		this->moduleToEnable = moduleToEnable;

		ret = true;
	}

	return ret;
}