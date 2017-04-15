#include "Player.h"
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


Player::Player()
{
}

Player::~Player()
{
}

bool Player::LoadEntity()
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

	type = player; 

	return ret;
}

bool Player::Start()
{
	bool ret = true;
	return ret;
}

bool Player::PreUpdate()
{
	bool ret = true;
	return ret;
}

bool Player::Update(float dt)
{
	bool ret = true;

	float speed = (200 * dt);

	if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT || App->input->GetControllerButton(gamepad_num, SDL_CONTROLLER_BUTTON_DPAD_LEFT) == KEY_REPEAT || App->input->GetControllerJoystickMove(gamepad_num, LEFTJOY_LEFT) > 12000)
	{
		player_go->SetPos({ player_go->fGetPos().x - speed, player_go->fGetPos().y });
		flip = true;
	}
	else if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT || App->input->GetControllerButton(gamepad_num, SDL_CONTROLLER_BUTTON_DPAD_RIGHT) == KEY_REPEAT || App->input->GetControllerJoystickMove(gamepad_num, LEFTJOY_RIGHT) > 12000)
	{
		player_go->SetPos({ player_go->fGetPos().x + speed, player_go->fGetPos().y });
		flip = false;
	}
	else if (App->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT || App->input->GetControllerButton(gamepad_num, SDL_CONTROLLER_BUTTON_DPAD_UP) == KEY_REPEAT || App->input->GetControllerJoystickMove(gamepad_num, LEFTJOY_UP) > 12000)
	{
		player_go->SetPos({ player_go->fGetPos().x, player_go->fGetPos().y - speed });
		flip = false;
	}
	else if (App->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT || App->input->GetControllerButton(gamepad_num, SDL_CONTROLLER_BUTTON_DPAD_DOWN) == KEY_REPEAT || App->input->GetControllerJoystickMove(gamepad_num, LEFTJOY_DOWN) > 12000)
	{
		player_go->SetPos({ player_go->fGetPos().x, player_go->fGetPos().y + speed });
		flip = false;
	}

	App->view->CenterCamera(player_go->GetPos().x - App->view->win_w/4, player_go->GetPos().y - App->view->win_h / 4);

	return ret;
}

bool Player::Draw(float dt)
{
	bool ret = true;

	if(player_go->animator->IsCurrentAnimation("run_lateral"))
		player_go->SetAnimation("idle_lateral");

	if (player_go->animator->IsCurrentAnimation("run_up"))
		player_go->SetAnimation("idle_up");

	if (player_go->animator->IsCurrentAnimation("run_down"))
		player_go->SetAnimation("idle_down");

	if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT || App->input->GetControllerButton(gamepad_num, SDL_CONTROLLER_BUTTON_DPAD_LEFT) == KEY_REPEAT || App->input->GetControllerJoystickMove(gamepad_num, LEFTJOY_LEFT) > 12000)
	{
		player_go->SetAnimation("run_lateral");
	}
	else if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT || App->input->GetControllerButton(gamepad_num, SDL_CONTROLLER_BUTTON_DPAD_RIGHT) == KEY_REPEAT || App->input->GetControllerJoystickMove(gamepad_num, LEFTJOY_RIGHT) > 12000)
	{
		player_go->SetAnimation("run_lateral");
	}
	else if (App->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT || App->input->GetControllerButton(gamepad_num, SDL_CONTROLLER_BUTTON_DPAD_UP) == KEY_REPEAT || App->input->GetControllerJoystickMove(gamepad_num, LEFTJOY_UP) > 12000)
	{
		player_go->SetAnimation("run_up");
	}
	else if (App->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT || App->input->GetControllerButton(gamepad_num, SDL_CONTROLLER_BUTTON_DPAD_DOWN) == KEY_REPEAT || App->input->GetControllerJoystickMove(gamepad_num, LEFTJOY_DOWN) > 12000)
	{
		player_go->SetAnimation("run_down");
	}
	
	
	if(flip)
		App->view->LayerBlit(2, player_go->GetTexture(), { player_go->GetPos().x - 26, player_go->GetPos().y - 35}, player_go->GetCurrentAnimationRect(dt), 0, -1.0f, SDL_FLIP_HORIZONTAL);
	else
		App->view->LayerBlit(2, player_go->GetTexture(), { player_go->GetPos().x - 23, player_go->GetPos().y - 35 }, player_go->GetCurrentAnimationRect(dt), 0, -1.0f, SDL_FLIP_NONE);

	return ret;
}

bool Player::PostUpdate()
{
	bool ret = true;



	return ret;
}

bool Player::CleanUp()
{
	bool ret = true;



	return ret;
}

void Player::OnColl(PhysBody* bodyA, PhysBody * bodyB, b2Fixture * fixtureA, b2Fixture * fixtureB)
{
	switch (bodyA->type)
	{
	case pbody_type::p_t_player:
		if (bodyB->type == pbody_type::p_t_world)
		{
		}
		break;
	
	}
}

void Player::SetGamePad(int id)
{
	gamepad_num = id-1;
}

void Player::SetCamera(int id)
{
	if (id > 0 && id < 5)
	{
		camera = id;
	}
}
