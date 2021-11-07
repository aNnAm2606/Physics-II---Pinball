#include "Globals.h"
#include "Application.h"
#include "Animation.h"
#include "ModuleRender.h"
#include "FadeToBlack.h"
#include "GameScene.h"
#include "ModuleInput.h"
#include "ModuleTextures.h"
#include "ModuleAudio.h"
#include "ModulePhysics.h"

#include <iostream>

GameScene::GameScene(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	circle = box = rick = NULL;
	ray_on = false;
	sensed = false;

	prevScore = 0;
	bestScore = 0;
}

GameScene::~GameScene()
{}

// Load assets
bool GameScene::Start()
{
	LOG("Loading Intro assets");
	bool ret = true;

	App->physics->Enable();

	App->renderer->camera.x = App->renderer->camera.y = 0;

	//Loading sprites
	circle = App->textures->Load("pinball/wheel.png"); 
	box = App->textures->Load("pinball/crate.png");
	rick = App->textures->Load("pinball/rick_head.png");
	meow_fx = App->audio->LoadFx("pinball/meow.wav");
	background = App->textures->Load("pinball/background.png");
	sprite = App->textures->Load("pinball/sprites.png");
	music_fx = App->audio->LoadFx("pinball/sound_track.wav");
	numberSprite = App->textures->Load("pinball/number_sprites.png");

	//background music
	App->audio->PlayFx(music_fx);

	//animations
	ballAnim.PushBack({ 193,17,23,22 });
	
	rightFlickerAnim.PushBack({ 91,314,58,53 });
	leftFlickerAnim.PushBack({ 27,315,58,53 });

	ratRAnim.PushBack({ 164,92,47,23 });
	ratRAnim.PushBack({ 211,89,47,23 });
	ratRAnim.PushBack({ 164,92,47,23 });
	ratRAnim.PushBack({ 204,63,47,23 });
	ratRAnim.PushBack({ 157,63,47,23 });
	ratRAnim.PushBack({ 204,63,47,23 });
	ratRAnim.speed = 0.07f;
	ratRAnim.loop = true;

	ratLAnim.PushBack({ 157,63,47,23 });
	ratLAnim.PushBack({ 204,63,47,23 });
	ratLAnim.PushBack({ 157,63,47,23 });
	ratLAnim.PushBack({ 211,89,47,23 });
	ratLAnim.PushBack({ 164,92,47,23 });
	ratLAnim.PushBack({ 211,89,47,23 });
	ratLAnim.speed = 0.07f;
	ratLAnim.loop = true;

	whiteCatAnim.PushBack({ 266,45,40,38 });
	whiteCatAnim.PushBack({ 312,46,40,38 });
	whiteCatAnim.PushBack({ 354,46,40,38 });
	whiteCatAnim.PushBack({ 399,45,40,38 });
	whiteCatAnim.PushBack({ 354,46,40,38 });
	whiteCatAnim.PushBack({ 312,46,40,38 });
	whiteCatAnim.speed = 0.02f;
	whiteCatAnim.loop = true;

	fishAnimR.PushBack({ 226,236,67,60 });
	fishAnimR.PushBack({ 297,236,67,60 });
	fishAnimR.PushBack({ 370,236,67,60 });
	fishAnimR.speed = 0.02f;
	fishAnimR.loop = true;

	fishAnim.PushBack({ 7,239,67,60 });
	fishAnim.PushBack({ 80,237,67,60 });
	fishAnim.PushBack({ 149,236,67,60 });
	fishAnim.speed = 0.02f;
	fishAnim.loop = true;

	bigBirdAnim.PushBack({ 0,33,55,51 });
	bigBirdAnim.PushBack({ 55,33,55,51 });
	bigBirdAnim.speed = 0.02f;
	bigBirdAnim.loop = true;

	blackCatAnim.PushBack({ 126,139,101,70 });
	blackCatAnim.PushBack({ 16,141,101,70 });
	blackCatAnim.PushBack({ 126,139,101,70 });
	blackCatAnim.PushBack({ 232,142,101,70 });
	blackCatAnim.PushBack({ 341,140,101,70 });
	blackCatAnim.speed = 0.05f;
	blackCatAnim.loop = true;

	spawn_position = {408, 500};

	birdoHitCount = 0;
	score = 0;
	lifes = 3;
	max_balls = 4;

	ball = App->physics->CreateCircle(spawn_position.x, spawn_position.y, 10, 0);

	ball->body->SetSleepingAllowed(false);
	ball->listener = this;

	bounce_ball = false;

	kicker_init_position = { 408, 520 };
	kicker_max_position = { 408,560 };

	kicker = App->physics->CreateRectangle(kicker_init_position.x, kicker_init_position.y, 42, 10, 1);

	boost_ball = true;
	boost_init_strength = 150.0f;
	boost_strength = boost_init_strength;
	boost_max_strength = 300.0f;

	boost_timer = 0.0f;
	boost_addspeed = 0.8f;
	boost_time = 1.0f;

	flickerSpeed = 50.0f;

	/*sensor = App->physics->CreateRectangleSensor(SCREEN_WIDTH / 2, SCREEN_HEIGHT, SCREEN_WIDTH, 50, false);*/

	// Draw walls
	int background[48] = {211, 667,
		211, 557,
		355, 437,
		356, 100,
		365, 89,
		386, 101,
		386, 580,
		431, 580,
		433, 106,
		422, 73,
		394, 46,
		339, 30,
		139, 29,
		96, 44,
		65, 68,
		41, 105,
		31, 130,
		32, 438,
		173, 557,
		173, 666,
		0, 666,
		0, 0,
		499, 0,
		499, 666
	};

	App->physics->CreateChain(0, 0, background, 48, 1);
	//Draw map colliders
	PhysBody* bouncer;
	bouncer = App->physics->CreateRectangle(199, 199, 56, 9, 1);
	bouncer->type = PhysBody::Type::BOUNCER;
	
	bouncer = App->physics->CreateRectangle(74, 242, 31, 9, 1);
	bouncer->type = PhysBody::Type::BOUNCER;
	bouncer = App->physics->CreateRectangle(313, 240, 31, 9, 1);
	bouncer->type = PhysBody::Type::BOUNCER;

	bouncer = App->physics->CreateRectangle(134, 73, 26, 5, 1);
	bouncer->type = PhysBody::Type::BOUNCER;
	bouncer = App->physics->CreateRectangle(204, 73, 26, 5, 1);
	bouncer->type = PhysBody::Type::BOUNCER;
	bouncer = App->physics->CreateRectangle(268, 73, 26, 5, 1);
	bouncer->type = PhysBody::Type::BOUNCER;

	bouncer = App->physics->CreateRectangle(180, 403, 37, 7, 1);
	bouncer->type = PhysBody::Type::BOUNCER;
	movingPlatform = bouncer;
	movingPlatformLimits = { 150, 227 };
	movingPlatformSpeed = 50.0f;
	movingPlatformRight = true;

	//moving Left rat
	bouncer = App->physics->CreateRectangle(70, 214, 40, 21, 1);
	bouncer->type = PhysBody::Type::BOUNCER;
	movingRatL = bouncer;
	movingRatLLimits = { 51, 133 };
	movingRatSpeed = 60.0f;
	movingRatLRight = true;

	//moving Right rat
	bouncer = App->physics->CreateRectangle(278, 214, 40, 21, 1);
	bouncer->type = PhysBody::Type::BOUNCER;
	movingRatR = bouncer;
	movingRatRLimits = { 260, 336 };
	movingRatSpeed = 60.0f;
	movingRatRRight = true;


	//moving Cat
	bouncer = App->physics->CreateRectangle(180, 384, 18, 33, 1);
	bouncer->type = PhysBody::Type::BOUNCER;
	movingCat = bouncer;
	movingCatLimits = { 150, 227 };
	movingCatSpeed = 50.0f;
	movingCatRight = true;

	bouncer = App->physics->CreateRectangle(103, 310, 26, 5, 1);
	bouncer->type = PhysBody::Type::BOUNCER;
	bouncer = App->physics->CreateRectangle(284, 306, 26, 5, 1);
	bouncer->type = PhysBody::Type::BOUNCER;

	// Walls
	App->physics->CreateRectangle(74, 336, 19, 180, 1);
	App->physics->CreateRectangle(312, 332, 19, 180, 1);
	App->physics->CreateRectangle(312, 332, 19, 180, 1);

	// small birds
	PhysBody* circle;
	circle = App->physics->CreateCircle(135, 62, 12, 1);
	circle->type = PhysBody::Type::SMALL_BIRD;

	circle = App->physics->CreateCircle(204, 62, 12, 1);
	circle->type = PhysBody::Type::SMALL_BIRD;

	circle = App->physics->CreateCircle(269, 62, 12, 1);
	circle->type = PhysBody::Type::SMALL_BIRD;

	circle = App->physics->CreateCircle(102, 299, 12, 1);
	circle->type = PhysBody::Type::SMALL_BIRD;

	circle = App->physics->CreateCircle(284, 295, 12, 1);
	circle->type = PhysBody::Type::SMALL_BIRD;

	//big bird
	bigBird = App->physics->CreateCircle(196, 178, 17, 1);
	bigBird->type = PhysBody::Type::BIG_BIRD;

	int leftFish[8] = {
	92, 240,
	159, 288,
	146, 305,
	80, 259
	};
	fishL = App->physics->CreateChain(0, 0, leftFish, 8, 1);

	int rightFish[8] = {
	293, 239,
	227, 284,
	236, 307,
	301, 258
	};
	fishR = App->physics->CreateChain(0, 0, rightFish, 8, 1);

	int milkbox[14] = {
	52, 150,
	65, 169,
	81, 169,
	104, 145,
	109, 132,
	116, 127,
	99, 111
	};
	App->physics->CreateChain(0,0, milkbox, 14, 1);

	int milkbox2[14] = {
	287, 112,
	270, 125,
	277, 130,
	282, 144,
	305, 169,
	321, 169,
	334, 154
	};
	App->physics->CreateChain(0, 0, milkbox2, 14, 1);

	int books[6] = {
		83, 415,
		133, 416,
		85, 348
	};
	PhysBody* pbooks = App->physics->CreateChain(0, 0, books, 6, 1);
	pbooks->type = PhysBody::Type::BOUNCER;

	int books2[6] = {
	300, 346,
	251, 415,
	300, 415
	};
	pbooks = App->physics->CreateChain(0, 0, books2, 6, 1);
	pbooks->type = PhysBody::Type::BOUNCER;

	int scratcherL[16] = {
	64, 425,
	110, 466,
	105, 469,
	109, 475,
	130, 455,
	126, 452,
	121, 456,
	82, 418
	};
	//App->physics->CreateChain(0, 0, scratcherL, 16, 1);
	wallFlickerL = App->physics->CreateChain(0, 0, scratcherL, 16, 1);
	wallFlickerL->type == PhysBody::Type::BOUNCER;


	int scratcherR[16] = {
	302, 416,
	262, 455,
	260, 450,
	255, 453,
	273, 473,
	278, 469,
	274, 466,
	320, 424
	};
	//App->physics->CreateChain(0, 0, scratcherR, 16, 1);
	wallFlickerR = App->physics->CreateChain(0, 0, scratcherR, 16, 1);
	wallFlickerR->type == PhysBody::Type::BOUNCER;

	// flickers
	flickerLeft = App->physics->CreateRectangle(152, 468, 57, 14, 0);
	flickerRight = App->physics->CreateRectangle(238, 468, 57, 14, 0);

	//Add kinematic fish
	
	//Create joint for flippers
	circleJointL = App->physics->CreateCircle(120, 463, 4, 1);
	circleJointR = App->physics->CreateCircle(270, 463, 4, 1);

	App->physics->CreateRevoluteJoint(flickerLeft, { -0.9f,0 }, circleJointL, { 0, 0 }, 45, true, true);
	App->physics->CreateRevoluteJoint(flickerRight, { 0.8f,0 }, circleJointR, { 0, 0 }, 45, true, true);

	// Rotatin fishes
	fish_radius = 55.0f;
	fish_center = { 196,178 };
	fish_rect = { 55, 5, 28, 20 };
	fish_count = 6;
	fish_step = 0;
	fish_speed = 50.0f;

	for (int i = 0; i < fish_count; i++) {
		fishes.add(App->physics->CreateRectangle(0, 0, fish_rect.w, fish_rect.h, 1));
	}
	
	return ret;
}

