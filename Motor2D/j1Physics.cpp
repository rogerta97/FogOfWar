#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Input.h"
#include "j1Render.h"
#include "j1Physics.h"
#include "p2Point.h"
#include "math.h"
#include "Functions.h"
#include "j1Viewports.h"

#ifdef _DEBUG
#pragma comment( lib, "Box2D/libx86/Debug/Box2D.lib" )
#else
#pragma comment( lib, "Box2D/libx86/Release/Box2D.lib" )
#endif

#define GRAVITY_X 0.0f
#define GRAVITY_Y -5.0f

enum class fixture_type;

j1Physics::j1Physics()
{
	world = NULL;
	mouse_joint = NULL;
	debug = true;
}

// Destructor
j1Physics::~j1Physics()
{
}

bool j1Physics::Start()
{
	LOG("Start module physics");

	world = new b2World(b2Vec2(GRAVITY_X, -GRAVITY_Y));
	world->SetContactListener(this);

	// needed to create joints like mouse joint
	b2BodyDef bd;
	ground = world->CreateBody(&bd);

	return true;
}

// 
bool j1Physics::PreUpdate()
{
	world->Step(App->GetDT(), 6, 2);

	for(b2Contact* c = world->GetContactList(); c; c = c->GetNext())
	{
		if(c->GetFixtureA()->IsSensor() && c->IsTouching())
		{
			PhysBody* pb1 = (PhysBody*)c->GetFixtureA()->GetBody()->GetUserData();
			PhysBody* pb2 = (PhysBody*)c->GetFixtureB()->GetBody()->GetUserData();

			b2Fixture* fA = c->GetFixtureA();
			b2Fixture* fB = c->GetFixtureB();

			if(pb1 && pb2 && pb1->listener)
				pb1->listener->OnCollision(pb1, pb2, fA, fB);
		}
	}

	return true;
}

PhysBody* j1Physics::CreateCircle(int x, int y, int radius, float density, float gravity_scale, float rest, float friction, int cat, int mask)
{
	b2BodyDef body;
	body.type = b2_dynamicBody;
	body.position.Set(PIXEL_TO_METERS(x), PIXEL_TO_METERS(y));
	body.gravityScale = gravity_scale;

	b2Body* b = world->CreateBody(&body);

	b2CircleShape shape;
	shape.m_radius = PIXEL_TO_METERS(radius);
	b2FixtureDef fixture;
	fixture.shape = &shape;
	fixture.density = density;
	fixture.filter.categoryBits = cat;
	fixture.filter.maskBits = mask;
	fixture.restitution = rest;
	fixture.friction = friction;

	b->CreateFixture(&fixture);

	PhysBody* pbody = new PhysBody();
	pbody->body = b;
	b->SetUserData(pbody);
	pbody->width = pbody->height = radius;

	return pbody;
}

PhysBody * j1Physics::CreateStaticCircle(int x, int y, int radius, float density, float gravity_scale, float rest, int cat, int mask)
{
	b2BodyDef body;
	body.type = b2_staticBody;
	body.position.Set(PIXEL_TO_METERS(x), PIXEL_TO_METERS(y));
	body.gravityScale = gravity_scale;

	b2Body* b = world->CreateBody(&body);

	b2CircleShape shape;
	shape.m_radius = PIXEL_TO_METERS(radius);
	b2FixtureDef fixture;
	fixture.shape = &shape;
	fixture.density = density;
	fixture.filter.categoryBits = cat;
	fixture.filter.maskBits = mask;
	fixture.restitution = rest;

	b->CreateFixture(&fixture);

	PhysBody* pbody = new PhysBody();
	pbody->body = b;
	b->SetUserData(pbody);
	pbody->width = pbody->height = radius;

	return pbody;
}

PhysBody * j1Physics::CreateCircleSensor(int x, int y, int radius, float density, float gravity_scale, float rest, int cat, int mask)
{
	b2BodyDef body;
	body.type = b2_staticBody;
	body.position.Set(PIXEL_TO_METERS(x), PIXEL_TO_METERS(y));
	body.gravityScale = gravity_scale;

	b2Body* b = world->CreateBody(&body);

	b2CircleShape shape;
	shape.m_radius = PIXEL_TO_METERS(radius);
	b2FixtureDef fixture;
	fixture.shape = &shape;
	fixture.density = density;
	fixture.isSensor = true;
	fixture.filter.categoryBits = cat;
	fixture.filter.maskBits = mask;
	fixture.restitution = rest;

	b->CreateFixture(&fixture);

	PhysBody* pbody = new PhysBody();
	pbody->body = b;
	b->SetUserData(pbody);
	pbody->width = pbody->height = radius;

	return pbody;
}

