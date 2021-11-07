#pragma once
#include "Module.h"
#include "Globals.h"
#include "Box2D/Box2D/Box2D.h"

#define GRAVITY_X 0.0f
#define GRAVITY_Y -7.0f

#define PIXELS_PER_METER 50.0f // if touched change METER_PER_PIXEL too
#define METER_PER_PIXEL 0.02f // this is 1 / PIXELS_PER_METER !
#define RAD_PER_DEG ((float)180/b2_pi)
#define DEG_PER_RAD ((float)b2_pi/180)

#define METERS_TO_PIXELS(m) ((int) floor(PIXELS_PER_METER * (m)))
#define PIXEL_TO_METERS(p)  ((float) METER_PER_PIXEL * (p))
#define RAD_TO_DEG(r) ((float) RAD_PER_DEG * (r))
#define DEG_TO_RAD(r) ((float) DEG_PER_RAD * (r))

// Small class to return to other modules to track position and rotation of physics bodies
class PhysBody
{
public:
	PhysBody();
	~PhysBody();

	void GetPosition(int& x, int &y) const;
	float GetRotation() const;
	bool Contains(int x, int y) const;
	int RayCast(int x1, int y1, int x2, int y2, float& normal_x, float& normal_y) const;


public:
	enum class Type {
		BOUNCER,
		SMALL_BIRD,
		BIG_BIRD,
		NONE
	};
	int width, height;
	b2Body* body;
	Module* listener;
	Type type;
};

// Module --------------------------------------
class ModulePhysics : public Module, public b2ContactListener // TODO
{
public:
	ModulePhysics(Application* app, bool start_enabled = true);
	~ModulePhysics();

	bool Start();
	update_status PreUpdate();
	update_status PostUpdate();
	bool CleanUp();
	bool CleanStage();

	PhysBody* CreateCircle(int x, int y, int radius, int dynamic);
	PhysBody* CreateRectangle(int x, int y, int width, int height, int dynamic);
	PhysBody* CreateRectangleSensor(int x, int y, int width, int height, int dynamic);
	PhysBody* CreateCircleSensor(int x, int y, int radius, int dynamic);
	PhysBody* CreateChain(int x, int y, int* points, int size, int dynamic);
	
	// joints functions
	b2RevoluteJoint* CreateRevoluteJoint(PhysBody* A, b2Vec2 anchorA, PhysBody* B, b2Vec2 anchorB, float angle, bool collideConnected, bool enableLimit);
	b2PrismaticJoint* CreatePrismaticJoint(PhysBody* A, b2Vec2 anchorA, PhysBody* B, b2Vec2 anchorB, b2Vec2 axys, float maxHeight, bool collideConnected, bool enableLimit);

	// b2ContactListener ---
	void BeginContact(b2Contact* contact);

private:

	bool debug;
	b2World* world;
	b2MouseJoint* mouse_joint;
	b2Body* mouse_body;
	b2Body* ground;
};