// Load assets
bool GameScene::CleanUp()
{
	LOG("Unloading Intro scene");

	App->physics->Disable();

	return true;
}

// Update: draw background
update_status GameScene::Update()
{
	App->renderer->Blit(background, 0, 0, NULL);

	//if(App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN)
	//{
	//	ray_on = !ray_on;
	//	ray.x = App->input->GetMouseX();
	//	ray.y = App->input->GetMouseY();
	//}

	//if(App->input->GetKey(SDL_SCANCODE_1) == KEY_DOWN)
	//{
	//	circles.add(App->physics->CreateCircle(App->input->GetMouseX(), App->input->GetMouseY(), 15, 0));
	//	circles.getLast()->data->listener = this;
	//}

	//if(App->input->GetKey(SDL_SCANCODE_2) == KEY_DOWN)
	//{
	//	boxes.add(App->physics->CreateRectangle(App->input->GetMouseX(), App->input->GetMouseY(), 100, 50, 0));
	//}

	//if(App->input->GetKey(SDL_SCANCODE_3) == KEY_DOWN)
	//{

	//	// Pivot 0, 0
	//	int rick_head[64] = {
	//		14, 36,
	//		42, 40,
	//		40, 0,
	//		75, 30,
	//		88, 4,
	//		94, 39,
	//		111, 36,
	//		104, 58,
	//		107, 62,
	//		117, 67,
	//		109, 73,
	//		110, 85,
	//		106, 91,
	//		109, 99,
	//		103, 104,
	//		100, 115,
	//		106, 121,
	//		103, 125,
	//		98, 126,
	//		95, 137,
	//		83, 147,
	//		67, 147,
	//		53, 140,
	//		46, 132,
	//		34, 136,
	//		38, 126,
	//		23, 123,
	//		30, 114,
	//		10, 102,
	//		29, 90,
	//		0, 75,
	//		30, 62
	//	};

	//	ricks.add(App->physics->CreateChain(App->input->GetMouseX(), App->input->GetMouseY(), rick_head, 64, 1));
	//}

	//animation 
	fishAnimR.Update();
	SDL_Rect rf = fishAnimR.GetCurrentFrame();
	App->renderer->Blit(sprite, 229, 245, &rf);


	fishAnim.Update();
	SDL_Rect cf = fishAnim.GetCurrentFrame();
	App->renderer->Blit(sprite, 89, 245, &cf);

	blackCatAnim.Update();
	SDL_Rect bc = blackCatAnim.GetCurrentFrame();
	App->renderer->Blit(sprite, 334, 528, &bc);

	bigBirdAnim.Update();
	SDL_Rect bb = bigBirdAnim.GetCurrentFrame();
	App->renderer->Blit(sprite, 170, 148, &bb);
	// Prepare for raycast ------------------------------------------------------
	
	iPoint mouse;
	mouse.x = App->input->GetMouseX();
	mouse.y = App->input->GetMouseY();
	int ray_hit = ray.DistanceTo(mouse);

	fVector normal(0.0f, 0.0f);

	// All draw functions ------------------------------------------------------
	p2List_item<PhysBody*>* c = circles.getFirst();

	while(c != NULL)
	{
		int x, y;
		c->data->GetPosition(x, y);
		if(c->data->Contains(App->input->GetMouseX(), App->input->GetMouseY()))
			App->renderer->Blit(circle, x, y, NULL, 1.0f, c->data->GetRotation());
		c = c->next;
	}

	c = boxes.getFirst();

	while(c != NULL)
	{
		int x, y;
		c->data->GetPosition(x, y);
		App->renderer->Blit(box, x, y, NULL, 1.0f, c->data->GetRotation());
		if(ray_on)
		{
			int hit = c->data->RayCast(ray.x, ray.y, mouse.x, mouse.y, normal.x, normal.y);
			if(hit >= 0)
				ray_hit = hit;
		}
		c = c->next;
	}

	c = ricks.getFirst();

	while(c != NULL)
	{
		int x, y;
		c->data->GetPosition(x, y);
		App->renderer->Blit(rick, x, y, NULL, 1.0f, c->data->GetRotation());
		c = c->next;
	}

	// ray -----------------
	if(ray_on == true)
	{
		fVector destination(mouse.x-ray.x, mouse.y-ray.y);
		destination.Normalize();
		destination *= ray_hit;

		App->renderer->DrawLine(ray.x, ray.y, ray.x + destination.x, ray.y + destination.y, 255, 255, 255);

		if(normal.x != 0.0f)
			App->renderer->DrawLine(ray.x + destination.x, ray.y + destination.y, ray.x + destination.x + normal.x * 25.0f, ray.y + destination.y + normal.y * 25.0f, 100, 255, 100);
	}

	// Ball update
	// If ball falls off the screen, respawn in position and lives--
	int ballX = METERS_TO_PIXELS(ball->body->GetPosition().x);
	int ballY = METERS_TO_PIXELS(ball->body->GetPosition().y);
	if (ballY >= SCREEN_HEIGHT)
	{
		boost_ball = true;
		lifes--;
		ball->body->SetTransform(b2Vec2(PIXEL_TO_METERS(spawn_position.x), PIXEL_TO_METERS(spawn_position.y)), 0);
		ball->body->SetLinearVelocity(b2Vec2_zero);
		ball->body->SetAngularVelocity(0);

		if (lifes <= 0) {
			if (score > bestScore) {
				bestScore = score;
			}

			prevScore = score;

			// GO END SCREEN
			//App->fadeToBlack->Fade_To_Black(this, (Module*)App->endSCreen, 90);
		}
	}

	if (bounce_ball) {
		ball->body->ApplyForceToCenter(b2Vec2(bounce_dir.x * 5.0f, bounce_dir.y * 5.0f), true);
		bounce_ball = false;
	}
	ballAnim.Update();
	SDL_Rect brect = ballAnim.GetCurrentFrame();
	App->renderer->Blit(sprite, ballX-10, ballY-10, &brect, 1.0f);

	// kicker logic
	if (App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT) {
		if (boost_ball) {
			boost_timer += App->deltaTime * boost_addspeed;
			boost_timer = CAP(boost_timer, 0.0f, boost_time);
			boost_strength = LERP(boost_init_strength, boost_max_strength, boost_timer);
			b2Vec2 kicker_pos = kicker->body->GetPosition();
			int lerp = LERP(kicker_init_position.y, kicker_max_position.y, boost_timer);
			kicker_pos.y = PIXEL_TO_METERS(lerp);
			kicker->body->SetTransform(kicker_pos, kicker->body->GetAngle());
		}
	}

	if (App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_UP)
	{
		if (boost_ball)
		{
			boost_ball = false;
			boost_strength = boost_init_strength;
			ball->body->ApplyForceToCenter(b2Vec2(0, -boost_strength), true);
			kicker->body->SetTransform(b2Vec2(PIXEL_TO_METERS(kicker_init_position.x), PIXEL_TO_METERS(kicker_init_position.y)), kicker->body->GetAngle());
		}
	}

	if (!boost_ball) {
		if (boost_timer > 0.0f) {
			boost_timer -= App->deltaTime * boost_addspeed * 8;
			boost_timer = CAP(boost_timer, 0.0f, boost_time);
			b2Vec2 kicker_pos = kicker->body->GetPosition();
			int lerp = LERP(kicker_init_position.y, kicker_max_position.y, boost_timer);
			kicker_pos.y = PIXEL_TO_METERS(lerp);
			kicker->body->SetTransform(kicker_pos, kicker->body->GetAngle());
		}
	}

	//FLICKER L
	if (App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT)
	{
		flickerLeft->body->ApplyForce({-5,0}, {0,0}, true);
	}

	int flickerLX = METERS_TO_PIXELS(flickerLeft->body->GetPosition().x);
	int flickerLY = METERS_TO_PIXELS(flickerLeft->body->GetPosition().y);
	double flickerLAngle = RADTODEG*(flickerLeft->body->GetAngle());

	leftFlickerAnim.Update();
	SDL_Rect flrect = leftFlickerAnim.GetCurrentFrame();
	App->renderer->Blit(sprite, flickerLX-25, flickerLY-20, &flrect, 2.0f, flickerLAngle);


	if (App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT)
	{
		flickerRight->body->ApplyForce({ 5,0 }, { 0,0 }, true);
	}


	int flickerRX = METERS_TO_PIXELS(flickerRight->body->GetPosition().x);
	int flickerRY = METERS_TO_PIXELS(flickerRight->body->GetPosition().y);
	double flickerRAngle = RADTODEG * (flickerRight->body->GetAngle());

	rightFlickerAnim.Update();
	SDL_Rect frrect = rightFlickerAnim.GetCurrentFrame();
	App->renderer->Blit(sprite, flickerRX - 30, flickerRY - 25, &frrect, 2.0f, flickerRAngle);

	// Moving platform
	b2Vec2 platform_pos = movingPlatform->body->GetPosition();
	platform_pos.x = METERS_TO_PIXELS(platform_pos.x);
	platform_pos.y = METERS_TO_PIXELS(platform_pos.y);

	if (movingPlatformRight) {
		platform_pos.x += movingPlatformSpeed * 2 * App->deltaTime;
		if (platform_pos.x >= movingPlatformLimits.y) {
			movingPlatformRight = false;
		}
	}
	else if (!movingPlatformRight) {
		platform_pos.x -= movingPlatformSpeed * App->deltaTime;

		if (platform_pos.x <= movingPlatformLimits.x) {
			movingPlatformRight = true;
		}
	}

	SDL_Rect rect = { 261, 334, 38, 8 };
	App->renderer->Blit(sprite, platform_pos.x - movingPlatform->width - 1, platform_pos.y - 1 - movingPlatform->height, &rect);

	platform_pos.x = PIXEL_TO_METERS(platform_pos.x);
	platform_pos.y = PIXEL_TO_METERS(platform_pos.y);

	movingPlatform->body->SetTransform(platform_pos, 0);

	// Moving cat
	b2Vec2 cat_pos = movingCat->body->GetPosition();
	cat_pos.x = METERS_TO_PIXELS(cat_pos.x);
	cat_pos.y = METERS_TO_PIXELS(cat_pos.y);

	if (movingCatRight) {
		cat_pos.x += movingCatSpeed * 2 * App->deltaTime;
		if (cat_pos.x >= movingCatLimits.y) {
			movingCatRight = false;
		}
	}
	else if (!movingCatRight) {
		cat_pos.x -= movingCatSpeed * App->deltaTime;

		if (cat_pos.x <= movingCatLimits.x) {
			movingCatRight = true;
		}
	}

	whiteCatAnim.Update();
	SDL_Rect wc = whiteCatAnim.GetCurrentFrame();
	App->renderer->Blit(sprite, cat_pos.x - movingCat->width - 1, cat_pos.y - 1 - movingCat->height, &wc);

	cat_pos.x = PIXEL_TO_METERS(cat_pos.x);
	cat_pos.y = PIXEL_TO_METERS(cat_pos.y);

	movingCat->body->SetTransform(cat_pos, 0);

	// Moving left rat
	b2Vec2 ratl_pos = movingRatL->body->GetPosition();
	ratl_pos.x = METERS_TO_PIXELS(ratl_pos.x);
	ratl_pos.y = METERS_TO_PIXELS(ratl_pos.y);

	if (movingRatLRight) {
		ratl_pos.x += movingRatSpeed * 2 * App->deltaTime;
		if (ratl_pos.x >= movingRatLLimits.y) {
			movingRatLRight = false;
		}
	}
	else if (!movingRatLRight) {
		ratl_pos.x -= movingRatSpeed * App->deltaTime;

		if (ratl_pos.x <= movingRatLLimits.x) {
			movingRatLRight = true;
		}
	}

	ratLAnim.Update();
	SDL_Rect lr = ratLAnim.GetCurrentFrame();
	App->renderer->Blit(sprite, ratl_pos.x - movingRatL->width - 1, ratl_pos.y - 1 - movingRatL->height, &lr);

	ratl_pos.x = PIXEL_TO_METERS(ratl_pos.x);
	ratl_pos.y = PIXEL_TO_METERS(ratl_pos.y);

	movingRatL->body->SetTransform(ratl_pos, 0);

	// Moving right rat
	b2Vec2 ratr_pos = movingRatR->body->GetPosition();
	ratr_pos.x = METERS_TO_PIXELS(ratr_pos.x);
	ratr_pos.y = METERS_TO_PIXELS(ratr_pos.y);

	if (movingRatRRight) {
		ratr_pos.x += movingRatSpeed * 2 * App->deltaTime;
		if (ratr_pos.x >= movingRatRLimits.y) {
			movingRatRRight = false;
		}
	}
	else if (!movingRatRRight) {
		ratr_pos.x -= movingRatSpeed * App->deltaTime;

		if (ratr_pos.x <= movingRatRLimits.x) {
			movingRatRRight = true;
		}
	}

	ratRAnim.Update();
	SDL_Rect rr = ratRAnim.GetCurrentFrame();
	App->renderer->Blit(sprite, ratr_pos.x - movingRatR->width - 1, ratr_pos.y - 1 - movingRatR->height, &rr);

	ratr_pos.x = PIXEL_TO_METERS(ratr_pos.x);
	ratr_pos.y = PIXEL_TO_METERS(ratr_pos.y);

	movingRatR->body->SetTransform(ratr_pos, 0);

	// Fishes rotation
	fish_step += fish_speed * App->deltaTime;
	float factor = (float)M_PI / 180.0f;
	float spacing = 360 / fish_count;

	p2List_item<PhysBody*>* fish = fishes.getFirst();
	for (int i = 0; i < fish_count; i++) {
		b2Vec2 fish_pos;

		fish_pos.x = fish_center.x + fish_radius * cos((fish_step + spacing * i) * factor);
		fish_pos.y = fish_center.y + fish_radius * sin((fish_step + spacing * i) * factor);

		App->renderer->Blit(sprite, fish_pos.x - fish->data->width, fish_pos.y - fish->data->height, &fish_rect);

		fish_pos.x = PIXEL_TO_METERS(fish_pos.x);
		fish_pos.y = PIXEL_TO_METERS(fish_pos.y);

		fish->data->body->SetTransform(fish_pos, 0);
		fish = fish->next;
	}

	// Draw score
	App->renderer->DrawNumber(prevScore, 20, 620, 6, numberSprite, 20, 20);
	App->renderer->DrawNumber(score, 200, 620, 6, numberSprite, 20, 20);
	App->renderer->DrawNumber(bestScore, 360, 620, 6, numberSprite, 20, 20);

	// Draw lifes
	App->renderer->DrawNumber(lifes, 440, 540, 1, numberSprite, 20, 20);

	// Instant Loss
	if (App->input->GetKey(SDL_SCANCODE_F2) == KEY_DOWN) App->fadeToBlack->Fade_To_Black(this, (Module*)App->gameOver, 120);

	return UPDATE_CONTINUE;
}

void GameScene::OnCollision(PhysBody* bodyA, PhysBody* bodyB)
{
	int x, y;

	App->audio->PlayFx(meow_fx);

	if (bodyB->type != PhysBody::Type::NONE) {
		bounce_ball = true;

		b2Vec2 dir = ball->body->GetLinearVelocity();

		bounce_dir.x = -dir.x;
		bounce_dir.y = -dir.y;
	}

	if (bodyB->type == PhysBody::Type::SMALL_BIRD) {
		birdoHitCount++;
		score += 5;
	}
	else if (bodyB->type == PhysBody::Type::BIG_BIRD) {
		birdoHitCount++;
		score += 10;
	}

	if (birdoHitCount == 3) {
		lifes++;
		birdoHitCount = 0;

		if (lifes > max_balls) {
			lifes = max_balls;
		}
	}

	//ball->body->ApplyForceToCenter(-ball->body->GetLinearVelocity(), true);

	/*
	if(bodyA)
	{
		bodyA->GetPosition(x, y);
		App->renderer->DrawCircle(x, y, 50, 100, 100, 100);
	}

	if(bodyB)
	{
		bodyB->GetPosition(x, y);
		App->renderer->DrawCircle(x, y, 50, 100, 100, 100);
	}*/
}
