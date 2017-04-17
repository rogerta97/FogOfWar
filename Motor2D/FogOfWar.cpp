#include "FogOfWar.h"
#include "j1Scene.h"
#include "p2Log.h"
#include "j1Input.h"
#include "Entity.h"
#include "j1Textures.h"
#include "MainScene.h"
#include "j1Map.h"
#include "GameObject.h"
#include "Player.h"

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

	player_frontier new_player; 

	if (new_entity == nullptr)
		return false;

	else if (new_entity->type == entity_name::player)
	{
		new_player.player_pos = App->map->WorldToMap(new_entity->player_go->GetPos().x, new_entity->player_go->GetPos().y);
		GetEntitiesVisibleArea(new_player); 
		players_on_fog.push_back(new_player); 
	}
		

	else
		simple_char_on_fog_pos.push_back(App->map->WorldToMap(new_entity->player_go->GetPos().x, new_entity->player_go->GetPos().y));

	return true;
}

uint FogOfWar::Get(int x, int y)
{
	return data[(y*App->map->data.width) + x];
}

void FogOfWar::Start()
{
	//FillFrontier(); 
	//RemoveDimJaggies();
	ManageCharacters(); 
}

void FogOfWar::Update(iPoint prev_pos)
{
	MoveFrontier(prev_pos);
	FillFrontier();
	RemoveDimJaggies(); 

	ManageCharacters(); 

}

void FogOfWar::GetEntitiesVisibleArea(player_frontier& new_player)
{

	// We BFS the for knowing the tiles we must blit! 
	
	new_player.frontier = App->map->PropagateBFS({ new_player.player_pos.x, new_player.player_pos.y }, FOW_RADIUM);

		// Once we have the frontier we delete the picks

		DeletePicks(new_player);

		// With that, we modify the data container

		for (list<iPoint>::iterator it = new_player.frontier.begin(); it != new_player.frontier.end(); it++)
			data[(*it).y * App->map->data.width + (*it).x] = dim_clear;	
	 
}

uint FogOfWar::RemoveDimJaggies()
{
	for (vector<player_frontier>::iterator curr = players_on_fog.begin(); curr != players_on_fog.end(); curr++)
	{
		for (list<iPoint>::iterator it = curr->frontier.begin(); it != curr->frontier.end(); it++)
		{		
			if ((*it) == iPoint(40, 21))
			{
				LOG(""); 
			}
				if (Get((*it).x, (*it).y + 1) == dim_middle || Get((*it).x, (*it).y + 1) == fow_null)
					data[(*it).y*App->map->data.width + (*it).x] = dim_down;

				if (Get((*it).x, (*it).y - 1) == dim_middle || Get((*it).x, (*it).y - 1) == fow_null)
					data[(*it).y*App->map->data.width + (*it).x] = dim_up;

				if (Get((*it).x - 1, (*it).y) == dim_middle || Get((*it).x - 1, (*it).y) == fow_null)
				{
					data[(*it).y*App->map->data.width + ((*it).x)] = dim_left;

					if (Get((*it).x, (*it).y - 1) == dim_middle || Get((*it).x, (*it).y - 1) == fow_null)
					{
						data[((*it).y)*App->map->data.width + (*it).x] = dim_inner_top_left;

						if (Get(((*it).x + 1), (*it).y - 1) != dim_middle && Get(((*it).x + 1), (*it).y - 1) != fow_null)
							data[((*it).y)*App->map->data.width + ((*it).x + 1)] = dim_top_left;

						if (Get(((*it).x - 1), (*it).y + 1) != dim_middle && Get(((*it).x - 1), (*it).y + 1) != fow_null)
							data[((*it).y + 1)*App->map->data.width + ((*it).x)] = dim_top_left;
					}

					else if (Get((*it).x, (*it).y + 1) == dim_middle || Get((*it).x, (*it).y + 1) == fow_null)
					{
						data[((*it).y)*App->map->data.width + (*it).x] = dim_inner_bottom_left;

						if (Get(((*it).x + 1), (*it).y + 1) != dim_middle && Get((*it).x + 1, (*it).y + 1) != fow_null)
							data[((*it).y)*App->map->data.width + ((*it).x + 1)] = dim_bottom_left;

						if (Get(((*it).x - 1), (*it).y - 1) != dim_middle && Get(((*it).x - 1), (*it).y - 1) != fow_null)
							data[((*it).y - 1)*App->map->data.width + ((*it).x)] = dim_bottom_left;
					}

				}
				else if (Get((*it).x + 1, (*it).y) == dim_middle || Get((*it).x + 1, (*it).y) == fow_null)
				{
					data[(*it).y*App->map->data.width + ((*it).x)] = dim_right;

					if (Get((*it).x, (*it).y - 1) == dim_middle || Get((*it).x, (*it).y - 1) == fow_null)
					{
						data[((*it).y)*App->map->data.width + (*it).x] = dim_inner_top_right;

						if (Get(((*it).x - 1), (*it).y - 1) != dim_middle && Get(((*it).x - 1), (*it).y - 1) != fow_null)
							data[((*it).y)*App->map->data.width + ((*it).x - 1)] = dim_top_right;

						if(Get(((*it).x + 1), (*it).y + 1) != dim_middle && Get(((*it).x + 1), (*it).y + 1) != fow_null)
							data[((*it).y + 1)*App->map->data.width + ((*it).x)] = dim_top_right;
					}

					else if (Get((*it).x, (*it).y + 1) == dim_middle || Get((*it).x, (*it).y + 1) == fow_null)
					{
						data[((*it).y)*App->map->data.width + (*it).x] = dim_inner_bottom_right;

						if (Get(((*it).x - 1), (*it).y + 1) != dim_middle && Get(((*it).x - 1), (*it).y + 1) != fow_null)
							data[((*it).y)*App->map->data.width + ((*it).x - 1)] = dim_bottom_right;


						if (Get(((*it).x + 1), (*it).y - 1) != dim_middle && Get(((*it).x + 1), (*it).y - 1) != fow_null)
							data[((*it).y - 1)*App->map->data.width + ((*it).x)] = dim_bottom_right;
					}
				}
			}
		}
	 return 0; 
}