void j1Physics::AddCircleToBody(PhysBody * pbody, int offset_x, int offset_y, int radius, fixture_type type, float density, float rest, float friction)
{
	b2CircleShape circle;
	circle.m_radius = PIXEL_TO_METERS(radius);
	circle.m_p = b2Vec2(PIXEL_TO_METERS(offset_x), PIXEL_TO_METERS(offset_y));

	b2FixtureDef fd;
	fd.shape = &circle;
	fd.density = density;
	fd.friction = friction;
	fd.isSensor = false;

	b2Fixture* fixture = pbody->body->CreateFixture(&fd);
	fixture->SetFixtureType(type);
}

void j1Physics::AddCircleSensorToBody(PhysBody * pbody, int offset_x, int offset_y, int radius, fixture_type type, float density, float rest, float friction)
{
	b2CircleShape circle;
	circle.m_radius = radius;
	circle.m_p = b2Vec2(offset_x, offset_y);

	b2FixtureDef fd;
	fd.shape = &circle;
	fd.density = density;
	fd.friction = friction;
	fd.isSensor = true;

	b2Fixture* fixture = pbody->body->CreateFixture(&fd);
	fixture->SetFixtureType(type);
}

PhysBody* j1Physics::CreateRectangle(int x, int y, int width, int height, float density, float gravity_scale, float rest, float friction, int cat, int mask, int angle)
{
	b2BodyDef body;
	body.type = b2_dynamicBody;
	body.position.Set(PIXEL_TO_METERS(x), PIXEL_TO_METERS(y));
	body.angle = DEGTORAD*angle;
	body.gravityScale = gravity_scale;

	b2Body* b = world->CreateBody(&body);
	b2PolygonShape box;
	box.SetAsBox(PIXEL_TO_METERS(width) * 0.5f, PIXEL_TO_METERS(height) * 0.5f);

	b2FixtureDef fixture;
	fixture.shape = &box;
	fixture.density = density;
	fixture.filter.categoryBits = cat;
	fixture.filter.maskBits = mask;
	fixture.restitution = rest;
	fixture.friction = friction;

	b->CreateFixture(&fixture);

	PhysBody* pbody = new PhysBody();
	pbody->body = b;
	b->SetUserData(pbody);
	pbody->width = width * 0.5f;
	pbody->height = height * 0.5f;

	return pbody;
}

PhysBody * j1Physics::CreateStaticRectangle(int x, int y, int width, int height, float density, float gravity_scale, float rest, int cat, int mask, int angle)
{
	b2BodyDef body;
	body.type = b2_staticBody;
	body.position.Set(PIXEL_TO_METERS(x), PIXEL_TO_METERS(y));
	body.angle = DEGTORAD*angle;
	body.gravityScale = gravity_scale;

	b2Body* b = world->CreateBody(&body);
	b2PolygonShape box;
	box.SetAsBox(PIXEL_TO_METERS(width) * 0.5f, PIXEL_TO_METERS(height) * 0.5f);

	b2FixtureDef fixture;
	fixture.shape = &box;
	fixture.density = density;
	fixture.filter.categoryBits = cat;
	fixture.filter.maskBits = mask;
	fixture.restitution = rest;

	b->CreateFixture(&fixture);

	PhysBody* pbody = new PhysBody();
	pbody->body = b;
	b->SetUserData(pbody);
	pbody->width = width * 0.5f;
	pbody->height = height * 0.5f;

	return pbody;
}

PhysBody * j1Physics::CreatePolygon(int x, int y, int* points, int size, float density, float gravity_scale, float rest, float friction, int cat, int mask, int angle)
{
	b2BodyDef body;
	body.type = b2_dynamicBody;
	body.position.Set(PIXEL_TO_METERS(x), PIXEL_TO_METERS(y));
	body.angle = DEGTORAD*angle;
	body.gravityScale = gravity_scale;

	b2Body* b = world->CreateBody(&body);
	b2PolygonShape box;
	b2Vec2* p = new b2Vec2[size / 2];

	for (uint i = 0; i < size / 2; ++i)
	{
		p[i].x = PIXEL_TO_METERS(points[i * 2 + 0]);
		p[i].y = PIXEL_TO_METERS(points[i * 2 + 1]);
	}
	box.Set(p, size / 2);

	b2FixtureDef fixture;
	fixture.shape = &box;
	fixture.density = density;
	fixture.filter.categoryBits = cat;
	fixture.filter.maskBits = mask;
	fixture.restitution = rest;
	fixture.friction = friction;

	b->CreateFixture(&fixture);

	PhysBody* pbody = new PhysBody();
	pbody->body = b;
	b->SetUserData(pbody);
	pbody->height = pbody->width = 0;

	return pbody;
}

