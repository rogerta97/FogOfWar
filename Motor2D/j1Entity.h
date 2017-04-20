#ifndef __j1ENTITY_H__
#define __j1ENTITY_H__

#include "j1Module.h"
#include "p2Defs.h"
#include "p2List.h"
#include "p2Point.h"

class b2Fixture;
class PhysBody;

enum entity_name
{
	player,
	simple_entity, 
};

class Entity;
class Player;
class SimpleEntity;

class j1Entity : public j1Module
{
public:

	j1Entity();

	// Destructor
	virtual ~j1Entity();

	// Called before render is available
	bool Awake(pugi::xml_node&);

	// Called before the first frame
	bool Start();

	// Called before all Updates
	bool PreUpdate();

	// Called each loop iteration
	bool Update(float dt);

	// Called before all Updates
	bool PostUpdate();

	// Called before quitting
	bool CleanUp();

	void OnCollision(PhysBody* bodyA, PhysBody* bodyB, b2Fixture* fixtureA, b2Fixture* fixtureB);

	Entity* CreateEntity(entity_name entity);

	void DeleteEntity(Entity* entity);

	void SetPrevCurrPos(); 

	Entity* curr_entity = nullptr;

	bool is_on_fow = false; 

	iPoint prev_curr_pos = NULLPOINT;
	iPoint next_curr_pos = NULLPOINT;

	bool update_fow = false;

private:

	void ChangeFOWCharacter();
	void ManageCharacters();

private:

	list<Entity*> entity_list; 

};

#endif // __j1ENTITY_H__