#include "FogOfWar.h"
#include "j1Scene.h"
#include "p2Log.h"
#include "Entity.h"
#include "j1Textures.h"
#include "MainScene.h"
#include "j1Map.h"
#include "GameObject.h"
#include "Player.h"

enum fow_id
{
	fow_null,
	dim_middle,
	dim_left,
	dim_right,
	dim_up,
	dim_down,
	dim_bottom_right,
	dim_bottom_left,
	dim_top_right,
	dim_top_left,
	dim_inner_top_left,
	dim_inner_top_right,
	dim_inner_bottom_left,
	dim_inner_bottom_right, 
	dim_closed_up,
	dim_closed_down, 
	dim_closed_right, 
	dim_closed_left,
	dim_clear, 
};

FogOfWar::FogOfWar()
{

	int size = App->map->data.width*App->map->data.height;
	data = new uint[size];
	
	memset(data, 0, size * sizeof(uint));

	fog_of_war_texture = App->tex->LoadTexture("maps/Fow_meta.png"); 

}

FogOfWar::~FogOfWar()
{
}

bool FogOfWar::AddPlayer(Player* new_entity)
{
	if (new_entity == nullptr)
		return false;

	players_on_fog_pos.push_back(App->map->WorldToMap(new_entity->player_go->GetPos().x, new_entity->player_go->GetPos().y));
	return true;
}

uint FogOfWar::Get(int x, int y)
{
	return data[(y*App->map->data.width) + x];
}

void FogOfWar::Start()
{
	GetEntitiesVisibleArea();
	RemoveJaggies();
}

void FogOfWar::Update()
{
	MoveFrontier();
	FillFrontier();
	RemoveJaggies(); 
}

void FogOfWar::GetEntitiesVisibleArea()
{
	vector<iPoint> seen_nodes;

	// We BFS the first time for knowing the tiles we must blit! 
	for (list<iPoint>::iterator it = players_on_fog_pos.begin(); it != players_on_fog_pos.end(); it++)
	{
		frontier = App->map->PropagateBFS(iPoint((*it).x, (*it).y), seen_nodes, FOW_RADIUM);

		// Passing each visible area to the total amount of visible tiles (if they are not yet)

		DeletePicks(); 

		for (list<iPoint>::iterator it = frontier.begin(); it != frontier.end(); it++)
			data[(*it).y * App->map->data.width + (*it).x] = dim_clear;	
	}
	 
}

uint FogOfWar::RemoveJaggies()
{
	for (list<iPoint>::iterator it = frontier.begin(); it != frontier.end(); it++)
	{

		if (Get((*it).x - 1, (*it).y) == dim_middle)
		{
			data[(*it).y*App->map->data.width + ((*it).x)] = dim_left;

			if (Get((*it).x, (*it).y - 1) == dim_middle)
			{
				data[((*it).y)*App->map->data.width + (*it).x] = dim_inner_top_left;

				if (Get(((*it).x + 1), (*it).y - 1) != dim_middle)
					data[((*it).y)*App->map->data.width + ((*it).x + 1)] = dim_top_left;
			}
				
			else if (Get((*it).x, (*it).y + 1) == dim_middle) 
			{
				data[((*it).y)*App->map->data.width + (*it).x] = dim_inner_bottom_left;

				if (Get(((*it).x + 1), (*it).y + 1) != dim_middle)
					data[((*it).y)*App->map->data.width + ((*it).x + 1)] = dim_bottom_left;
			}
				
		}
		else if (Get((*it).x + 1, (*it).y) == dim_middle)
		{
			data[(*it).y*App->map->data.width + ((*it).x)] = dim_right;

			if (Get((*it).x, (*it).y - 1) == dim_middle)
			{
				data[((*it).y)*App->map->data.width + (*it).x] = dim_inner_top_right;

				if (Get(((*it).x - 1), (*it).y - 1) != dim_middle)
					data[((*it).y)*App->map->data.width + ((*it).x - 1)] = dim_top_right;
			}

			else if (Get((*it).x, (*it).y + 1) == dim_middle)
			{
				data[((*it).y)*App->map->data.width + (*it).x] = dim_inner_bottom_right;

				if (Get(((*it).x - 1), (*it).y + 1) != dim_middle)
					data[((*it).y)*App->map->data.width + ((*it).x - 1)] = dim_bottom_right;
			}
		}
		else if (Get((*it).x, (*it).y + 1) == dim_middle)
			data[(*it).y*App->map->data.width + (*it).x] = dim_down; 
	
		else if (Get((*it).x, (*it).y - 1) == dim_middle)
			data[(*it).y*App->map->data.width + (*it).x] = dim_up;
	}

 return 0; 

}