PhysBody * j1Physics::CreateStaticPolygon(int x, int y, int* points, int size, float density, float gravity_scale, float rest, int cat, int mask, int angle)
{
	b2BodyDef body;
	body.type = b2_staticBody;
	body.position.Set(PIXEL_TO_METERS(x), PIXEL_TO_METERS(y));
	body.angle = DEGTORAD*angle;

	b2Body* b = world->CreateBody(&body);
	b2PolygonShape box;
	b2Vec2* p = new b2Vec2[size / 2];

	for (uint i = 0; i < size / 2; ++i)
	{
		p[i].x = PIXEL_TO_METERS(points[i * 2 + 0]);
		p[i].y = PIXEL_TO_METERS(points[i * 2 + 1]);
	}
	box.Set(p, size / 2);

	b2FixtureDef fixture;
	fixture.shape = &box;
	fixture.density = 1.0f;
	fixture.filter.categoryBits = cat;
	fixture.filter.maskBits = mask;
	fixture.restitution = rest;

	b->CreateFixture(&fixture);

	PhysBody* pbody = new PhysBody();
	pbody->body = b;
	b->SetUserData(pbody);
	pbody->height = pbody->width = 0;

	return pbody;
}

PhysBody * j1Physics::CreatePolygonSensor(int x, int y, int * points, int size, float density, float gravity_scale, float rest, int cat, int mask, int angle)
{
	b2BodyDef body;
	body.type = b2_staticBody;
	body.position.Set(PIXEL_TO_METERS(x), PIXEL_TO_METERS(y));
	body.angle = DEGTORAD*angle;

	b2Body* b = world->CreateBody(&body);
	b2PolygonShape box;
	b2Vec2* p = new b2Vec2[size / 2];

	for (uint i = 0; i < size / 2; ++i)
	{
		p[i].x = PIXEL_TO_METERS(points[i * 2 + 0]);
		p[i].y = PIXEL_TO_METERS(points[i * 2 + 1]);
	}
	box.Set(p, size / 2);

	b2FixtureDef fixture;
	fixture.shape = &box;
	fixture.density = 1.0f;
	fixture.filter.categoryBits = cat;
	fixture.filter.maskBits = mask;
	fixture.restitution = rest;
	fixture.isSensor = true;

	b->CreateFixture(&fixture);

	PhysBody* pbody = new PhysBody();
	pbody->body = b;
	b->SetUserData(pbody);
	pbody->height = pbody->width = 0;

	return pbody;
}

void j1Physics::AddPolygonToBody(PhysBody * pbody, int offset_x, int offset_y, int * points, int size, fixture_type type, float density, float gravity_scale, float rest, float friction)
{
	b2PolygonShape box;
	b2Vec2* p = new b2Vec2[size / 2];

	for (uint i = 0; i < size / 2; ++i)
	{
		p[i].x = PIXEL_TO_METERS(points[i * 2 + 0] + offset_x);
		p[i].y = PIXEL_TO_METERS(points[i * 2 + 1] + offset_y);
	}

	box.Set(p, size / 2);

	b2FixtureDef fd;
	fd.shape = &box;
	fd.density = density;
	fd.friction = friction;
	fd.isSensor = false;

	b2Fixture* fixture = pbody->body->CreateFixture(&fd);
	fixture->SetFixtureType(type);
}

