#include "Player2.h"
#include "GameObject.h"
#include "Scene.h"
#include "j1App.h"
#include "j1Input.h"
#include "CollisionFilters.h"
#include "j1Render.h"
#include "j1Scene.h"
#include "Functions.h"
#include "j1Textures.h"
#include "p2Log.h"
#include "j1Viewports.h"


Player2::Player2()
{
}

Player2::~Player2()
{
}

bool Player2::LoadEntity()
{
	bool ret = true;

	player_go = new GameObject(iPoint(400, 300), App->cf->CATEGORY_PLAYER, App->cf->MASK_PLAYER, pbody_type::p_t_player, 0);

	player_go->CreateCollision(iPoint(0, 0), 45, 70, fixture_type::f_t_null);
	player_go->SetListener((j1Module*)App->entity);
	player_go->SetFixedRotation(true);

	player_go->SetTexture(App->tex->LoadTexture("spritesheet.png"));

	pugi::xml_document doc;
	App->LoadXML("player2.xml", doc);
	player_go->LoadAnimationsFromXML(doc);

	player_go->SetAnimation("idle_down");

	last_height = player_go->fGetPos().y;

	return ret;
}

bool Player2::Start()
{
	bool ret = true;



	return ret;
}

bool Player2::PreUpdate()
{
	bool ret = true;



	return ret;
}

bool Player2::Update(float dt)
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

	App->view->CenterCamera(camera, player_go->GetPos().x + 23, player_go->GetPos().y + 35);

	return ret;
}

bool Player2::Draw(float dt)
{
	bool ret = true;

	if (player_go->animator->IsCurrentAnimation("run_lateral"))
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


	if (flip)
		App->view->LayerBlit(2, player_go->GetTexture(), { player_go->GetPos().x - 26, player_go->GetPos().y - 35 }, player_go->GetCurrentAnimationRect(dt), 0, -1.0f, SDL_FLIP_HORIZONTAL);
	else
		App->view->LayerBlit(2, player_go->GetTexture(), { player_go->GetPos().x - 23, player_go->GetPos().y - 35 }, player_go->GetCurrentAnimationRect(dt), 0, -1.0f, SDL_FLIP_NONE);

	return ret;
}

bool Player2::PostUpdate()
{
	bool ret = true;



	return ret;
}

bool Player2::CleanUp()
{
	bool ret = true;

	return ret;
}

void Player2::OnColl(PhysBody* bodyA, PhysBody * bodyB, b2Fixture * fixtureA, b2Fixture * fixtureB)
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

void Player2::SetGamePad(int id)
{
	gamepad_num = id-1;
}

void Player2::SetCamera(int id)
{
	if (id > 0 && id < 5)
	{
		camera = id;
	}
}
