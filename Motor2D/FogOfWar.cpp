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

	memset(data, 0, size*sizeof(uint)); 


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

void FogOfWar::GetEntitiesVisibleArea(vector<iPoint>& current_visited_points, int limit)
{
	vector<iPoint> origins;
	vector<iPoint> seen_nodes; 
	bool repeated = false; 
	bool opaque = false; 
	
	// We extend the BFS for every origin, each origin means each player.

	for (list<Player*>::iterator it = players_on_fog.begin(); it != players_on_fog.end(); it++)
	{
		App->map->PropagateBFS(App->map->WorldToMap((*it)->player_go->GetPos().x, (*it)->player_go->GetPos().y), seen_nodes, limit);

		// Passing each visible area to the total amount of visible tiles (if they are not yet)

		for (vector<iPoint>::iterator it = seen_nodes.begin(); it != seen_nodes.end(); it++)
		{			
			repeated = false; 

			for (vector<iPoint>::iterator it2 = current_visited_points.begin(); it2 != current_visited_points.end(); it2++)
			{
				if (*it == *it2) 
				{
					repeated = true;
					break; 
				}			
			}
			if(!repeated)
				current_visited_points.push_back(*it);		
		}
	}

	// ----

	// Update the data matrix in order to draw 

	for (vector<iPoint>::iterator it = current_visited_points.begin(); it != current_visited_points.end(); it++)
	{
		opaque = false; 

		data[it->y*App->map->data.width + it->x] = 2;

		for (vector<iPoint>::iterator it2 = seen_nodes.begin(); it2 != seen_nodes.end(); it2++)
		{
			if (*it == *it2)
				opaque = true; 
		}

		if(!opaque)
			data[it->y*App->map->data.width + it->x] = 1;
	}
	

	// ----
}
