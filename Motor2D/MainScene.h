#ifndef _MAINSCENE_H_
#define _MAINSCENE_H_

#include "Scene.h"
#include "FogOfWar.h"
#include "j1Render.h"

class GameObject;
class b2Fixture;
class Player;

class MainScene : public Scene 
{
public:
	MainScene();
	~MainScene();

	bool Start();
	bool PreUpdate();
	bool Update(float dt);
	bool PostUpdate();
	bool CleanUp();

	void OnColl(PhysBody* bodyA, PhysBody* bodyB, b2Fixture* fixtureA, b2Fixture* fixtureB);
	void OnCommand(std::list<std::string>& tokens);

	GameObject* go = nullptr;

	fPoint pos;

	//Research
	Player*			player;
	Player*			simple_player;

	FogOfWar*		fog_of_war = nullptr; 

	iPoint	prev_pos;
	iPoint	next_pos;

};


#endif // !_MAINSCENE_H_