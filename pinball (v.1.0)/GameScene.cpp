#include "Globals.h"
#include "Application.h"
#include "Animation.h"
#include "ModuleRender.h"
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
}

GameScene::~GameScene()
{}

// Load assets
bool GameScene::Start()
{
	LOG("Loading Intro assets");
	bool ret = true;

	App->renderer->camera.x = App->renderer->camera.y = 0;

	//Loading sprites
	circle = App->textures->Load("pinball/wheel.png"); 
	box = App->textures->Load("pinball/crate.png");
	rick = App->textures->Load("pinball/rick_head.png");
	bonus_fx = App->audio->LoadFx("pinball/bonus.wav");
	background = App->textures->Load("pinball/background.png");

	spawn_position = {408, 500};

	ball = App->physics->CreateCircle(spawn_position.x, spawn_position.y, 10, 0);
	ball->listener = this;

	kicker_init_position = { 408, 520 };
	kicker_max_position = { 408,560 };

	kicker = App->physics->CreateRectangle(kicker_init_position.x, kicker_init_position.y, 42, 10, 1);

	boost_ball = true;
	boost_init_strength = 100.0f;
	boost_strength = boost_init_strength;
	boost_max_strength = 300.0f;

	boost_timer = 0.0f;
	boost_addspeed = 0.8f;
	boost_time = 1.0f;

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
	App->physics->CreateRectangle(199, 199, 56, 9, 1);
	App->physics->CreateRectangle(74, 242, 31, 9, 1);
	App->physics->CreateRectangle(313, 240, 31, 9, 1);
	App->physics->CreateRectangle(313, 240, 31, 9, 1);
	App->physics->CreateRectangle(74, 336, 19, 180, 1);
	App->physics->CreateRectangle(312, 332, 19, 180, 1);
	App->physics->CreateRectangle(312, 332, 19, 180, 1);
	App->physics->CreateRectangle(227, 403, 37, 7, 1);
	App->physics->CreateRectangle(103, 310, 26, 5, 1);
	App->physics->CreateRectangle(284, 306, 26, 5, 1);
	App->physics->CreateRectangle(134, 73, 26, 5, 1);
	App->physics->CreateRectangle(204, 72, 26, 5, 1);
	App->physics->CreateRectangle(268, 73, 26, 5, 1);
	
	App->physics->CreateCircleSensor(135, 62, 12, 1);
	App->physics->CreateCircleSensor(204, 62, 12, 1);
	App->physics->CreateCircleSensor(269, 62, 12, 1);
	App->physics->CreateCircleSensor(102, 299, 12, 1);
	App->physics->CreateCircleSensor(284, 295, 12, 1);

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
	App->physics->CreateChain(0, 0, books, 6, 1);

	int books2[6] = {
	300, 346,
	251, 415,
	300, 415
	};
	App->physics->CreateChain(0, 0, books2, 6, 1);

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
	App->physics->CreateChain(0, 0, scratcherL, 16, 1);
	wallFlickerL = App->physics->CreateChain(0, 0, scratcherL, 16, 1);


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
	App->physics->CreateChain(0, 0, scratcherR, 16, 1);
	wallFlickerR = App->physics->CreateChain(0, 0, scratcherR, 16, 1);

	// flickers
	int flickerL[16] = {
	120, 458,
	169, 454,
	176, 445,
	190, 455,
	190, 471,
	176, 483,
	166, 472,
	119, 470
	};
	
	flickerLeft = App->physics->CreateChain(0, 0, flickerL, 16, 0);

	int flickerR[16] = {
	265, 457,
	218, 456,
	209, 443,
	196, 456,
	196, 470,
	210, 483,
	221, 470,
	266, 470
	};
	flickerRight = App->physics->CreateChain(0, 0, flickerR, 14, 2);
	/*flickerRight_p.x = -22;
	flickerRight_p.y = -4;*/

	//Add kinematic fish
	
	//Create joint for flippers
	circleJointL = App->physics->CreateCircle(120, 463, 4,1);
	App->physics->CreateRevoluteJoint(flickerLeft, { 0,30 }, circleJointL, { 0, 0 }, 0, true, true);
	
	return ret;
}

// Load assets
bool GameScene::CleanUp()
{
	LOG("Unloading Intro scene");

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
	int ballY = METERS_TO_PIXELS(ball->body->GetPosition().y);
	if (ballY >= SCREEN_HEIGHT)
	{
		boost_ball = true;
		lifes--;
		ball->body->SetTransform(b2Vec2(PIXEL_TO_METERS(spawn_position.x), PIXEL_TO_METERS(spawn_position.y)), ball->body->GetAngle());
	}

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
		flickerLeft->body->ApplyForce({40,0}, {0,0}, true);
		}

	return UPDATE_CONTINUE;
}

void GameScene::OnCollision(PhysBody* bodyA, PhysBody* bodyB)
{
	int x, y;

	App->audio->PlayFx(bonus_fx);

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