PhysBody* j1Physics::CreateRectangleSensor(int x, int y, int width, int height, float density, float gravity_scale, float rest, int cat, int mask, int angle)
{
	b2BodyDef body;
	body.type = b2_staticBody;
	body.position.Set(PIXEL_TO_METERS(x), PIXEL_TO_METERS(y));
	body.angle = DEGTORAD*angle;
	body.gravityScale = gravity_scale;

	b2Body* b = world->CreateBody(&body);

	b2PolygonShape box;
	box.SetAsBox(PIXEL_TO_METERS(width) * 0.5f, PIXEL_TO_METERS(height) * 0.5f);

	b2FixtureDef fixture;
	fixture.shape = &box;
	fixture.density = density;
	fixture.isSensor = true;
	fixture.filter.categoryBits = cat;
	fixture.filter.maskBits = mask;
	fixture.restitution = rest;

	b->CreateFixture(&fixture);

	PhysBody* pbody = new PhysBody();
	pbody->body = b;
	b->SetUserData(pbody);
	pbody->width = width;
	pbody->height = height;

	return pbody;
}

void j1Physics::AddRectangleToBody(PhysBody * pbody, int offset_x, int offset_y, int width, int height, fixture_type type, float density, float rest, float friction)
{
	b2PolygonShape box;
	box.SetAsBox(PIXEL_TO_METERS(width) * 0.5f, PIXEL_TO_METERS(height) * 0.5f, b2Vec2(PIXEL_TO_METERS(offset_x), PIXEL_TO_METERS(offset_y)), 0);

	b2FixtureDef fd;
	fd.shape = &box;
	fd.density = density;
	fd.friction = friction;
	fd.isSensor = false;

	b2Fixture* fixture = pbody->body->CreateFixture(&fd);
	fixture->SetFixtureType(type);
}

void j1Physics::AddRectangleSensorToBody(PhysBody * pbody, int offset_x, int offset_y, int width, int height, fixture_type type, float density, float rest, float friction)
{
	b2PolygonShape box;
	box.SetAsBox(PIXEL_TO_METERS(width) * 0.5f, PIXEL_TO_METERS(height) * 0.5f, b2Vec2(PIXEL_TO_METERS(offset_x), PIXEL_TO_METERS(offset_y)), 0);

	b2FixtureDef fd;
	fd.shape = &box;
	fd.density = density;
	fd.friction = friction;
	fd.isSensor = true;

	b2Fixture* fixture = pbody->body->CreateFixture(&fd);
	fixture->SetFixtureType(type);
}

PhysBody* j1Physics::CreateChain(int x, int y, int* points, int size, float density, float gravity_scale, float rest, int cat, int mask, int angle)
{
	b2BodyDef body;
	body.type = b2_dynamicBody;
	body.position.Set(PIXEL_TO_METERS(x), PIXEL_TO_METERS(y));
	body.angle = DEGTORAD*angle;
	body.gravityScale = gravity_scale;

	b2Body* b = world->CreateBody(&body);

	b2ChainShape shape;
	b2Vec2* p = new b2Vec2[size / 2];

	for(uint i = 0; i < size / 2; ++i)
	{
		p[i].x = PIXEL_TO_METERS(points[i * 2 + 0]);
		p[i].y = PIXEL_TO_METERS(points[i * 2 + 1]);
	}

	shape.CreateLoop(p, size / 2);

	b2FixtureDef fixture;
	fixture.shape = &shape;
	fixture.density = density;
	fixture.filter.categoryBits = cat;
	fixture.filter.maskBits = mask;
	fixture.restitution = rest;

	b->CreateFixture(&fixture);

	delete p;

	PhysBody* pbody = new PhysBody();
	pbody->body = b;
	b->SetUserData(pbody);
	pbody->width = pbody->height = 0;

	return pbody;
}

PhysBody* j1Physics::CreateStaticChain(int x, int y, int* points, int size, float density, float gravity_scale, float rest, int cat, int mask, int angle)
{
	b2BodyDef body;
	body.type = b2_staticBody;
	body.position.Set(PIXEL_TO_METERS(x), PIXEL_TO_METERS(y));
	body.angle = DEGTORAD*angle;
	body.gravityScale = gravity_scale;

	b2Body* b = world->CreateBody(&body);

	b2ChainShape shape;
	b2Vec2* p = new b2Vec2[size / 2];

	for (uint i = 0; i < size / 2; ++i)
	{
		p[i].x = PIXEL_TO_METERS(points[i * 2 + 0]);
		p[i].y = PIXEL_TO_METERS(points[i * 2 + 1]);
	}

	shape.CreateLoop(p, size / 2);

	b2FixtureDef fixture;
	fixture.shape = &shape;
	fixture.density = density;
	fixture.filter.categoryBits = cat;
	fixture.filter.maskBits = mask;
	fixture.restitution = rest;

	b->CreateFixture(&fixture);

	delete p;

	PhysBody* pbody = new PhysBody();
	pbody->body = b;
	b->SetUserData(pbody);
	pbody->width = pbody->height = 0;

	return pbody;
}

