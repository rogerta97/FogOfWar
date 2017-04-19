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
	
	// Creating the entities

	player = (Player*)App->entity->CreateEntity(entity_name::player);
	player->player_go->SetPos({ 1200,1000 });
	player->player_go->pbody->type = pbody_type::p_t_player;

	Player* player_2 = (Player*)App->entity->CreateEntity(entity_name::simple_entity);
	player_2->player_go->SetPos({ 1500,1000 }); 
	player_2->player_go->pbody->type = pbody_type::p_t_player;

	Player* player_3 = (Player*)App->entity->CreateEntity(entity_name::simple_entity);
	player_3->player_go->SetPos({ 1600,1000 });
	player_3->player_go->pbody->type = pbody_type::p_t_player;

	Player* player_4 = (Player*)App->entity->CreateEntity(entity_name::simple_entity);
	player_4->player_go->SetPos({ 1700,1000 });
	player_4->player_go->pbody->type = pbody_type::p_t_player;

	Player* player_5 = (Player*)App->entity->CreateEntity(entity_name::simple_entity);
	player_5->player_go->SetPos({ 1800,1000 });
	player_5->player_go->pbody->type = pbody_type::p_t_player;

	Player* player_6 = (Player*)App->entity->CreateEntity(entity_name::simple_entity);
	player_6->player_go->SetPos({ 1900,1000 });
	player_6->player_go->pbody->type = pbody_type::p_t_player;

	Player* player_7 = (Player*)App->entity->CreateEntity(entity_name::simple_entity);
	player_7->player_go->SetPos({ 2000,1000 });
	player_7->player_go->pbody->type = pbody_type::p_t_player;

	Player* player_8 = (Player*)App->entity->CreateEntity(entity_name::simple_entity);
	player_8->player_go->SetPos({ 2100,1000 });
	player_8->player_go->pbody->type = pbody_type::p_t_player;

	simple_player = (Player*)App->entity->CreateEntity(entity_name::simple_entity);

	simple_player->player_go->SetPos({ 600, 1200 });
	
	player->SetCamera(1);
	App->console->AddCommand("scene.set_player_camera", App->scene, 2, 2, "Set to player the camera number. Min_args: 2. Max_args: 2. Args: 1, 2, 3, 4");

	//Load Map
	App->map->Load("zelda_moba.tmx");

	CreateMapCollisions(); 

	iPoint joker = App->map->WorldToMap(player->player_go->GetPos().x, player->player_go->GetPos().y);
	joker.x += 3;

	// Start the fog of war

	fog_of_war = new FogOfWar(); 

	fog_of_war->AddPlayer(player); 

	fog_of_war->AddPlayer(player_2); 
	fog_of_war->AddPlayer(player_3);
	fog_of_war->AddPlayer(player_4);
	fog_of_war->AddPlayer(player_5);
	fog_of_war->AddPlayer(player_6);
	fog_of_war->AddPlayer(player_7);
	fog_of_war->AddPlayer(player_8);

	fog_of_war->AddPlayer(simple_player);

	fog_of_war->curr_character = player;

	fog_of_war->Start();

	// ----

	prev_pos = App->map->WorldToMap(fog_of_war->curr_character->player_go->GetPos().x, fog_of_war->curr_character->player_go->GetPos().y);

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
	
	next_pos = App->map->WorldToMap(fog_of_war->curr_character->player_go->GetPos().x, fog_of_war->curr_character->player_go->GetPos().y);

	if (App->input->GetKey(SDL_SCANCODE_TAB) == KEY_DOWN && App->input->GetKey(SDL_SCANCODE_W) == KEY_IDLE && App->input->GetKey(SDL_SCANCODE_D) == KEY_IDLE && App->input->GetKey(SDL_SCANCODE_S) == KEY_IDLE  && App->input->GetKey(SDL_SCANCODE_A) == KEY_IDLE)
	{
		fog_of_war->ChangeCharacter(prev_pos);
	}

	if (prev_pos != next_pos)
	{
		fog_of_war->Update(prev_pos); 
		prev_pos = next_pos;
	}
		
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

void MainScene::CreateMapCollisions()
{
	pugi::xml_document doc;
	App->LoadXML("MapCollisions.xml", doc);
	pugi::xml_node collisions = doc.child("collisions");

	for (pugi::xml_node chain = collisions.child("chain"); chain != NULL; chain = chain.next_sibling("chain"))
	{
		string points_string = chain.child_value();
		int num_points = chain.attribute("vertex").as_int();
		int* points = new int[num_points];

		std::list<string> points_list;
		Tokenize(points_string, ',', points_list);

		int i = 0;
		for (std::list<string>::iterator it = points_list.begin(); it != points_list.end(); it++)
		{
			if (i >= num_points)
				break;

			if (*it != "")
			{
				*(points + i) = stoi(*it);
				i++;
			}
		}
		PhysBody* b = App->physics->CreateStaticChain(0, 0, points, num_points, 1, 0, 0.0f, App->cf->CATEGORY_SCENERY, App->cf->MASK_SCENERY);
		b->type = pbody_type::p_t_player;

		map_collisions.push_back(b);
		RELEASE_ARRAY(points);
	}


}



