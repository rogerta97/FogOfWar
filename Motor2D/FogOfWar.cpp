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

	players_on_fog.push_back(new_entity);
	return true;
}

uint FogOfWar::Get(int x, int y)
{
	return data[(y*App->map->data.width) + x];
}

void FogOfWar::Start()
{
	frontier = GetEntitiesVisibleArea(radium);
	RemoveJaggies(frontier);
}

void FogOfWar::Update()
{
	UpdateEntitiesVisibleArea(); 
	UpdateMatrix(); 
	RemoveJaggies(frontier); 
}

list<iPoint> FogOfWar::GetEntitiesVisibleArea(int limit)
{

	list<iPoint> frontier; 

	vector<iPoint> seen_nodes;
	bool repeated = false;
	bool opaque = false;

	// We BFS the first time for knowing the tiles we must blit! 
	for (vector<Player*>::iterator it = players_on_fog.begin(); it != players_on_fog.end(); it++)
	{
		frontier = App->map->PropagateBFS(App->map->WorldToMap((*it)->player_go->GetPos().x, (*it)->player_go->GetPos().y), seen_nodes, limit);

		// Passing each visible area to the total amount of visible tiles (if they are not yet)

		for (vector<iPoint>::iterator it = seen_nodes.begin(); it != seen_nodes.end(); it++)
			current_visited_points.push_back(*it);	
	}

	// Update the data matrix in order to draw 
	for (vector<iPoint>::iterator it = current_visited_points.begin(); it != current_visited_points.end(); it++)
		data[it->y*App->map->data.width + it->x] = dim_clear;
	
	return frontier; 
	// ----
}

uint FogOfWar::RemoveJaggies(list<iPoint> frontier)
{
	vector<iPoint> corners; 

	for(list<iPoint>::iterator it = frontier.begin(); it != frontier.end(); it++)
	{

		if (Get((*it).x - 1, (*it).y) == dim_middle && Get((*it).x, (*it).y - 1) == dim_middle) 
		{
			if (Get((*it).x + 1, (*it).y) == dim_middle) 
			{
			
				data[(*it).y*App->map->data.width + (*it).x] = dim_middle; 
				data[((*it).y + 1)*App->map->data.width + ((*it).x)] = dim_up;
				continue; 
			}

	
			data[(*it).y*App->map->data.width + (*it).x] = dim_inner_top_left;
			data[((*it).y + 1)*App->map->data.width + (*it).x] = dim_top_left;
			
	

		}

		if (Get((*it).x + 1, (*it).y) == dim_middle && Get((*it).x, (*it).y - 1) == dim_middle)
		{
			if (Get((*it).x , (*it).y + 1) == dim_middle)
			{
				corners.push_back(*it);
			/*	data[(*it).y*App->map->data.width + (*it).x] = dim_middle; 
				data[(*it).y*App->map->data.width + ((*it).x - 1)] = dim_right;*/
				continue;
			}
			
				data[(*it).y*App->map->data.width + (*it).x] = dim_inner_top_right; 
				data[((*it).y + 1)*App->map->data.width + (*it).x] = dim_top_right;

			
			
		}

	
		if (Get((*it).x - 1, (*it).y) == dim_middle && Get((*it).x, (*it).y + 1) == dim_middle)
		{
			if (Get((*it).x, (*it).y - 1) == dim_middle)
			{
				corners.push_back(*it);
				/*data[(*it).y*App->map->data.width + (*it).x] = dim_middle; 
				data[(*it).y*App->map->data.width + ((*it).x + 1)] = dim_left;*/
				continue;
			}

		
				data[(*it).y*App->map->data.width + (*it).x] = dim_inner_bottom_left;
				data[((*it).y - 1)*App->map->data.width + (*it).x] = dim_bottom_left;
			
			
		}

		if (Get(((*it).x + 1), (*it).y) == dim_middle && Get((*it).x, (*it).y + 1) == dim_middle)
		{
			if (Get((*it).x - 1, (*it).y) == dim_middle)
			{
		
				data[(*it).y*App->map->data.width + (*it).x] = dim_middle; 
				data[((*it).y - 1)*App->map->data.width + (*it).x] = dim_down;
				continue;
			}
			
				data[(*it).y*App->map->data.width + (*it).x] = dim_inner_bottom_right;
				data[(*it).y*App->map->data.width + (*it).x - 1] = dim_bottom_right;
			
		
		}
			
	}




	return 0; 
}

void FogOfWar::UpdateEntitiesVisibleArea()
{
	int id = 0; 

	for (vector<Player*>::iterator it = players_on_fog.begin(); it != players_on_fog.end(); it++)
	{

		string direction = (*it)->player_go->animator->GetCurrentAnimation()->GetName(); 

		if (direction == "run_down")
			MoveArea(id, fow_down, current_visited_points);

		else if (direction == "run_up")
			MoveArea(id, fow_up, current_visited_points);

		else if (direction == "run_lateral" && (*it)->flip == true)
			MoveArea(id, fow_left, current_visited_points);

		else if (direction == "run_lateral")
			MoveArea(id, fow_right, current_visited_points);

		id++; 
	}
}

void FogOfWar::UpdateMatrix()
{
	bool dim = true;

	iPoint ini_p = App->map->WorldToMap(players_on_fog.at(0)->player_go->GetPos().x, players_on_fog.at(0)->player_go->GetPos().y);
	ini_p.y -= radium + 1; ini_p.x -= radium + 1;

	iPoint end_p = App->map->WorldToMap(players_on_fog.at(0)->player_go->GetPos().x, players_on_fog.at(0)->player_go->GetPos().y);
	end_p.y += radium + 2; end_p.x += radium + 2;

	for (int x = ini_p.x; x < end_p.x; x++)
	{
		for (int y = ini_p.y; y < end_p.y; y++)
		{
			dim = true;

			for (vector<iPoint>::iterator it = current_visited_points.begin(); it != current_visited_points.end(); it++)
				if (iPoint(x, y) == *it) 
					dim = false;
									 	
			if (data[y*App->map->data.width + x] == 0 && dim)
				data[y*App->map->data.width + x] = fow_null;
			else
				data[y*App->map->data.width + x] = dim_middle;

			if (!dim)
				data[y*App->map->data.width + x] = dim_clear;
		}
	}
}

void FogOfWar::MoveArea(int player_id, fow_directions direction, vector<iPoint>& current_points)
{
	// When adding more players manage with player_id 

	switch(direction)
	{
	case fow_up: 
		for (vector<iPoint>::iterator it = current_points.begin(); it != current_points.end(); it++)
			it->y -= 1; 

		for (list<iPoint>::iterator it = frontier.begin(); it != frontier.end(); it++)
			it->y -= 1;

		break; 

	case fow_down:
		for (vector<iPoint>::iterator it = current_points.begin(); it != current_points.end(); it++)
			it->y += 1;

		for (list<iPoint>::iterator it = frontier.begin(); it != frontier.end(); it++)
			it->y += 1;

		break;

	case fow_left:
		for (vector<iPoint>::iterator it = current_points.begin(); it != current_points.end(); it++)
			it->x -= 1;

		for (list<iPoint>::iterator it = frontier.begin(); it != frontier.end(); it++)
			it->x -= 1;

		break;

	case fow_right:
		for (vector<iPoint>::iterator it = current_points.begin(); it != current_points.end(); it++)
			it->x += 1;

		for (list<iPoint>::iterator it = frontier.begin(); it != frontier.end(); it++)
			it->x += 1;

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

bool FogOfWar::IsBorder(const char* dir)
{
	
	return false;
}