PhysBody* j1Physics::CreateChainSensor(int x, int y, int * points, int size, float density, float gravity_scale, float rest, int cat, int mask, int angle)
{
	b2BodyDef body;
	body.type = b2_staticBody;
	body.position.Set(PIXEL_TO_METERS(x), PIXEL_TO_METERS(y));
	body.angle = DEGTORAD*angle;
	body.gravityScale = gravity_scale;

	b2Body* b = world->CreateBody(&body);

	b2ChainShape shape;
	b2Vec2* p = new b2Vec2[size / 2];

	for (uint i = 0; i < size / 2; ++i)
	{
		p[i].x = PIXEL_TO_METERS(points[i * 2 + 0]);
		p[i].y = PIXEL_TO_METERS(points[i * 2 + 1]);
	}

	shape.CreateLoop(p, size / 2);

	b2FixtureDef fixture;
	fixture.shape = &shape;
	fixture.density = density;
	fixture.filter.categoryBits = cat;
	fixture.filter.maskBits = mask;
	fixture.restitution = rest;
	fixture.isSensor = true;

	b->CreateFixture(&fixture);

	delete p;

	PhysBody* pbody = new PhysBody();
	pbody->body = b;
	b->SetUserData(pbody);
	pbody->width = pbody->height = 0;

	return pbody;
}

void j1Physics::AddChainBody(PhysBody* pbody, int x, int y, int * points, int size, fixture_type type, float density, float gravity_scale, float rest, float friction)
{
	b2ChainShape shape;
	b2Vec2* p = new b2Vec2[size / 2];

	for (uint i = 0; i < size / 2; ++i)
	{
		p[i].x = PIXEL_TO_METERS(points[i * 2 + 0]);
		p[i].y = PIXEL_TO_METERS(points[i * 2 + 1]);
	}

	shape.CreateLoop(p, size / 2);

	b2FixtureDef fd;
	fd.shape = &shape;
	fd.density = density;
	fd.friction = friction;
	fd.isSensor = false;

	b2Fixture* fixture = pbody->body->CreateFixture(&fd);
	fixture->SetFixtureType(type);
}

b2RevoluteJoint* j1Physics::CreateAtachJoint(PhysBody * body1, PhysBody * body2, int distance_between_x, int distance_between_y, float angle_between)
{
	b2RevoluteJointDef rev_joint;
	rev_joint.bodyA = body1->body;
	rev_joint.bodyB = body2->body;
	rev_joint.collideConnected = false;
	rev_joint.type = e_revoluteJoint;
	rev_joint.localAnchorA = b2Vec2(PIXEL_TO_METERS(distance_between_x), PIXEL_TO_METERS(distance_between_y));
	rev_joint.localAnchorB = b2Vec2(0, 0);
	rev_joint.enableLimit = true;

	rev_joint.lowerAngle = DEGTORAD*angle_between;
	rev_joint.upperAngle = DEGTORAD*angle_between;
	

	b2RevoluteJoint* rev = (b2RevoluteJoint*)world->CreateJoint(&rev_joint);

	return rev;
}


void j1Physics::CleanBodies()
{
	while (world->GetBodyList() != nullptr) 
	{
		world->DestroyBody(world->GetBodyList());
	}
}

void j1Physics::DeleteBody(PhysBody * object)
{
	world->DestroyBody(object->body);
}

b2RevoluteJoint* j1Physics::CreateRevoluteJoint(PhysBody * anchor, PhysBody * body, iPoint anchor_offset, iPoint body_offset, bool enable_limit,
													float max_angle, float min_angle, bool enable_motor, int motor_speed, int max_torque)
{
	b2RevoluteJointDef rev_joint;
	rev_joint.bodyA = anchor->body;
	rev_joint.bodyB = body->body;
	rev_joint.collideConnected = false;
	rev_joint.type = e_revoluteJoint;
	rev_joint.enableLimit = enable_limit;
	rev_joint.enableMotor = enable_motor;

	b2Vec2 anchor_center_diff(PIXEL_TO_METERS(anchor_offset.x), PIXEL_TO_METERS(anchor_offset.y));
	rev_joint.localAnchorA = anchor_center_diff;
	b2Vec2 body_center_diff(PIXEL_TO_METERS(body_offset.x), PIXEL_TO_METERS(body_offset.y));
	rev_joint.localAnchorB = body_center_diff;

	if (enable_limit) 
	{
		rev_joint.lowerAngle = DEGTORAD*min_angle;
		rev_joint.upperAngle = DEGTORAD*max_angle;
	}
	if (enable_motor) 
	{
		rev_joint.motorSpeed = motor_speed;
		rev_joint.maxMotorTorque = max_torque;
	}

	b2RevoluteJoint* rev = (b2RevoluteJoint*)world->CreateJoint(&rev_joint);

	return rev;
}