void FogOfWar::RemoveDarkJaggies(iPoint curr)
{
	if ( Get(curr.x, curr.y + 1) == fow_null)
		data[curr.y*App->map->data.width + curr.x] = darkc_down;

	if ( Get(curr.x, curr.y - 1) == fow_null)
		data[curr.y*App->map->data.width + curr.x] = dim_up;

	if ( Get(curr.x - 1, curr.y) == fow_null)
	{
		data[curr.y*App->map->data.width + (curr.x)] = dim_left;

		if (Get(curr.x, curr.y - 1) == dim_middle || Get(curr.x, curr.y - 1) == fow_null)
		{
			data[(curr.y)*App->map->data.width + curr.x] = dim_inner_top_left;

			if (Get((curr.x + 1), curr.y - 1) != dim_middle && Get((curr.x + 1), curr.y - 1) != fow_null)
				data[(curr.y)*App->map->data.width + (curr.x + 1)] = dim_top_left;

			if (Get((curr.x - 1), curr.y + 1) != dim_middle && Get((curr.x - 1), curr.y + 1) != fow_null)
				data[(curr.y + 1)*App->map->data.width + (curr.x)] = dim_top_left;
		}

		else if (Get(curr.x, curr.y + 1) == dim_middle || Get(curr.x, curr.y + 1) == fow_null)
		{
			data[(curr.y)*App->map->data.width + curr.x] = dim_inner_bottom_left;

			if (Get((curr.x + 1), curr.y + 1) != dim_middle && Get(curr.x + 1, curr.y + 1) != fow_null)
				data[(curr.y)*App->map->data.width + (curr.x + 1)] = dim_bottom_left;

			if (Get((curr.x - 1), curr.y - 1) != dim_middle && Get((curr.x - 1), curr.y - 1) != fow_null)
				data[(curr.y - 1)*App->map->data.width + (curr.x)] = dim_bottom_left;
		}

	}
	else if (Get(curr.x + 1, curr.y) == dim_middle || Get(curr.x + 1, curr.y) == fow_null)
	{
		data[curr.y*App->map->data.width + (curr.x)] = dim_right;

		if (Get(curr.x, curr.y - 1) == dim_middle || Get(curr.x, curr.y - 1) == fow_null)
		{
			data[(curr.y)*App->map->data.width + curr.x] = dim_inner_top_right;

			if (Get((curr.x - 1), curr.y - 1) != dim_middle && Get((curr.x - 1), curr.y - 1) != fow_null)
				data[(curr.y)*App->map->data.width + (curr.x - 1)] = dim_top_right;

			if (Get((curr.x + 1), curr.y + 1) != dim_middle && Get((curr.x + 1), curr.y + 1) != fow_null)
				data[(curr.y + 1)*App->map->data.width + (curr.x)] = dim_top_right;
		}

		else if (Get(curr.x, curr.y + 1) == dim_middle || Get(curr.x, curr.y + 1) == fow_null)
		{
			data[(curr.y)*App->map->data.width + curr.x] = dim_inner_bottom_right;

			if (Get((curr.x - 1), curr.y + 1) != dim_middle && Get((curr.x - 1), curr.y + 1) != fow_null)
				data[(curr.y)*App->map->data.width + (curr.x - 1)] = dim_bottom_right;


			if (Get((curr.x + 1), curr.y - 1) != dim_middle && Get((curr.x + 1), curr.y - 1) != fow_null)
				data[(curr.y - 1)*App->map->data.width + (curr.x)] = dim_bottom_right;
		}
	}
}

