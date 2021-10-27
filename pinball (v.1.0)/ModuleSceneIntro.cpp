#include "Globals.h"
#include "Application.h"
#include "ModuleRender.h"
#include "ModuleSceneIntro.h"
#include "ModuleInput.h"
#include "ModuleTextures.h"
#include "ModuleAudio.h"
#include "ModulePhysics.h"

ModuleSceneIntro::ModuleSceneIntro(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	circle = box = rick = NULL;
	ray_on = false;
	sensed = false;
}

ModuleSceneIntro::~ModuleSceneIntro()
{}

// Load assets
bool ModuleSceneIntro::Start()
{
	LOG("Loading Intro assets");
	bool ret = true;

	App->renderer->camera.x = App->renderer->camera.y = 0;

	circle = App->textures->Load("pinball/wheel.png"); 
	box = App->textures->Load("pinball/crate.png");
	rick = App->textures->Load("pinball/rick_head.png");
	bonus_fx = App->audio->LoadFx("pinball/bonus.wav");
	background = App->textures->Load("pinball/background.png");

	/*sensor = App->physics->CreateRectangleSensor(SCREEN_WIDTH / 2, SCREEN_HEIGHT, SCREEN_WIDTH, 50, false);*/

	//Draw map colliders
	App->physics->CreateRectangle(199, 199, 56, 9, false);
	App->physics->CreateRectangle(74, 242, 31, 9, false);
	App->physics->CreateRectangle(313, 240, 31, 9, false);
	App->physics->CreateRectangle(313, 240, 31, 9, false);
	App->physics->CreateRectangle(74, 336, 19, 180, false);
	App->physics->CreateRectangle(312, 332, 19, 180, false);
	App->physics->CreateRectangle(312, 332, 19, 180, false);
	App->physics->CreateRectangle(227, 403, 37, 7, false);
	App->physics->CreateRectangle(103, 310, 26, 5, false);
	App->physics->CreateRectangle(284, 306, 26, 5, false);
	App->physics->CreateRectangle(134, 73, 26, 5, false);
	App->physics->CreateRectangle(204, 72, 26, 5, false);
	App->physics->CreateRectangle(268, 73, 26, 5, false);
	
	App->physics->CreateCircle(135, 62, 12, false);
	App->physics->CreateCircle(204, 62, 12, false);
	App->physics->CreateCircle(269, 62, 12, false);
	App->physics->CreateCircle(102, 299, 12, false);
	App->physics->CreateCircle(284, 295, 12, false);


	return ret;
}

// Load assets
bool ModuleSceneIntro::CleanUp()
{
	LOG("Unloading Intro scene");

	return true;
}

// Update: draw background
update_status ModuleSceneIntro::Update()
{
	App->renderer->Blit(background, 0, 0, NULL);
	
	//wall bounds coordinates
	int background[88] = {
	434, 561,
	433, 106,
	422, 70,
	377, 35,
	336, 30,
	167, 29,
	130, 34,
	98, 43,
	73, 60,
	51, 86,
	40, 106,
	31, 130,
	32, 438,
	169, 555,
	169, 567,
	24, 444,
	22, 124,
	34, 95,
	46, 75,
	71, 47,
	106, 29,
	140, 19,
	166, 18,
	346, 20,
	385, 25,
	410, 42,
	432, 62,
	442, 100,
	443, 579,
	377, 579,
	376, 107,
	369, 93,
	361, 93,
	360, 444,
	216, 569,
	213, 556,
	354, 439,
	356, 101,
	358, 89,
	369, 88,
	378, 92,
	388, 102,
	392, 117,
	392, 561
	};

	App->physics->CreateChain(0, 0, background, 88, false);

	if(App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN)
	{
		ray_on = !ray_on;
		ray.x = App->input->GetMouseX();
		ray.y = App->input->GetMouseY();
	}

	if(App->input->GetKey(SDL_SCANCODE_1) == KEY_DOWN)
	{
		circles.add(App->physics->CreateCircle(App->input->GetMouseX(), App->input->GetMouseY(), 15, true));
		circles.getLast()->data->listener = this;
	}

	if(App->input->GetKey(SDL_SCANCODE_2) == KEY_DOWN)
	{
		boxes.add(App->physics->CreateRectangle(App->input->GetMouseX(), App->input->GetMouseY(), 100, 50, true));
	}

	if(App->input->GetKey(SDL_SCANCODE_3) == KEY_DOWN)
	{

		// Pivot 0, 0
		int rick_head[64] = {
			14, 36,
			42, 40,
			40, 0,
			75, 30,
			88, 4,
			94, 39,
			111, 36,
			104, 58,
			107, 62,
			117, 67,
			109, 73,
			110, 85,
			106, 91,
			109, 99,
			103, 104,
			100, 115,
			106, 121,
			103, 125,
			98, 126,
			95, 137,
			83, 147,
			67, 147,
			53, 140,
			46, 132,
			34, 136,
			38, 126,
			23, 123,
			30, 114,
			10, 102,
			29, 90,
			0, 75,
			30, 62
		};

		ricks.add(App->physics->CreateChain(App->input->GetMouseX(), App->input->GetMouseY(), rick_head, 64, true));
	}

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

	return UPDATE_CONTINUE;
}

void ModuleSceneIntro::OnCollision(PhysBody* bodyA, PhysBody* bodyB)
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
