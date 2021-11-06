#pragma once
#include "Module.h"
#include "p2List.h"
#include "p2Point.h"
#include "Globals.h"

class PhysBody;

class GameScene : public Module
{
public:
	GameScene(Application* app, bool start_enabled = true);
	~GameScene();

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

	PhysBody* wallFlickerL;
	PhysBody* wallFlickerR;
	PhysBody* flickerLeft;
	PhysBody* flickerRight;
	PhysBody* circleJointL;

	/*b2Vec2 wallFlickerL_p;
	b2Vec2 wallFlickerR_p;
	b2Vec2 flickerRight_p;
	b2Vec2 flickerLeft_p;*/

	int lifes;

	SDL_Texture* circle;
	SDL_Texture* box;
	SDL_Texture* rick;
	SDL_Texture* background;
	uint bonus_fx;
	p2Point<int> ray;
	bool ray_on;
};