b2PrismaticJoint * j1Physics::CreatePrismaticJoint(PhysBody * anchor, PhysBody * body, iPoint anchor_offset, iPoint body_offset, bool enable_limit, float max_trans, float min_trans, bool enable_motor, int motor_speed, int max_force)
{
	b2PrismaticJointDef def;
	def.bodyA = anchor->body;
	def.bodyB = body->body;
	def.collideConnected = false;
	def.enableLimit = enable_limit;
	def.enableMotor = enable_motor;
	b2Vec2 localA(PIXEL_TO_METERS(anchor_offset.x), PIXEL_TO_METERS(anchor_offset.y));
	def.localAnchorA = localA;
	b2Vec2 localB(PIXEL_TO_METERS(body_offset.x), PIXEL_TO_METERS(body_offset.y));
	def.localAnchorB = localB;
	b2Vec2 localAxis(0,1);
	def.localAxisA = localAxis;
	if (enable_limit) {
		def.lowerTranslation = PIXEL_TO_METERS(min_trans);
		def.upperTranslation = PIXEL_TO_METERS(max_trans);
	}
	if (enable_motor) {
		def.motorSpeed = motor_speed;
		def.maxMotorForce = max_force;
	}

	return (b2PrismaticJoint*)world->CreateJoint(&def);
}

b2MotorJoint* j1Physics::CreateMotorJoint(b2Body* body, b2Vec2 target)
{
	target.x = PIXEL_TO_METERS(target.x);
	target.y = PIXEL_TO_METERS(target.y);

	b2MotorJointDef def;
	def.bodyA = ground;
	def.bodyB = body;
	def.maxTorque = 100.0f * body->GetMass();
	def.maxForce = 100.0f * body->GetMass();

	

	return (b2MotorJoint*)world->CreateJoint(&def);
}

path_joint * j1Physics::CreatePathJoint(b2Body * body, int * path, int path_size, int x_offset, int y_offset)
{
	path_joint* aux = new path_joint;

	b2Vec2* p = new b2Vec2[path_size / 2];

	for (uint i = 0; i < path_size / 2; ++i)
	{
		p[i].x = PIXEL_TO_METERS((path[i * 2 + 0] + x_offset));
		p[i].y = PIXEL_TO_METERS((path[i * 2 + 1] + y_offset));
	}

	aux->path = p;
	aux->points = path_size / 2;

	b2MouseJointDef def;
	def.bodyA = ground;
	def.bodyB = body;
	def.target = p[0];
	def.dampingRatio = 1.0f;
	def.frequencyHz = 2.0f;
	def.maxForce = 100.0f * body->GetMass();

	b2MouseJoint* mouse_joint = (b2MouseJoint*)world->CreateJoint(&def);
	aux->joint = mouse_joint;

	return aux;
}

void j1Physics::DeleteJoint(b2Joint* joint)
{
	world->DestroyJoint(joint);
}