void FogOfWar::MoveFrontier()
{
	int id = 0; 

	string direction = App->scene->main_scene->player->player_go->animator->GetCurrentAnimation()->GetName();

	for (list<iPoint>::iterator it = players_on_fog_pos.begin(); it != players_on_fog_pos.end(); it++)
	{
		if (direction == "run_down")
			MoveArea(id, fow_down);

		else if (direction == "run_up")
			MoveArea(id, fow_up);

		else if (direction == "run_lateral" && App->scene->main_scene->player->flip == true)
			MoveArea(id, fow_left);

		else if (direction == "run_lateral")
			MoveArea(id, fow_right);

		id++; 
	}
}

void FogOfWar::FillFrontier()
{
	iPoint ini_p = iPoint(players_on_fog_pos.begin()->x, players_on_fog_pos.begin()->y);
	ini_p.y -= radium + 1; ini_p.x -= radium + 1;

	iPoint end_p = iPoint(players_on_fog_pos.begin()->x, players_on_fog_pos.begin()->y);
	end_p.y += radium + 2; end_p.x += radium + 2;

	
		for (int y = ini_p.y; y < end_p.y; y++)
		{
			for (int x = ini_p.x; x < end_p.x; x++)
			{

				if (data[y*App->map->data.width + x] ==  dim_clear)
						while(data[y*App->map->data.width + x] != dim_clear)
						{
							data[y*App->map->data.width + x++] = dim_clear;						
						}

				
				else
					data[y*App->map->data.width + x] = dim_middle;

					data[y*App->map->data.width + x] = dim_clear;
			}
		
		}
}

void FogOfWar::MoveArea(int player_id, fow_directions direction)
{
	// When adding more players manage with player_id 

	switch(direction)
	{
	case fow_up: 
		for (list<iPoint>::iterator it = frontier.begin(); it != frontier.end(); it++)
			(*it).y -= 1;

		players_on_fog_pos.begin()->y -= 1; 

		break; 

	case fow_down:
		for (list<iPoint>::iterator it = frontier.begin(); it != frontier.end(); it++)
			(*it).y += 1;

		players_on_fog_pos.begin()->y += 1;


		break;

	case fow_left:
		for (list<iPoint>::iterator it = frontier.begin(); it != frontier.end(); it++)
			(*it).x -= 1;

		players_on_fog_pos.begin()->x -= 1;

		break;

	case fow_right:
		for (list<iPoint>::iterator it = frontier.begin(); it != frontier.end(); it++)
			(*it).x += 1;

		players_on_fog_pos.begin()->x += 1;

		break;
	}
}

SDL_Rect FogOfWar::GetRect(int fow_id)
{
	SDL_Rect rect_ret = { 0, 0, 32, 32 };

	if( fow_id > 0)
	{
		rect_ret.y = 0;
		rect_ret.x = 32 * (fow_id - 1); 	
	}

	return rect_ret;
}

void FogOfWar::DeletePicks()
{

	iPoint player_pos = players_on_fog_pos.front();

	for (list<iPoint>::iterator it = frontier.begin(); it != frontier.end(); it++)
	{
		if ((*it) == iPoint(player_pos.x, player_pos.y + radium))		
			(*it) = iPoint((*it).x, (*it).y - 1); 

		else if ((*it) == iPoint(player_pos.x, player_pos.y - radium))
			(*it) = iPoint((*it).x, (*it).y + 1);

		else if ((*it) == iPoint(player_pos.x + radium, player_pos.y))
			(*it) = iPoint((*it).x - 1, (*it).y);

		else if ((*it) == iPoint(player_pos.x - radium, player_pos.y))
			(*it) = iPoint((*it).x + 1, (*it).y);
	}

	bool done = false;
	int count = 0; 

	//for (vector<iPoint>::iterator it = current_visited_points.begin();; it++)
	//{
	//	if ((*it) == iPoint(player_pos.x, player_pos.y + radium) || (*it) == iPoint(player_pos.x, player_pos.y - radium) || (*it) == iPoint(player_pos.x + radium, player_pos.y) || (*it) == iPoint(player_pos.x - radium, player_pos.y))
	//	{
	//		current_visited_points.erase(it);
	//		count++; 
	//	}
	//		
	//	if (count == 4)
	//		break; 

	//}

}


