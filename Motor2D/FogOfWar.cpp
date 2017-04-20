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

	// TODO 3 ---  If a player is added, fill the player frontier with the information and add it to the list 

	else if (new_entity->type == entity_name::player)
	{
		new_player.player_pos = App->map->WorldToMap(new_entity->player_go->GetPos().x, new_entity->player_go->GetPos().y);
		GetEntitiesCircleArea(new_player);
		new_player.id = new_entity->id;
		new_entity->is_on_fow = true; 
		players_on_fog.push_back(new_player);
	}

	else
		simple_char_on_fog_pos.push_back(App->map->WorldToMap(new_entity->player_go->GetPos().x, new_entity->player_go->GetPos().y));

	// TODO 3 ----

	return true;
}

uint FogOfWar::Get(int x, int y)
{
	return data[(y*App->map->data.width) + x];
}

void FogOfWar::Start()
{
	FillFrontier();

	//RemoveDimJaggies();
	//RemoveDarkJaggies(); 

	ManageCharacters(); 
}

void FogOfWar::Update(iPoint prev_pos, iPoint next_pos)
{
	// We look for the direction that the player is moving

	if(prev_pos.x + 1 == next_pos.x)
		MoveFrontier(prev_pos, "right");

	else if (prev_pos.x - 1 == next_pos.x)
		MoveFrontier(prev_pos, "left");

	else if (prev_pos.y + 1 == next_pos.y)
		MoveFrontier(prev_pos, "down");

	else if (prev_pos.y - 1 == next_pos.y)
		MoveFrontier(prev_pos, "up");


	FillFrontier();

	//RemoveDimJaggies(); 
	//RemoveDarkJaggies(); 

	ManageCharacters(); 

}

void FogOfWar::GetEntitiesCircleArea(player_frontier& new_player)
{

	// We BFS the for knowing the tiles we must blit! 

	// TODO 1 ---- Modify BFS, it will have to return the frontier and expand untill a certain radius.
	
	new_player.frontier = App->map->PropagateBFS({ new_player.player_pos.x, new_player.player_pos.y }, FOW_RADIUM);

	// TODO 1 ----

	// TODO 2 ---- Delete the picks of the frontier and modify it on the current data of the FOW

	DeletePicks(new_player);

	GetCurrentPointsFromFrontier(new_player); 

	for (list<iPoint>::iterator it = new_player.frontier.begin(); it != new_player.frontier.end(); it++)
		data[(*it).y * App->map->data.width + (*it).x] = dim_clear;
	
	// TODO 2 -----
}

void FogOfWar::GetEntitiesRectangleFrontier(player_frontier & new_player)
{
}

uint FogOfWar::RemoveDimJaggies()
{

	// In this part we place the edges and the inner corners

	for (vector<player_frontier>::iterator curr = players_on_fog.begin(); curr != players_on_fog.end(); curr++)
	{
		for (list<iPoint>::iterator it = curr->frontier.begin(); it != curr->frontier.end(); it++)
		{
			if (Get((*it).x - 1, (*it).y) == dim_middle || Get((*it).x - 1, (*it).y) == fow_null)
				data[(*it).y*App->map->data.width + (*it).x] = dim_left; 

			else if (Get((*it).x + 1, (*it).y) == dim_middle || Get((*it).x + 1, (*it).y) == fow_null)
				data[(*it).y*App->map->data.width + (*it).x] = dim_right;
		}

		for (list<iPoint>::iterator it = curr->frontier.begin(); it != curr->frontier.end(); it++)
		{
			if (Get((*it).x, (*it).y - 1) == dim_middle || Get((*it).x, (*it).y - 1) == fow_null )
			{
				if (data[(*it).y* App->map->data.width + (*it).x] == dim_left)
					data[(*it).y* App->map->data.width + (*it).x] = dim_inner_top_left;

				else if (data[(*it).y* App->map->data.width + (*it).x] == dim_right)
					data[(*it).y* App->map->data.width + (*it).x] = dim_inner_top_right;

				else
					data[(*it).y* App->map->data.width + (*it).x] = dim_up; 

			}
				
			else if (Get((*it).x, (*it).y + 1) == dim_middle || Get((*it).x, (*it).y + 1) == fow_null)
			{
				if(data[(*it).y* App->map->data.width + (*it).x] == dim_right)
					data[(*it).y*App->map->data.width + (*it).x] = dim_inner_bottom_right;

				else if (data[(*it).y* App->map->data.width + (*it).x] == dim_left)
					data[(*it).y*App->map->data.width + (*it).x] = dim_inner_bottom_left;

				else
					data[(*it).y* App->map->data.width + (*it).x] = dim_down;
			}
				
		}
	}

	// ------

	// Here we place the outermost corners 

	for (vector<player_frontier>::iterator curr = players_on_fog.begin(); curr != players_on_fog.end(); curr++)
	{
		for (list<iPoint>::iterator it = curr->frontier.begin(); it != curr->frontier.end(); it++)
		{
			if (Get((*it).x, (*it).y) == dim_inner_top_left)
			{
				if (data[(*it).y* App->map->data.width + (*it).x + 1] == dim_clear)
					data[(*it).y * App->map->data.width + (*it).x + 1] = dim_top_left;


			}

			else if (Get((*it).x, (*it).y) == dim_inner_top_right)
			{
				if (data[(*it).y* App->map->data.width + (*it).x - 1] == dim_clear)
					data[(*it).y * App->map->data.width + (*it).x - 1] = dim_top_right;

			}

			else if (Get((*it).x, (*it).y) == dim_inner_bottom_right)
			{
				if (data[(*it).y* App->map->data.width + (*it).x - 1] == dim_clear)
					data[(*it).y * App->map->data.width + (*it).x - 1] = dim_bottom_right;

			}

			else if (Get((*it).x, (*it).y) == dim_inner_bottom_left)
			{
				if (data[(*it).y* App->map->data.width + (*it).x + 1] == dim_clear)
					data[(*it).y * App->map->data.width + (*it).x + 1] = dim_bottom_left;

			}

			else if (Get((*it).x, (*it).y) == dim_up && Get((*it).x + 1, (*it).y - 1) == dim_left &&Get((*it).x, (*it).y) == dim_inner_top_left)
				data[(*it).y* App->map->data.width + (*it).x + 1] = dim_top_left; 

			
		}
	}

	// ------

	 return 0; 
}