void FogOfWar::MoveFrontier(iPoint prev_pos)
{
	string direction = curr_character->player_go->animator->GetCurrentAnimation()->GetName();

	for (vector<player_frontier>::iterator it = players_on_fog.begin(); it != players_on_fog.end(); it++)
	{	
		if (prev_pos == it->player_pos)
		{
			MoveArea(*it, direction);
			break;
		}			
	}
	
}

void FogOfWar::FillFrontier()
{

	for (vector<player_frontier>::iterator it = players_on_fog.begin(); it != players_on_fog.end(); it++)
	{

	iPoint ini_p = iPoint(it->player_pos.x, it->player_pos.y);
	ini_p.y -= FOW_RADIUM + 1; ini_p.x -= FOW_RADIUM + 1;

	iPoint end_p = iPoint(it->player_pos.x, it->player_pos.y);
	end_p.y += FOW_RADIUM + 2; end_p.x += FOW_RADIUM + 2;

	
		for (int y = ini_p.y; y < end_p.y; y++)
		{
			for (int x = ini_p.x; x < end_p.x; x++)
			{
				if (iPoint(x, y).DistanceTo(it->player_pos) < FOW_RADIUM - 1)
				{
					data[y*App->map->data.width + x] = dim_clear;
				}


			}
		}

		
	}
			
}

void FogOfWar::MoveArea(player_frontier& player, string direction_str)
{

	int direction = -1; 

	if (direction_str == "run_down")
		direction = fow_down;

	else if (direction_str == "run_up")
		direction = fow_up;

	else if (direction_str == "run_lateral" && curr_character->flip == true)
		direction = fow_left;

	else if (direction_str == "run_lateral")
		direction = fow_right;
	else
		LOG(""); 


	for (list<iPoint>::iterator it = player.frontier.begin(); it != player.frontier.end(); it++)
		data[(*it).y * App->map->data.width + (*it).x] = dim_middle;

	switch(direction)
	{
	case fow_up: 
		for (list<iPoint>::iterator it = player.frontier.begin(); it != player.frontier.end(); it++)
			(*it).y -= 1;

		player.player_pos.y -= 1; 

		break; 

	case fow_down:
		for (list<iPoint>::iterator it = player.frontier.begin(); it != player.frontier.end(); it++)
			(*it).y += 1;

		player.player_pos.y += 1;


		break;

	case fow_left:
		for (list<iPoint>::iterator it = player.frontier.begin(); it != player.frontier.end(); it++)
			(*it).x -= 1;

		player.player_pos.x -= 1;

		break;

	case fow_right:
		for (list<iPoint>::iterator it = player.frontier.begin(); it != player.frontier.end(); it++)
			(*it).x += 1;

		player.player_pos.x += 1;

		break;
	}

	for (list<iPoint>::iterator it = player.frontier.begin(); it != player.frontier.end(); it++)
		data[(*it).y * App->map->data.width + (*it).x] = dim_clear;
}

