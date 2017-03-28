#ifndef _GAMEOBJECT__H__
#define _GAMEOJBECT__H__

#include "p2Defs.h"
#include "j1Render.h"
#include "p2List.h"

class Animator;
class PhysBody;
class b2Joint;
class Animation;

enum class pbody_type;
enum class fixture_type;

// -----------------------------------------
// -----------------------------------------

		    // GAME OBJECT //

// -----------------------------------------
// -----------------------------------------

class GameObject
{
public:

	// TO KEEP IN MIND:
	// - The CATEGORY and the MASK are defined on CollisionFilters.h.  
	// - pbody_type it's an enum that refeers to the PhysBodys.
	// - fixture_type it's an enum that refeers to a b2Fixture from a PhysBody.
	// - Both pbody and fixture _type are used to know what collides with what on the OnCollision method.
	// - Both pbody and fixture _type are defined on CollisionFilters.h. 
	GameObject(iPoint pos, int cat, int mask, pbody_type pb_type, float gravity_scale = 1.0f, float density = 1.0f, float friction = 1.0f);
	~GameObject();

	// Return the position in pixels of the GameObject
	iPoint GetPos();
	fPoint fGetPos();

	// Return the rotation in degrees of the GameObject 
	float GetRotation();

	// Inputs a position in pixels and changes the position of the GameObject
	void SetPos(fPoint pos);

	// Inputs rotation in degrees and rotates the GameObject
	void SetRotation(float angle);

	// Enable or disable the ability to rotate of the GameObject
	void SetFixedRotation(bool set);

	// Set pbody to dynamic
	void SetDynamic();

	// Set pbody to kinematic
	void SetKinematic();

	// Sets how much the body is affected by gravity
	void SetGravityScale(float gravity_scale);

	// Sets listener module
	void SetListener(j1Module* scene);

	// Changes cateogry and mask of the body
	void SetCatMask(int cat, int mask);

	// Adds an animation
	void AddAnimation(Animation* animation);

	// Loads all animations from an XML
	void LoadAnimationsFromXML(pugi::xml_document& doc);

	// Sets an animation
	void SetAnimation(const char* animation);

	// Returns the rect of the current animation of the animator
	SDL_Rect GetCurrentAnimationRect(float dt);

	// Adds a box shape to the current body
	void CreateCollision(iPoint offset, int width, int height, fixture_type type);

	// Adds a circle shape to the current body
	void CreateCollision(iPoint offset, int rad, fixture_type type);

	// Adds a box sensor shape to the current body
	void CreateCollisionSensor(iPoint offset, int width, int height, fixture_type type);

	// Adds a circle sensor shape to the current body
	void CreateCollisionSensor(iPoint offset, int rad, fixture_type type);

	// Set the texture to be using
	void SetTexture(SDL_Texture* texture);

	// Returns the texture
	SDL_Texture* GetTexture();

private:

public:
	PhysBody*      pbody = nullptr;
	Animator*	   animator = nullptr;

private:
	float		   gravity_scale = 0.0f;
	float		   density = 0.0f;
	float		   friction = 0.0f;
	float          restitution = 0.0f;
	int			   cat = 0;
	int		       mask = 0;

	SDL_Texture*   texture = nullptr;
};

#endif