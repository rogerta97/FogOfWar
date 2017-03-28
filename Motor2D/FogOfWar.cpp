#include "FogOfWar.h"
#include "j1Scene.h"
#include "MainScene.h"
#include "j1Map.h"
#include "GameObject.h"
#include "Player.h"

FogOfWar::FogOfWar()
{
	//iPoint player_pos = App->map->WorldToMap(App->scene->main_scene->player->player_go->GetPos().x, App->scene->main_scene->player->player_go->GetPos().y);

	int size = App->map->data.width*App->map->data.height;
	data = new uint[size];

	memset(data, 1, size*sizeof(uint)); 

}

FogOfWar::~FogOfWar()
{
}

bool FogOfWar::AddEntity(Entity* new_entity)
{
	if (new_entity == nullptr)
		return false; 
	
	entities_on_fog.push_back(new_entity);
	return true;
}

uint FogOfWar::Get(int x, int y)
{
	return data[(y*App->map->data.width) + x];
}

void FogOfWar::GetEntitiesVisibleArea(vector<iPoint*>& current_visited_points)
{

}