// 
bool j1Physics::PostUpdate()
{
	debug = App->debug_mode;

	if(!debug)
		return true;

	// Bonus code: this will iterate all objects in the world and draw the circles
	// You need to provide your own macro to translate meters to pixels
	for(b2Body* b = world->GetBodyList(); b; b = b->GetNext())
	{
		for(b2Fixture* f = b->GetFixtureList(); f; f = f->GetNext())
		{
			switch(f->GetType())
			{
				// Draw circles ------------------------------------------------
				case b2Shape::e_circle:
				{
					b2CircleShape* shape = (b2CircleShape*)f->GetShape();
					b2Vec2 pos = f->GetBody()->GetPosition();

					float dist = DistanceFromTwoPoints((PIXELS_PER_METER * pos.x), (PIXELS_PER_METER * pos.y), PIXELS_PER_METER * (pos.x + shape->m_p.x), PIXELS_PER_METER * (pos.y + shape->m_p.y));

					float pos_x = 0;
					float pos_y = 0;

					float angle_between = AngleFromTwoPoints((PIXELS_PER_METER * pos.x), (PIXELS_PER_METER * pos.y), PIXELS_PER_METER * (pos.x + shape->m_p.x), PIXELS_PER_METER * (pos.y + shape->m_p.y));

					pos_x = (PIXELS_PER_METER * pos.x) + sin(-b->GetAngle() - ((angle_between - 90) * DEGTORAD)) * (dist) + App->render->camera.x;
					pos_y = (PIXELS_PER_METER * pos.y) + cos(-b->GetAngle() - ((angle_between - 90) * DEGTORAD)) * (dist) + App->render->camera.y;
					
					App->view->LayerDrawCircle(pos_x, pos_y, METERS_TO_PIXELS(shape->m_radius), 255, 255, 255);

				}
				break;

				// Draw polygons ------------------------------------------------
				case b2Shape::e_polygon:
				{
					b2PolygonShape* polygonShape = (b2PolygonShape*)f->GetShape();
					int32 count = polygonShape->GetVertexCount();
					b2Vec2 prev, v;

					for(int32 i = 0; i < count; ++i)
					{
						v = b->GetWorldPoint(polygonShape->GetVertex(i));
						if(i > 0)
							App->view->LayerDrawLine(METERS_TO_PIXELS(prev.x), METERS_TO_PIXELS(prev.y), METERS_TO_PIXELS(v.x), METERS_TO_PIXELS(v.y), 0, 0, 255);

						prev = v;
					}

					v = b->GetWorldPoint(polygonShape->GetVertex(0));
						App->view->LayerDrawLine(METERS_TO_PIXELS(prev.x), METERS_TO_PIXELS(prev.y), METERS_TO_PIXELS(v.x), METERS_TO_PIXELS(v.y), 0, 0, 255);
				}
				break;

				// Draw chains contour -------------------------------------------
				case b2Shape::e_chain:
				{
					b2ChainShape* shape = (b2ChainShape*)f->GetShape();
					b2Vec2 prev, v;

					for(int32 i = 0; i < shape->m_count; ++i)
					{
						v = b->GetWorldPoint(shape->m_vertices[i]);
						if(i > 0)
							App->view->LayerDrawLine(METERS_TO_PIXELS(prev.x), METERS_TO_PIXELS(prev.y), METERS_TO_PIXELS(v.x), METERS_TO_PIXELS(v.y), 100, 255, 100);
						prev = v;
					}

					v = b->GetWorldPoint(shape->m_vertices[0]);
					App->view->LayerDrawLine(METERS_TO_PIXELS(prev.x), METERS_TO_PIXELS(prev.y), METERS_TO_PIXELS(v.x), METERS_TO_PIXELS(v.y), 100, 255, 100);
				}
				break;

				// Draw a single segment(edge) ----------------------------------
				case b2Shape::e_edge:
				{
					b2EdgeShape* shape = (b2EdgeShape*)f->GetShape();
					b2Vec2 v1, v2;

					v1 = b->GetWorldPoint(shape->m_vertex0);
					v1 = b->GetWorldPoint(shape->m_vertex1);
					App->view->LayerDrawLine(METERS_TO_PIXELS(v1.x), METERS_TO_PIXELS(v1.y), METERS_TO_PIXELS(v2.x), METERS_TO_PIXELS(v2.y), 100, 100, 255);
				}
				break;
			}

			//If mouse button 1 is pressed ...
			if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_DOWN)
			{
				int mouse_x, mouse_y;
				App->input->GetMousePosition(mouse_x, mouse_y);
				mouse_x -= App->render->camera.x;
				mouse_y -= App->render->camera.y;

				b2Vec2 mouse(PIXEL_TO_METERS(mouse_x), PIXEL_TO_METERS(mouse_y));
				if (f->TestPoint(mouse)) 
				{
					selected = f->GetBody();
					break;
				}
			}
			// test if the current body contains mouse position
		}
	}

	// If a body was selected we will attach a mouse joint to it
	// so we can pull it around
	//If a body was selected, create a mouse joint
	// using mouse_joint class property
	if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_DOWN && selected != nullptr) 
	{
		int mouse_x, mouse_y;
		App->input->GetMousePosition(mouse_x, mouse_y);
		mouse_x -= App->render->camera.x;
		mouse_y -= App->render->camera.y;

		b2Vec2 mouse(PIXEL_TO_METERS(mouse_x), PIXEL_TO_METERS(mouse_y));

		b2MouseJointDef def;
		def.bodyA = ground;
		def.bodyB = selected;
		def.target = mouse;
		def.dampingRatio = 0.5f;
		def.frequencyHz = 2.0f;
		def.maxForce = 100.0f * selected->GetMass();

		mouse_joint = (b2MouseJoint*)world->CreateJoint(&def);

	}

	//If the player keeps pressing the mouse button, update
	// target position and draw a red line between both anchor points
	if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_REPEAT && selected != nullptr) {
		int mouse_x, mouse_y;
		App->input->GetMousePosition(mouse_x, mouse_y);
		mouse_x -= App->render->camera.x;
		mouse_y -= App->render->camera.y;

		b2Vec2 mouse(PIXEL_TO_METERS(mouse_x), PIXEL_TO_METERS(mouse_y));
		b2Vec2 mouse_pix(mouse_x, mouse_y);

		mouse_joint->SetTarget(mouse);
		App->view->LayerDrawLine(mouse_pix.x, mouse_pix.y, METERS_TO_PIXELS(selected->GetPosition().x), METERS_TO_PIXELS(selected->GetPosition().y), 255, 0, 0, 255);
	}
	//If the player releases the mouse button, destroy the joint
	if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_UP && selected != nullptr) {
		world->DestroyJoint(mouse_joint);
		mouse_joint = nullptr;
		selected = nullptr;
	}

	return true;
}


