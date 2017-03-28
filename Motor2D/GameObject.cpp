#include "GameObject.h"
#include "j1Physics.h"
#include "Animation.h"
#include "p2Defs.h"
#include "j1Physics.h"
#include "Functions.h" 

GameObject::GameObject(iPoint _pos, int _cat, int _mask, pbody_type pb_type, float _gravity_scale,  float _density, float _friction) : gravity_scale(_gravity_scale), density(_density), friction(_friction), cat(_cat), mask(_mask)
{
	animator = new Animator();
	pbody = App->physics->CreateCircleSensor(_pos.x, _pos.y, 5, _density, _gravity_scale, 0, cat, mask);
	pbody->body->SetType(b2_dynamicBody);
	pbody->body->SetSleepingAllowed(false);
	pbody->type = pb_type;
}

GameObject::~GameObject()
{
	App->physics->DeleteBody(pbody);
}

iPoint GameObject::GetPos()
{
	iPoint ret = NULLPOINT;

	pbody->GetPosition(ret.x, ret.y);

	return ret;
}

fPoint GameObject::fGetPos()
{
	fPoint ret = NULLPOINT;

	pbody->fGetPosition(ret.x, ret.y);

	return ret;
}

float GameObject::GetRotation()
{
	return pbody->GetRotation();
}

void GameObject::SetPos(fPoint new_pos)
{
	pbody->body->SetTransform(b2Vec2(PIXEL_TO_METERS(new_pos.x), PIXEL_TO_METERS(new_pos.y)), pbody->body->GetAngle());
}

void GameObject::SetRotation(float angle)
{
	pbody->body->SetTransform(b2Vec2(pbody->body->GetPosition().x, pbody->body->GetPosition().y), DEGTORAD * angle);
}

void GameObject::SetFixedRotation(bool set)
{
	pbody->body->SetFixedRotation(set);
}

void GameObject::SetDynamic()
{
	pbody->body->SetType(b2_dynamicBody);
}

void GameObject::SetKinematic()
{
	pbody->body->SetType(b2_kinematicBody);
}

void GameObject::SetGravityScale(float _gravity_scale)
{
	pbody->body->SetGravityScale(_gravity_scale);
	gravity_scale = _gravity_scale;
}

void GameObject::SetListener(j1Module * scene)
{
	pbody->listener = scene;
}

void GameObject::SetCatMask(int cat, int mask)
{
	b2Filter data;
	data.categoryBits = cat;
	data.maskBits = mask;
	pbody->body->GetFixtureList()->SetFilterData(data);
}

void GameObject::AddAnimation(Animation* animation)
{
	animator->AddAnimation(animation);
}

void GameObject::LoadAnimationsFromXML(pugi::xml_document& doc)
{
	animator->LoadAnimationsFromXML(doc);
}

void GameObject::SetAnimation(const char * animation)
{
	animator->SetAnimation(animation);
}

SDL_Rect GameObject::GetCurrentAnimationRect(float dt)
{
	return animator->GetCurrentAnimation()->GetAnimationFrame(dt);
}

void GameObject::CreateCollision(iPoint offset, int width, int height, fixture_type type)
{
	App->physics->AddRectangleToBody(pbody, offset.x, offset.y, width, height, type, density, 0, friction);
}

void GameObject::CreateCollision(iPoint offset, int rad, fixture_type type)
{
	App->physics->AddCircleToBody(pbody, offset.x, offset.y, rad, type, density, 0, friction);
}

void GameObject::CreateCollisionSensor(iPoint offset, int width, int height, fixture_type type)
{
	App->physics->AddRectangleSensorToBody(pbody, offset.x, offset.y, width, height, type, density, 0, friction);
}

void GameObject::CreateCollisionSensor(iPoint offset, int rad, fixture_type type)
{
	App->physics->AddCircleSensorToBody(pbody, offset.x, offset.y, rad, type, density, 0, friction);
}

void GameObject::SetTexture(SDL_Texture * _texture)
{
	if (_texture != nullptr)
		texture = _texture;
}

SDL_Texture * GameObject::GetTexture()
{
	return texture;
}



