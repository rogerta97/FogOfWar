#pragma once
#include "j1Module.h"
#include "p2Point.h"
#include "Box2D/Box2D/Box2D.h"

#define PIXELS_PER_METER 50.0f // if touched change METER_PER_PIXEL too
#define METER_PER_PIXEL 0.02f // this is 1 / PIXELS_PER_METER !

#define METERS_TO_PIXELS(m) ((int) round(PIXELS_PER_METER * m))
#define PIXEL_TO_METERS(p)  ((float) METER_PER_PIXEL * p)

enum class pbody_type;
enum class fixture_type;

struct path_joint 
{
	b2MouseJoint* joint;
	b2Vec2*       path;
	int           points = 0;
	int			  cur_point = 0;
};

// Small class to return to other modules to track position and rotation of physics bodies
class PhysBody
{
public:
	PhysBody() : listener(NULL), body(NULL)
	{}

	void GetPosition(int& x, int &y) const;
	void fGetPosition(float& x, float &y) const;
	float GetRotation() const;
	bool Contains(int x, int y) const;
	int RayCast(int x1, int y1, int x2, int y2, float& normal_x, float& normal_y) const;

public:
	int        width = 0;
	int        height = 0;
	b2Body*    body = nullptr;
	j1Module*  listener = nullptr;
	pbody_type type = pbody_type::p_t_null;
};

// Module --------------------------------------
class j1Physics : public j1Module, public b2ContactListener // TODO
{
public:
	j1Physics();
	~j1Physics();

	bool Start();
	bool PreUpdate();
	bool PostUpdate();
	bool CleanUp();

	PhysBody* CreateCircle(int x, int y, int radius, float density = 1.0f, float gravity_scale = 1.0f, float rest = 0.0f, float friction = 1.0f, int cat = 1, int mask = 1);
	PhysBody* CreateStaticCircle(int x, int y, int radius, float density = 1.0f, float gravity_scale = 1.0f, float rest = 0.0f, int cat = 1, int mask = 1);
	PhysBody* CreateCircleSensor(int x, int y, int radius, float density = 1.0f, float gravity_scale = 1.0f, float rest = 0.0f, int cat = 1, int mask = 1);
	void AddCircleToBody(PhysBody* pbody, int offset_x, int offset_y, int radius, fixture_type type, float density = 1.0f, float rest = 0.0f, float friction = 1.0f);
	void AddCircleSensorToBody(PhysBody* pbody, int offset_x, int offset_y, int radius, fixture_type type, float density = 1.0f, float rest = 0.0f, float friction = 1.0f);

	PhysBody* CreateRectangle(int x, int y, int width, int height,  float density = 1.0f, float gravity_scale = 1.0f, float rest = 0.0f, float friction = 0.0f, int cat = 1, int mask = 1, int angle = 0);
	PhysBody* CreateStaticRectangle(int x, int y, int width, int height, float density = 1.0f, float gravity_scale = 1.0f, float rest = 0.0f, int cat = 1, int mask = 1, int angle = 0);
	PhysBody* CreateRectangleSensor(int x, int y, int width, int height, float density = 1.0f, float gravity_scale = 1.0f, float rest = 0.0f, int cat = 1, int mask = 1, int angle = 0);
	void AddRectangleToBody(PhysBody* pbody, int offset_x, int offset_y, int width, int height, fixture_type type, float density = 1.0f, float rest = 0.0f, float friction = 1.0f);
	void AddRectangleSensorToBody(PhysBody* pbody, int offset_x, int offset_y, int width, int height, fixture_type type, float density = 1.0f, float rest = 0.0f, float friction = 1.0f);

	PhysBody* CreatePolygon(int x, int y, int* points, int size, float density = 1.0f, float gravity_scale = 1.0f, float rest = 0.0f, float friction = 0.0f, int cat = 1, int mask = 1, int angle = 0);
	PhysBody* CreateStaticPolygon(int x, int y, int* points, int size, float density = 1.0f, float gravity_scale = 1.0f, float rest = 0.0f, int cat = 1, int mask = 1, int angle = 0);
	PhysBody* CreatePolygonSensor(int x, int y, int* points, int size, float density = 1.0f, float gravity_scale = 1.0f, float rest = 0.0f, int cat = 1, int mask = 1, int angle = 0);
	void AddPolygonToBody(PhysBody* pbody, int offset_x, int offset_y, int* points, int size, fixture_type type, float density = 1.0f, float gravity_scale = 1.0f, float rest = 0.0f, float friction = 0.0f);

	PhysBody* CreateChain(int x, int y, int* points, int size, float density = 1.0f, float gravity_scale = 1.0f, float rest = 0.0f, int cat = 1, int mask = 1, int angle = 0);
	PhysBody* CreateStaticChain(int x, int y, int* points, int size, float density = 1.0f, float gravity_scale = 1.0f, float rest = 0.0f, int cat = 1, int mask = 1, int angle = 0);
	PhysBody* CreateChainSensor(int x, int y, int* points, int size, float density = 1.0f, float gravity_scale = 1.0f, float rest = 0.0f, int cat = 1, int mask = 1, int angle = 0);
	void AddChainBody(PhysBody* pbody, int x, int y, int* points, int size, fixture_type type, float density = 1.0f, float gravity_scale = 1.0f, float rest = 0.0f, float friction = 1.0f);

	void CleanBodies();

	void DeleteBody(PhysBody* object);

	b2RevoluteJoint* CreateAtachJoint(PhysBody* body1, PhysBody* body2, int distance_between_x, int distance_between_y, float angle_between);
	b2RevoluteJoint* CreateRevoluteJoint(PhysBody* anchor, PhysBody* body, iPoint anchor_offset, iPoint body_offset, bool enable_limit, float max_angle, float min_angle, bool enable_motor, int motor_speed, int max_torque);
	b2PrismaticJoint* CreatePrismaticJoint(PhysBody* anchor,PhysBody* body, iPoint anchor_offset, iPoint body_offset, bool enable_limit, float max_trans, float min_trans, bool enable_motor, int motor_speed, int max_force);
	b2MotorJoint* CreateMotorJoint(b2Body* body, b2Vec2 target);
	path_joint* CreatePathJoint(b2Body* body, int *path, int path_size, int x_offset = 0, int y_offset = 0);

	void DeleteJoint(b2Joint* joint);

	void SetGround(b2Body* body) 
	{
		ground = body;
	}

	// b2ContactListener ---
	void BeginContact(b2Contact* contact);

private:

	bool			debug = false;
	b2World*	    world = nullptr;
	b2MouseJoint*   mouse_joint = nullptr;
	b2Body*			ground = nullptr;
	b2Body*			selected = nullptr;

};