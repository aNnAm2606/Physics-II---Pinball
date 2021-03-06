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

	bool bounce_ball;
	iPoint bounce_dir;

	iPoint spawn_position;

	PhysBody* kicker;
	iPoint kicker_init_position;
	iPoint kicker_max_position;

	p2List<PhysBody*> fishes;
	int fish_count;
	float fish_radius;
	float fish_step;
	float fish_speed;
	iPoint fish_center;
	SDL_Rect fish_rect;

	int max_balls;

	PhysBody* sensor;
	bool sensed;

	PhysBody* wallFlickerL;
	PhysBody* wallFlickerR;
	PhysBody* flickerLeft;
	PhysBody* flickerRight;
	PhysBody* circleJointL;
	PhysBody* circleJointR;
	PhysBody* bigBird;
	PhysBody* whiteCat;
	PhysBody* fishR;
	PhysBody* fishL;

	PhysBody* movingRatL;
	PhysBody* movingRatR;
	PhysBody* movingCat;
	PhysBody* movingPlatform;
	iPoint movingPlatformLimits;
	iPoint movingCatLimits;
	iPoint movingRatRLimits;
	iPoint movingRatLLimits;
	float movingPlatformSpeed;
	float movingCatSpeed;
	float movingRatSpeed;
	bool movingPlatformRight;
	bool movingRatRRight;
	bool movingRatLRight;
	bool movingCatRight;

	float flickerSpeed;

	/*b2Vec2 wallFlickerL_p;
	b2Vec2 wallFlickerR_p;
	b2Vec2 flickerRight_p;
	b2Vec2 flickerLeft_p;*/

	int birdoHitCount;
	int prevScore;
	int bestScore;
	int score;
	int lifes;

	SDL_Texture* circle;
	SDL_Texture* box;
	SDL_Texture* rick;
	SDL_Texture* background;
	SDL_Texture* sprite;
	SDL_Texture* numberSprite;

	Animation bigBirdAnim;
	Animation blackCatAnim;
	Animation fishAnim;
	Animation fishAnimR;
	Animation whiteCatAnim;
	Animation ratRAnim;
	Animation ratLAnim;
	Animation ballAnim;
	Animation leftFlickerAnim;
	Animation rightFlickerAnim;

	uint music_fx;
	uint meow_fx;
	p2Point<int> ray;
	bool ray_on;
};
