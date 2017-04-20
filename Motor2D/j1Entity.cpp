#include "j1Entity.h"
#include "Player.h"
#include "j1App.h"
#include "j1Scene.h"
#include "MainScene.h"
#include "GameObject.h"
#include "SimpleEntity.h"
#include "j1Map.h"
#include "j1Input.h"
#include "p2Log.h"

j1Entity::j1Entity()
{
	name = "entity";
}

j1Entity::~j1Entity()
{
}

bool j1Entity::Awake(pugi::xml_node &)
{
	bool ret = true;


	return ret;
}

bool j1Entity::Start()
{
	bool ret = true;

	return ret;
}

bool j1Entity::PreUpdate()
{
	bool ret = true;

	for(list<Entity*>::iterator it = entity_list.begin(); it != entity_list.end(); it++)
		ret = (*it)->PreUpdate();

	return ret;
}

bool j1Entity::Update(float dt)
{

	bool ret = true;

	if (App->input->GetKey(SDL_SCANCODE_TAB) == KEY_DOWN && App->input->GetKey(SDL_SCANCODE_W) == KEY_IDLE && App->input->GetKey(SDL_SCANCODE_D) == KEY_IDLE && App->input->GetKey(SDL_SCANCODE_S) == KEY_IDLE  && App->input->GetKey(SDL_SCANCODE_A) == KEY_IDLE)
		if(curr_entity->is_on_fow)
			ChangeFOWCharacter();


	for (list<Entity*>::iterator it = entity_list.begin(); it != entity_list.end(); it++)
	{
		// Not doing the update when the player have a fog of war but it's not the current, all other entities will update :)
		if ((*it)->is_on_fow == false)
			ret = (*it)->Update(dt);

		else if ((*it) == curr_entity)
			ret = (*it)->Update(dt);

		(*it)->Draw(dt);
	}

	return ret;
}

bool j1Entity::PostUpdate()
{
	bool ret = true;

	for (list<Entity*>::iterator it = entity_list.begin(); it != entity_list.end(); it++)
		ret = (*it)->PostUpdate();

	return ret;
}

bool j1Entity::CleanUp()
{
	bool ret = true;

	for (list<Entity*>::iterator it = entity_list.begin(); it != entity_list.end(); it++)
		ret = (*it)->CleanUp();

	return ret;
}


void j1Entity::OnCollision(PhysBody * bodyA, PhysBody * bodyB, b2Fixture * fixtureA, b2Fixture * fixtureB)
{
	for (list<Entity*>::iterator it = entity_list.begin(); it != entity_list.end(); it++)
		(*it)->OnColl(bodyA, bodyB, fixtureA, fixtureB);
}

Entity* j1Entity::CreateEntity(entity_name entity)
{
	Entity* ret = nullptr;

	switch (entity)
	{
	case player:
		ret = new Player();
		break;

	case simple_entity:
		ret = new SimpleEntity();
		break;

	default:
		break;
	}

	if (ret != nullptr)
	{
		ret->LoadEntity();
		ret->Start();
		ret->id = entity_list.size(); 
		entity_list.push_back(ret);
	}
	else
		LOG("Entity creation returned nullptr");

	return ret;
}


//vector<iPoint> j1Entity::GetEntitiesPos()
//{
//	return vector<iPoint>();
//}



void j1Entity::DeleteEntity(Entity* entity)
{
	entity->CleanUp();
	entity_list.remove(entity);
	RELEASE(entity);
}



void j1Entity::ChangeFOWCharacter()
{
	for (list<Entity*>::iterator it = entity_list.begin(); it != entity_list.end(); it++)
	{
		if ((*it)->id == curr_entity->id)
		{

			if ((*++it) == nullptr)
				it = entity_list.begin();

			for (it;; it++)
			{
				if ((*it)->is_on_fow)
				{
					curr_entity = *it;
					break;
				}
			}
		}
			
	}
}

void j1Entity::ManageCharacters()
{
}