void FogOfWar::RemoveDarkJaggies()
{

	for (vector<player_frontier>::iterator it = players_on_fog.begin(); it != players_on_fog.end(); it++)
	{

		iPoint ini_p = iPoint(it->player_pos.x, it->player_pos.y);
		ini_p.y -= FOW_RADIUM + 1; ini_p.x -= FOW_RADIUM + 1;

		iPoint end_p = iPoint(it->player_pos.x, it->player_pos.y);
		end_p.y += FOW_RADIUM + 1; end_p.x += FOW_RADIUM + 1;

		// Horitzontals ----

		for (int y = ini_p.y; y < end_p.y; y++)
		{
			for (int x = ini_p.x; x < end_p.x; x++)
			{

				if (Get(x, y) == dim_middle && Get(x - 1, y) == fow_null)
					data[y*App->map->data.width + x] = darkd_left;

				else if (Get(x, y) == dim_middle && Get(x + 1, y) == fow_null)
					data[y*App->map->data.width + x] = darkd_right;

				if (Get(x, y) == dim_right  &&  Get(x + 1, y) == fow_null)
					data[y*App->map->data.width + x] = darkc_right;

				if ((Get(x, y) == dim_left &&  Get(x - 1, y) == fow_null))
					data[y*App->map->data.width + x] = darkc_left;

	
			}
		}

		// -----

		// Verticals (ading corner in case of overlaping) 


		for (int y = ini_p.y; y < end_p.y; y++)
		{
			for (int x = ini_p.x; x < end_p.x; x++)
			{

			// Dark->dim

				// Comparing upper

				if (Get(x, y) == darkd_right && Get(x, y - 1) == fow_null)
					data[y*App->map->data.width + x] = darkd_inner_top_right;

				else if (Get(x, y) == darkd_left && Get(x, y - 1) == fow_null)
					data[y*App->map->data.width + x] = darkd_inner_top_left;

				else if (Get(x, y) == dim_middle && Get(x, y - 1) == fow_null)
					data[y*App->map->data.width + x] = darkd_up;

				// Comparing lower

				if (Get(x, y) == darkd_right && Get(x, y + 1) == fow_null)
					data[y*App->map->data.width + x] = darkd_inner_bottom_right;

				else if (Get(x, y) == darkd_left && Get(x, y + 1) == fow_null)
					data[y*App->map->data.width + x] = darkd_inner_bottom_left;

				else if (Get(x, y) == dim_middle && Get(x, y + 1) == fow_null)
					data[y*App->map->data.width + x] = darkd_down;

				// ----

				// Dark->clear

				if ((Get(x, y) == dim_inner_bottom_left &&  Get(x - 1, y) == fow_null))
					data[y*App->map->data.width + x] = darkc_inner_bottom_left;

				else if ((Get(x, y) == dim_inner_top_left && Get(x - 1, y) == fow_null))
					data[y*App->map->data.width + x] = darkc_inner_top_left;

				 if ((Get(x, y) == dim_inner_bottom_right &&  Get(x + 1, y) == fow_null))
				data[y*App->map->data.width + x] = darkc_inner_bottom_right;

				else if ((Get(x, y) == dim_inner_top_right &&  Get(x + 1, y) == fow_null))
				data[y*App->map->data.width + x] = darkc_inner_top_right;


				if((Get(x, y) == dim_up &&  Get(x, y - 1) == fow_null))
					 data[y*App->map->data.width + x] = darkc_up;

				else if ((Get(x, y) == dim_down && Get(x, y + 1) == fow_null))
					 data[y*App->map->data.width + x] = darkc_down;
			}
		}

	// ----

	// Corners 

		for (int y = ini_p.y; y < end_p.y; y++)
		{
			for (int x = ini_p.x; x < end_p.x; x++)
			{

				// When dark is next to the player


				if (Get(x, y) == darkc_inner_bottom_right)
				{

					if (Get(x - 1, y) != darkc_down)			
						data[y*App->map->data.width + (x - 1)] = darkc_bottom_right;
				}

				else if (Get(x, y) == darkc_inner_bottom_left)
				{
					if (Get(x + 1, y) != darkc_down)
					data[y*App->map->data.width + (x + 1)] = darkc_bottom_left; 
				}

				else if (Get(x, y) == darkc_inner_top_right)
				{
					if (Get(x - 1, y) != darkc_up)
						data[y*App->map->data.width + (x - 1)] = darkc_top_right;
				}

				else if (Get(x, y) == darkc_inner_top_left)
				{
					if (Get(x + 1, y) != darkc_up)
						data[y*App->map->data.width + (x + 1)] = darkc_top_left;
				}



				// corners already visited


				if (Get(x, y) == darkd_inner_bottom_right)
				{

					if (Get(x - 1, y) != darkd_down )
						data[y*App->map->data.width + (x - 1)] = darkd_bottom_right;
				}

				else if (Get(x, y) == darkd_inner_bottom_left)
				{
					if (Get(x + 1, y) != darkd_down)
						data[y*App->map->data.width + (x + 1)] = darkd_bottom_left;
				}

				else if (Get(x, y) == darkd_inner_top_right)
				{
					if (Get(x - 1, y) != darkd_up)
						data[y*App->map->data.width + (x - 1)] = darkd_top_right;
				}

				else if (Get(x, y) == darkd_inner_top_left)
				{
					if (Get(x + 1, y) != darkd_up)
						data[y*App->map->data.width + (x + 1)] = darkd_top_left;
				}

				else if (Get(x, y) == darkd_up && Get(x + 1, y - 1) == darkd_left)
					data[y*App->map->data.width + (x + 1)] = darkd_top_left;

				else if (Get(x, y) == darkd_up && Get(x - 1, y - 1) == darkd_right)
					data[y*App->map->data.width + (x - 1)] = darkd_top_right;

				else if (Get(x, y) == darkd_down && Get(x + 1, y + 1) == darkd_left)
					data[y*App->map->data.width + (x + 1)] = darkd_bottom_left;

				else if (Get(x, y) == darkd_down && Get(x - 1, y + 1) == darkd_right)
					data[y*App->map->data.width + (x - 1)] = darkd_bottom_right;
			}
		}

	
	}

}


