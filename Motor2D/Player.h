#ifndef _PLAYER_
#define _PLAYER_

#include "Entity.h"

class GameObject;
class Player : public Entity
{
public:
	Player();
	~Player();

	// Load animations
	bool LoadEntity();

	// Start variables
	bool Start();

	// PreUpdate
	bool PreUpdate();

	// Update
	bool Update(float dt);

	// Draw and change animations
	bool Draw(float dt);

	// Post Update
	bool PostUpdate();

	// CleanUp
	bool CleanUp();

	// On Collision
	void OnColl(PhysBody* bodyA, PhysBody* bodyB, b2Fixture* fixtureA, b2Fixture* fixtureB);

	//Set Gamepad number to this player. id>0
	void SetGamePad(int id);

	//Set Camera to this player. 1<=id<=4
	void SetCamera(int id);

public:
	GameObject* player_go = nullptr;

	bool flip = false;
	bool on_ground = false;
	bool going_up = false;

	float last_height = 0.0f;

private:
	uint gamepad_num = 20;
	int camera = 1;

};

#endif