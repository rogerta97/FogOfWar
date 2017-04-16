#include "MainScene.h"
#include "j1Scene.h"
#include "p2Log.h"
#include "j1Input.h"
#include "Functions.h"
#include "j1Physics.h"
#include "GameObject.h"
#include "j1App.h"
#include "j1Gui.h"
#include "j1Console.h"
#include "Parallax.h"
#include "j1Entity.h"
#include "CollisionFilters.h"
#include "Player.h"
#include "j1Map.h"


MainScene::MainScene()
{
}

MainScene::~MainScene()
{
}

bool MainScene::Start()
{
	bool ret = true;

	LOG("Start MainScene");
	


	player = (Player*)App->entity->CreateEntity(entity_name::player);
	Player* player_2 = (Player*)App->entity->CreateEntity(entity_name::player);

	simple_player = (Player*)App->entity->CreateEntity(entity_name::simple_entity);

	simple_player->player_go->SetPos({ 700, 1000 });
	

	player->SetCamera(1);
	App->console->AddCommand("scene.set_player_camera", App->scene, 2, 2, "Set to player the camera number. Min_args: 2. Max_args: 2. Args: 1, 2, 3, 4");

	//Load Map
	App->map->Load("zelda_moba2.tmx");

	fog_of_war = new FogOfWar(); 

	fog_of_war->AddPlayer(player); 
	fog_of_war->AddPlayer(player_2); 
	fog_of_war->AddPlayer(simple_player); 

	fog_of_war->Start();

	prev_pos = App->map->WorldToMap(player->player_go->GetPos().x, player->player_go->GetPos().y);

	return ret;
}

bool MainScene::PreUpdate()
{
	bool ret = true;

	return ret;
}

bool MainScene::Update(float dt)
{
	bool ret = true;
	
	if (prev_pos != App->map->WorldToMap(player->player_go->GetPos().x, player->player_go->GetPos().y))
		fog_of_war->Update();

	prev_pos = App->map->WorldToMap(player->player_go->GetPos().x, player->player_go->GetPos().y);

	App->map->Draw();


	return ret;
}

bool MainScene::PostUpdate()
{
	bool ret = true;


	return ret;
}

bool MainScene::CleanUp()
{
	bool ret = true;


	return ret;
}

void MainScene::OnColl(PhysBody * bodyA, PhysBody * bodyB, b2Fixture * fixtureA, b2Fixture * fixtureB)
{

}

void MainScene::OnCommand(std::list<std::string>& tokens)
{
	switch (tokens.size())
	{
	case 3:
		if (tokens.front() == "scene.set_player_gamepad") {
			int _player, gamepad;
			_player = atoi((++tokens.begin())->c_str());
			gamepad = atoi(tokens.back().c_str());
			if (_player > 0 && _player <= 4 && gamepad > 0 && gamepad <= 4)
			{
				switch (_player)
				{
				case 1:
					player->SetGamePad(gamepad);
					break;

				}
			}
			else
			{
				LOG("Invalid player or gamepad number");
			}
		}
		else if (tokens.front() == "scene.set_player_camera") {
			int _player, camera;
			_player = atoi((++tokens.begin())->c_str());
			camera = atoi(tokens.back().c_str());
			if (_player > 0 && _player <= 4 && camera > 0 && camera <= 4)
			{
				switch (_player)
				{
				case 1:
					player->SetCamera(camera);
					break;

				}
			}
			else
			{
				LOG("Invalid player or camera number");
			}
		}
		break;
	default:
		break;
	}
}

