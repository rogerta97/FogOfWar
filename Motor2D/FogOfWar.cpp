#include "FogOfWar.h"
#include "j1Scene.h"
#include "p2Log.h"
#include "Entity.h"
#include "j1Textures.h"
#include "MainScene.h"
#include "j1Map.h"
#include "GameObject.h"
#include "Player.h"

FogOfWar::FogOfWar()
{
	//iPoint player_pos = App->map->WorldToMap(App->scene->main_scene->player->player_go->GetPos().x, App->scene->main_scene->player->player_go->GetPos().y);

	int size = App->map->data.width*App->map->data.height;
	data = new uint[size];

	memset(data, 0, size * sizeof(uint));
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

void FogOfWar::Start(int radium)
{
	GetEntitiesVisibleArea(radium);
	//current_visited_points = start_visited_points; 
}

void FogOfWar::Update(vector<iPoint>& current_points)
{
	UpdateEntitiesVisibleArea(); 
	UpdateMatrix(); 
}

void FogOfWar::GetEntitiesVisibleArea(int limit)
{
	vector<iPoint> seen_nodes;
	bool repeated = false;
	bool opaque = false;

	// We BFS the first time for knowing the tiles we must blit! 
	for (vector<Player*>::iterator it = players_on_fog.begin(); it != players_on_fog.end(); it++)
	{
		App->map->PropagateBFS(App->map->WorldToMap((*it)->player_go->GetPos().x, (*it)->player_go->GetPos().y), seen_nodes, limit);

		// Passing each visible area to the total amount of visible tiles (if they are not yet)

		for (vector<iPoint>::iterator it = seen_nodes.begin(); it != seen_nodes.end(); it++)
			current_visited_points.push_back(*it);	
	}

	// Update the data matrix in order to draw 
	for (vector<iPoint>::iterator it = current_visited_points.begin(); it != current_visited_points.end(); it++)
		data[it->y*App->map->data.width + it->x] = 2;
	

	// ----
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
	bool opaque = false; 

	for (vector<iPoint>::iterator it = current_visited_points.begin(); it != current_visited_points.end(); it++)
	{


		data[it->y*App->map->data.width + it->x] = 2;



	/*aaa	opaque = false; 

		

	
			if (*it == *it2)
				opaque = true; 
	

		if(!opaque)
		data[it->y*App->map->data.width + it->x] = 1;*/
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

		break; 

	case fow_down:
		for (vector<iPoint>::iterator it = current_points.begin(); it != current_points.end(); it++)
			it->y += 1;

		break;

	case fow_left:
		for (vector<iPoint>::iterator it = current_points.begin(); it != current_points.end(); it++)
			it->x -= 1;

		break;

	case fow_right:
		for (vector<iPoint>::iterator it = current_points.begin(); it != current_points.end(); it++)
			it->x += 1;

		break;
	}
}