void FogOfWar::MoveFrontier(iPoint prev_pos, const char* direction)
{
	string direction_str(direction); 

	for (vector<player_frontier>::iterator it = players_on_fog.begin(); it != players_on_fog.end(); it++)
	{	
		if (it->id == App->entity->curr_entity->id)
		{
			MoveArea(*it, direction); // TODO 5 --- Actually Move the frontier of each player in case of update 
			break;
		}			
	}
	
}

void FogOfWar::GetCurrentPointsFromFrontier(player_frontier& player)
{

	iPoint ini_p = iPoint(player.player_pos.x, player.player_pos.y);
	ini_p.y -= FOW_RADIUM + 1; ini_p.x -= FOW_RADIUM + 1;

	iPoint end_p = iPoint(player.player_pos.x, player.player_pos.y);
	end_p.y += FOW_RADIUM + 1; end_p.x += FOW_RADIUM + 1;

	//First we add the frontier 

	for (list<iPoint>::iterator it = player.frontier.begin(); it != player.frontier.end(); it++)
	{
		player.current_points.push_back(*it);
	}

	// Now we add the tiles inside 

	bool advance = false; 

	for (list<iPoint>::iterator it = player.frontier.begin(); it != player.frontier.end(); it++)
	{
		if (it->x < player.player_pos.x && it->y != player.player_pos.y + (FOW_RADIUM - 1) && it->y != player.player_pos.y - (FOW_RADIUM - 1))
		{
			for (int x = it->x + 1;;x++)
			{
				if (IsFrontier(iPoint(x, it->y), player))
					break; 

				else
					player.current_points.push_back({ x, it->y });				
			}
		}
	}
	
}



