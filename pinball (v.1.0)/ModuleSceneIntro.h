#pragma once
#include "Module.h"
#include "p2List.h"
#include "p2Point.h"
#include "Globals.h"

class PhysBody;

class ModuleSceneIntro : public Module
{
public:
	ModuleSceneIntro(Application* app, bool start_enabled = true);
	~ModuleSceneIntro();

	bool Start();
	update_status Update();
	bool CleanUp();
	void OnCollision(PhysBody* bodyA, PhysBody* bodyB);

public:
	p2List<PhysBody*> circles;
	p2List<PhysBody*> boxes;
	p2List<PhysBody*> ricks;
	p2List<PhysBody*> walls;

	PhysBody* ball;
	bool boost_ball;
	float boost_strength;

	iPoint spawn_position;

	PhysBody* sensor;
	bool sensed;

	PhysBody* wallFlickerL;
	PhysBody* wallFlickerR;
	PhysBody* flickerLeft;
	PhysBody* flickerRight;

	b2Vec2 wallFlickerL_p;
	b2Vec2 wallFlickerR_p;
	b2Vec2 flickerRight_p;
	b2Vec2 flickerLeft_p;

	int lifes;

	SDL_Texture* circle;
	SDL_Texture* box;
	SDL_Texture* rick;
	SDL_Texture* background;
	uint bonus_fx;
	p2Point<int> ray;
	bool ray_on;
};
