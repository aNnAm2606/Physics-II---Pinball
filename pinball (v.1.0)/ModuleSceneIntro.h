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
	float boost_init_strength;
	float boost_strength;
	float boost_max_strength;

	float boost_timer;
	float boost_addspeed;
	float boost_time;

	iPoint spawn_position;

	PhysBody* kicker;
	iPoint kicker_init_position;
	iPoint kicker_max_position;

	PhysBody* sensor;
	bool sensed;

	int lifes;

	SDL_Texture* circle;
	SDL_Texture* box;
	SDL_Texture* rick;
	SDL_Texture* background;
	uint bonus_fx;
	p2Point<int> ray;
	bool ray_on;
};