void FogOfWar::FillFrontier()
{

	// TODO 4 ---- Make a function for filling the frontier. 

	for (vector<player_frontier>::iterator it = players_on_fog.begin(); it != players_on_fog.end(); it++)
	{

		GetCurrentPointsFromFrontier(*it);

		
	}

	// TODO 4 -----
			
}

void FogOfWar::MoveArea(player_frontier& player_unity, string direction_str)
{
	for (list<iPoint>::iterator it = player_unity.frontier.begin(); it != player_unity.frontier.end(); it++)
		data[(*it).y * App->map->data.width + (*it).x] = dim_middle;

	if (direction_str == "right")
	{
		for (list<iPoint>::iterator it = player_unity.frontier.begin(); it != player_unity.frontier.end(); it++)
			(*it).x += 1;

		player_unity.player_pos.x += 1;
	}


	else if (direction_str == "left")
	{
		for (list<iPoint>::iterator it = player_unity.frontier.begin(); it != player_unity.frontier.end(); it++)
			(*it).x -= 1;

		player_unity.player_pos.x -= 1;
	}
	

	else if (direction_str == "up")
	{
		for (list<iPoint>::iterator it = player_unity.frontier.begin(); it != player_unity.frontier.end(); it++)
			(*it).y -= 1;

		player_unity.player_pos.y -= 1;
	}
	

	else if (direction_str == "down")
	{
		for (list<iPoint>::iterator it = player_unity.frontier.begin(); it != player_unity.frontier.end(); it++)
			(*it).y += 1;

		player_unity.player_pos.y += 1;
	}
	

	for (list<iPoint>::iterator it = player_unity.frontier.begin(); it != player_unity.frontier.end(); it++)
	{
		data[(*it).y * App->map->data.width + (*it).x] = dim_clear;
	}

	// Redraw the others in case of overlaping

	for (vector<player_frontier>::iterator it = players_on_fog.begin(); it != players_on_fog.end(); it++) 
	{
		if (it->id != App->entity->curr_entity->id)
		{
			for (list<iPoint>::iterator it2 = it->frontier.begin(); it2 != it->frontier.end(); it2++)
			{
				data[(*it2).y * App->map->data.width + (*it2).x] = dim_clear;
			}
		}
	}
		
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
	else if(fow_id > dim_inner_bottom_right && fow_id <= darkd_inner_bottom_right)
	{
		fow_id -= columns; 
		rect_ret.y = 32;
		rect_ret.x = 32 * (fow_id - 1);
	}
	else if (fow_id > darkd_inner_bottom_right && fow_id <= darkc_inner_bottom_right)
	{
		fow_id -= (columns*2);
		rect_ret.y = 32;
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


	/*iPoint next_character_pos; 

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
	}*/



void FogOfWar::ManageCharacters()
{
	

	// TODO 6 --- Make simple characters appear or disappear if they are close to one of the players in the FOW

	//for(list<iPoint>::iterator it = simple_char_on_fog_pos.begin(); it != simple_char_on_fog_pos.end(); it++)
	//{		
	//	bool entered = false;

	//		if (IsVisible(*it))
	//		{
	//			for (list<Entity*>::iterator it2 = App->entity->entity_list.begin(); it2 != App->entity->entity_list.end(); it2++)
	//			{
	//				if (App->map->WorldToMap((*it2)->player_go->GetPos().x, (*it2)->player_go->GetPos().y) == *it)
	//				{
	//					(*it2)->active = true;
	//					entered = true; 
	//				}
	//					
	//			}
	//	}

	//	if (!entered)
	//	{
	//		for (list<Entity*>::iterator it2 = App->entity->entity_list.begin(); it2 != App->entity->entity_list.end(); it2++)
	//		{
	//			if (App->map->WorldToMap((*it2)->player_go->GetPos().x, (*it2)->player_go->GetPos().y) == *it)
	//				(*it2)->active = false;
	//		}
	//	}
	//		
	//	
	//}

}

bool FogOfWar::IsVisible(iPoint char_pos)
{

	if (Get(char_pos.x, char_pos.y) > dim_middle && Get(char_pos.x, char_pos.y) < darkd_middle)		
		return true;

	if (Get(char_pos.x, char_pos.y) >= darkc_middle && Get(char_pos.x, char_pos.y) < darkc_inner_bottom_right)
		return true; 

	if (Get(char_pos.x, char_pos.y) == dim_clear)
		return true; 

	else
		return false; 

}

bool FogOfWar::IsFrontier(iPoint point, player_frontier& player)
{

	for (list<iPoint>::iterator it = player.frontier.begin(); it != player.frontier.end(); it++)
	{
		if (point == *it)
			return true;
	}

	return false;
}