SDL_Rect FogOfWar::GetRect(int fow_id)
{
	SDL_Rect rect_ret = { 0, 0, 32, 32 };

	int columns = 13; 

	if(fow_id > 0 && fow_id <= dim_inner_bottom_right)
	{
		rect_ret.y = 0;
		rect_ret.x = 32 * (fow_id - 1); 	
	}
	else if (fow_id > dim_inner_bottom_right && fow_id <= darkc_inner_bottom_right)
	{
		fow_id -= columns; 
		rect_ret.y = 32;
		rect_ret.x = 32 * (fow_id - 1);
	}

	else if (fow_id > darkc_inner_bottom_right && fow_id <= darkd_inner_bottom_right)
	{
		fow_id -= (columns*2);
		rect_ret.y = 64;
		rect_ret.x = 32 * (fow_id - 1);
	}

	return rect_ret;
}

void FogOfWar::DeletePicks(player_frontier& player)
{
	int count = 0;

	for (list<iPoint>::iterator it = player.frontier.begin(); it != player.frontier.end(); it++)
	{
		if ((*it) == iPoint(player.player_pos.x, player.player_pos.y + FOW_RADIUM))
			(*it) = iPoint((*it).x, (*it).y - 1); 

		else if ((*it) == iPoint(player.player_pos.x, player.player_pos.y - FOW_RADIUM))
			(*it) = iPoint((*it).x, (*it).y + 1);

		else if ((*it) == iPoint(player.player_pos.x + FOW_RADIUM, player.player_pos.y))
			(*it) = iPoint((*it).x - 1, (*it).y);

		else if ((*it) == iPoint(player.player_pos.x - FOW_RADIUM, player.player_pos.y))
			(*it) = iPoint((*it).x + 1, (*it).y);
	}

}

void FogOfWar::ChangeCharacter(iPoint prev_pos)
{
	iPoint next_character_pos; 

	int count = 1; 

	for (vector<player_frontier>::iterator it = players_on_fog.begin(); it != players_on_fog.end(); it++)
	{
		if (prev_pos == it->player_pos) 
		{

			if (count < players_on_fog.size()) 
			{
				it++; 
				next_character_pos = it->player_pos;
			}
				
			else
			{
				next_character_pos = players_on_fog.begin()->player_pos; 
			}
		}	

		count++; 
	}

	for (list<Entity*>::iterator it = App->entity->entity_list.begin(); it != App->entity->entity_list.end(); it++)
	{
		if (App->map->WorldToMap((*it)->player_go->GetPos().x, (*it)->player_go->GetPos().y) == next_character_pos)
			curr_character = (Player*)*it; 		
	}

}

void FogOfWar::ManageCharacters()
{
	bool entered = false; 

	for(list<iPoint>::iterator it = simple_char_on_fog_pos.begin(); it != simple_char_on_fog_pos.end(); it++)
	{		
			if (IsVisible(*it))
			{
				for (list<Entity*>::iterator it3 = App->entity->entity_list.begin(); it3 != App->entity->entity_list.end(); it3++)
				{
					if (App->map->WorldToMap((*it3)->player_go->GetPos().x, (*it3)->player_go->GetPos().y) == *it)
					{
						(*it3)->active = true;
						entered = true; 
					}
						
				}
		}

		if (!entered)
		{
			for (list<Entity*>::iterator it3 = App->entity->entity_list.begin(); it3 != App->entity->entity_list.end(); it3++)
			{
				if (App->map->WorldToMap((*it3)->player_go->GetPos().x, (*it3)->player_go->GetPos().y) == *it)
					(*it3)->active = false;
			}
		}
			
		
	}

}

bool FogOfWar::IsVisible(iPoint char_pos)
{

	if (Get(char_pos.x, char_pos.y) == fow_null)
		return false;

	else if (Get(char_pos.x, char_pos.y) == dim_middle)
		return false;

	else
		return true; 

}






