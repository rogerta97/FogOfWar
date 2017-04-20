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

	void CreateMapCollisions(); 

	void OnColl(PhysBody* bodyA, PhysBody* bodyB, b2Fixture* fixtureA, b2Fixture* fixtureB);
	void OnCommand(std::list<std::string>& tokens);

	FogOfWar*			fog_of_war = nullptr;

	iPoint				prev_pos;
	iPoint				next_pos;

private: 

	Player*				player;
	Player*				simple_player;

	

	vector<PhysBody*>	map_collisions; 

};


#endif // !_MAINSCENE_H_