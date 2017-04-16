#include "SimpleEntity.h"
#include "GameObject.h"
#include "Scene.h"
#include "j1App.h"
#include "j1Input.h"
#include "CollisionFilters.h"
#include "j1Render.h"
#include "j1Scene.h"
#include "j1Map.h"
#include "Functions.h"
#include "j1Textures.h"
#include "p2Log.h"
#include "j1Viewports.h"


SimpleEntity::SimpleEntity()
{
}

SimpleEntity::~SimpleEntity()
{
}

bool SimpleEntity::LoadEntity()
{
	bool ret = true;

	player_go = new GameObject(iPoint(330, 750), App->cf->CATEGORY_PLAYER, App->cf->MASK_PLAYER, pbody_type::p_t_player, 0);

	player_go->CreateCollision(iPoint(-3, -5), 22, 35, fixture_type::f_t_null);
	player_go->SetListener((j1Module*)App->entity);
	player_go->SetFixedRotation(true);

	player_go->SetTexture(App->tex->LoadTexture("link_spritesheet.png"));

	pugi::xml_document doc;
	App->LoadXML("player.xml", doc);
	player_go->LoadAnimationsFromXML(doc);

	player_go->SetAnimation("idle_down");

	last_height = player_go->fGetPos().y;

	type = simple_entity; 

	return ret;
}

bool SimpleEntity::Start()
{
	bool ret = true;
	return ret;
}

bool SimpleEntity::PreUpdate()
{
	bool ret = true;
	return ret;
}

bool SimpleEntity::Update(float dt)
{
	bool ret = true;

	//App->view->CenterCamera(player_go->GetPos().x - App->view->win_w / 4, player_go->GetPos().y - App->view->win_h / 4);

	return ret;
}

bool SimpleEntity::Draw(float dt)
{
	bool ret = true;

	App->view->LayerBlit(2, player_go->GetTexture(), { player_go->GetPos().x - 23, player_go->GetPos().y - 35 }, player_go->GetCurrentAnimationRect(dt), 0, -1.0f, SDL_FLIP_NONE);

	return ret;
}

bool SimpleEntity::PostUpdate()
{
	bool ret = true;



	return ret;
}

bool SimpleEntity::CleanUp()
{
	bool ret = true;



	return ret;
}

void SimpleEntity::OnColl(PhysBody* bodyA, PhysBody * bodyB, b2Fixture * fixtureA, b2Fixture * fixtureB)
{

}

void SimpleEntity::SetGamePad(int id)
{
	gamepad_num = id - 1;
}

void SimpleEntity::SetCamera(int id)
{
	if (id > 0 && id < 5)
	{
		camera = id;
	}
}