// Called before quitting
bool j1Physics::CleanUp()
{
	LOG("Destroying physics world");

	// Delete the whole physics world!
	delete world;

	return true;
}

void PhysBody::GetPosition(int& x, int &y) const
{
	b2Vec2 pos = body->GetPosition();
	x = METERS_TO_PIXELS(pos.x);
	y = METERS_TO_PIXELS(pos.y);
}

void PhysBody::fGetPosition(float& x, float& y) const
{
	b2Vec2 pos = body->GetPosition();
	x = PIXELS_PER_METER*(pos.x);
	y = PIXELS_PER_METER*(pos.y);
}

float PhysBody::GetRotation() const
{
	return RADTODEG * body->GetAngle();
}

bool PhysBody::Contains(int x, int y) const
{
	b2Vec2 p(PIXEL_TO_METERS(x), PIXEL_TO_METERS(y));

	const b2Fixture* fixture = body->GetFixtureList();

	while(fixture != NULL)
	{
		if(fixture->GetShape()->TestPoint(body->GetTransform(), p) == true)
			return true;
		fixture = fixture->GetNext();
	}

	return false;
}

int PhysBody::RayCast(int x1, int y1, int x2, int y2, float& normal_x, float& normal_y) const
{
	int ret = -1;

	b2RayCastInput input;
	b2RayCastOutput output;

	input.p1.Set(PIXEL_TO_METERS(x1), PIXEL_TO_METERS(y1));
	input.p2.Set(PIXEL_TO_METERS(x2), PIXEL_TO_METERS(y2));
	input.maxFraction = 1.0f;

	const b2Fixture* fixture = body->GetFixtureList();

	while(fixture != NULL)
	{
		if(fixture->GetShape()->RayCast(&output, input, body->GetTransform(), 0) == true)
		{
			// do we want the normal ?

			float fx = x2 - x1;
			float fy = y2 - y1;
			float dist = sqrtf((fx*fx) + (fy*fy));

			normal_x = output.normal.x;
			normal_y = output.normal.y;

			return output.fraction * dist;
		}
		fixture = fixture->GetNext();
	}

	return ret;
}

void j1Physics::BeginContact(b2Contact* contact)
{
	PhysBody* physA = (PhysBody*)contact->GetFixtureA()->GetBody()->GetUserData();
	PhysBody* physB = (PhysBody*)contact->GetFixtureB()->GetBody()->GetUserData();

	b2Fixture* fixtureA = contact->GetFixtureA();
	b2Fixture* fixtureB = contact->GetFixtureB();

	if(physA && physA->listener != NULL)
		physA->listener->OnCollision(physA, physB, fixtureA, fixtureB);

	if(physB && physB->listener != NULL)
		physB->listener->OnCollision(physB, physA, fixtureB, fixtureA);
}