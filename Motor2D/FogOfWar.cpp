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
	// Dim 

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

	// Dark->clear 

	darkc_middle,
	darkc_left,
	darkc_right,
	darkc_up,
	darkc_down,
	darkc_bottom_right,
	darkc_bottom_left,
	darkc_top_right,
	darkc_top_left,
	darkc_inner_top_left,
	darkc_inner_top_right,
	darkc_inner_bottom_left,
	darkc_inner_bottom_right,

	// Dark->dim

	darkd_middle,
	darkd_left,
	darkd_right,
	darkd_up,
	darkd_down,
	darkd_bottom_right,
	darkd_bottom_left,
	darkd_top_right,
	darkd_top_left,
	darkd_inner_top_left,
	darkd_inner_top_right,
	darkd_inner_bottom_left,
	darkd_inner_bottom_right,

	// No mask 

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
	FillFrontier(); 
	RemoveDimJaggies();
}

void FogOfWar::Update()
{
	MoveFrontier();
	FillFrontier();
	RemoveDimJaggies(); 
}

void FogOfWar::GetEntitiesVisibleArea()
{
	player_frontier frontier; 

	// We BFS the first time for knowing the tiles we must blit! 
	for (list<iPoint>::iterator it = players_on_fog_pos.begin(); it != players_on_fog_pos.end(); it++)
	{
		frontier.data = App->map->PropagateBFS(iPoint((*it).x, (*it).y), FOW_RADIUM);

		// Passing each visible area to the total amount of visible tiles (if they are not yet)

		DeletePicks(frontier.data, 0); 

		for (list<iPoint>::iterator it = frontier.data.begin(); it != frontier.data.end(); it++)
			data[(*it).y * App->map->data.width + (*it).x] = dim_clear;	

		frontiers.push_back(frontier); 
	}
	 
}

uint FogOfWar::RemoveDimJaggies()
{
	for (vector<player_frontier>::iterator curr = frontiers.begin(); curr != frontiers.end(); curr++)
	{
		for (list<iPoint>::iterator it = curr->data.begin(); it != curr->data.end(); it++)
		{
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

					if (Get(((*it).x + 1), (*it).y - 1) != dim_middle)
						data[((*it).y)*App->map->data.width + ((*it).x + 1)] = dim_top_left;
				}

				else if (Get((*it).x, (*it).y + 1) == dim_middle || Get((*it).x, (*it).y + 1) == fow_null)
				{
					data[((*it).y)*App->map->data.width + (*it).x] = dim_inner_bottom_left;

					if (Get(((*it).x + 1), (*it).y + 1) != dim_middle && Get((*it).x + 1, (*it).y + 1) != fow_null)
						data[((*it).y)*App->map->data.width + ((*it).x + 1)] = dim_bottom_left;
				}

			}
			else if (Get((*it).x + 1, (*it).y) == dim_middle || Get((*it).x + 1, (*it).y) == fow_null)
			{
				data[(*it).y*App->map->data.width + ((*it).x)] = dim_right;

				if (Get((*it).x, (*it).y - 1) == dim_middle || Get((*it).x, (*it).y - 1) == fow_null)
				{
					data[((*it).y)*App->map->data.width + (*it).x] = dim_inner_top_right;

					if (Get(((*it).x - 1), (*it).y - 1) != dim_middle)
						data[((*it).y)*App->map->data.width + ((*it).x - 1)] = dim_top_right;
				}

				else if (Get((*it).x, (*it).y + 1) == dim_middle || Get((*it).x, (*it).y + 1) == fow_null)
				{
					data[((*it).y)*App->map->data.width + (*it).x] = dim_inner_bottom_right;

					if (Get(((*it).x - 1), (*it).y + 1) != dim_middle && Get(((*it).x - 1), (*it).y + 1) != fow_null)
						data[((*it).y)*App->map->data.width + ((*it).x - 1)] = dim_bottom_right;
				}


			}
		}
	}

		

 return 0; 

}

void FogOfWar::RemoveDarkJaggies(iPoint curr)
{






//	// Soft from black area to dim area 
//	if (data[curr.y*App->map->data.width + curr.x] == fow_null && data[(curr.y + 1)*App->map->data.width + curr.x] == dim_middle)
//	{
//		data[(curr.y + 1)*App->map->data.width + curr.x] = darkd_up;
//	}
//
//	if (data[curr.y*App->map->data.width + curr.x] == fow_null && data[(curr.y - 1)*App->map->data.width + curr.x] == dim_middle)
//	{
//		data[(curr.y - 1)*App->map->data.width + curr.x] = darkd_down; 
//	}
//
//	if (data[curr.y*App->map->data.width + curr.x] == fow_null && data[curr.y*App->map->data.width + curr.x + 1] == dim_middle)
//	{
//		curr.x++; 
//		if (data[curr.y*App->map->data.width + curr.x] != dim_clear)
//		{
//			data[curr.y*App->map->data.width + curr.x] = darkd_left;
//
//			if(data[(curr.y - 1)*App->map->data.width + curr.x] == fow_null)
//			{
//				data[curr.y*App->map->data.width + curr.x] = darkd_inner_top_left;
//
//				if(data[(curr.y - 1)*App->map->data.width + curr.x + 1] != fow_null)
//					data[curr.y*App->map->data.width + curr.x + 1] = darkd_top_left;
//			}
//
//			else if (data[(curr.y + 1)*App->map->data.width + curr.x] == fow_null)
//			{
//				data[curr.y*App->map->data.width + curr.x] = darkd_inner_bottom_left;
//
//				if (data[(curr.y + 1)*App->map->data.width + curr.x + 1] != fow_null)
//					data[curr.y*App->map->data.width + curr.x + 1] = darkd_bottom_left;
//			}		
//		}
//	}
//	else if (data[curr.y*App->map->data.width + curr.x] == fow_null && data[curr.y*App->map->data.width + curr.x - 1] != fow_null)
//	{
//		curr.x--; 
//		if (data[curr.y*App->map->data.width + curr.x] != dim_clear)
//		{
//			data[curr.y*App->map->data.width + curr.x] = darkd_right;
//
//			if (data[(curr.y - 1)*App->map->data.width + curr.x] == fow_null)
//			{
//				data[curr.y*App->map->data.width + curr.x] = darkd_inner_top_right;
//
//				if (data[(curr.y - 1)*App->map->data.width + curr.x - 1] != fow_null)
//					data[curr.y*App->map->data.width + curr.x - 1] = darkd_top_right;
//			}
//
//			else if (data[(curr.y + 1)*App->map->data.width + curr.x] == fow_null)
//			{
//				data[curr.y*App->map->data.width + curr.x] = darkd_inner_bottom_right;
//
//				if (data[(curr.y + 1)*App->map->data.width + curr.x - 1] != fow_null)
//					data[curr.y*App->map->data.width + curr.x - 1] = darkd_bottom_right;
//			}
//		}
//	}
//
//	// ----
//
//
//	// Soft from black area to clear area 
//
//	if (data[curr.y*App->map->data.width + curr.x] == fow_null && data[curr.y*App->map->data.width + curr.x + 1] == dim_clear)
//	{
//		curr.x++;
//	
//		data[curr.y*App->map->data.width + curr.x] = darkc_left;
//
//			if (data[(curr.y - 1)*App->map->data.width + curr.x] == fow_null)
//			{
//				data[curr.y*App->map->data.width + curr.x] = darkc_inner_top_left;
//
//				if (data[(curr.y - 1)*App->map->data.width + curr.x + 1] != fow_null)
//					data[curr.y*App->map->data.width + curr.x + 1] = darkc_top_left;
//			}
//
//			else if (data[(curr.y + 1)*App->map->data.width + curr.x] == fow_null)
//			{
//				data[curr.y*App->map->data.width + curr.x] = darkc_inner_bottom_left;
//
//				if (data[(curr.y + 1)*App->map->data.width + curr.x + 1] != fow_null)
//					data[curr.y*App->map->data.width + curr.x + 1] = darkc_bottom_left;
//			}
//		
//	}
//	else if (data[curr.y*App->map->data.width + curr.x] == fow_null && data[curr.y*App->map->data.width + curr.x - 1] != fow_null)
//	{
//		curr.x--;
//		
//		data[curr.y*App->map->data.width + curr.x] = darkc_right;
//
//			if (data[(curr.y - 1)*App->map->data.width + curr.x] == fow_null)
//			{
//				data[curr.y*App->map->data.width + curr.x] = darkc_inner_top_right;
//
//				if (data[(curr.y - 1)*App->map->data.width + curr.x - 1] != fow_null)
//					data[curr.y*App->map->data.width + curr.x - 1] = darkc_top_right;
//			}
//
//			else if (data[(curr.y + 1)*App->map->data.width + curr.x] == fow_null)
//			{
//				data[curr.y*App->map->data.width + curr.x] = darkc_inner_bottom_right;
//
//				if (data[(curr.y + 1)*App->map->data.width + curr.x - 1] != fow_null)
//					data[curr.y*App->map->data.width + curr.x - 1] = darkc_bottom_right;
//		}
//		
//	}
//
//
}

void FogOfWar::MoveFrontier()
{
	int id = 0; 

	string direction = App->scene->main_scene->player->player_go->animator->GetCurrentAnimation()->GetName();

	for (list<iPoint>::iterator it = players_on_fog_pos.begin(); it != players_on_fog_pos.end(); it++)
	{
		if (direction == "run_down")
			MoveArea(frontiers.at(id).data, fow_down);

		else if (direction == "run_up")
			MoveArea(frontiers.at(id).data, fow_up);

		else if (direction == "run_lateral" && App->scene->main_scene->player->flip == true)
			MoveArea(frontiers.at(id).data, fow_left);

		else if (direction == "run_lateral")
			MoveArea(frontiers.at(id).data, fow_right);

		id++; 
	}
}

void FogOfWar::FillFrontier()
{

	for (list<iPoint>::iterator it = players_on_fog_pos.begin(); it != players_on_fog_pos.end(); it++)
	{

	iPoint ini_p = iPoint(it->x, it->y);
	ini_p.y -= radium + 1; ini_p.x -= radium + 1;

	iPoint end_p = iPoint(it->x, it->y);
	end_p.y += radium + 2; end_p.x += radium + 2;

	
		for (int y = ini_p.y; y < end_p.y; y++)
		{
			for (int x = ini_p.x; x < end_p.x; x++)
			{
				if (iPoint(x, y).DistanceTo(*it) < FOW_RADIUM - 1)
				{
					data[y*App->map->data.width + x] = dim_clear;
				}

				//RemoveDarkJaggies({ x,y });

			}
		}
	}
			
}

void FogOfWar::MoveArea(list<iPoint>& frontier, fow_directions direction)
{
	for (list<iPoint>::iterator it = frontier.begin(); it != frontier.end(); it++)
		data[(*it).y * App->map->data.width + (*it).x] = dim_middle;
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

	for (list<iPoint>::iterator it = frontier.begin(); it != frontier.end(); it++)
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

void FogOfWar::DeletePicks(list<iPoint>& frontier, int player_id)
{
	int count = 0;
	iPoint player_pos; 

	for (list<iPoint>::iterator it = players_on_fog_pos.begin(); it != players_on_fog_pos.end(); it++)
	{
		if(count++ == player_id)
			player_pos = *it;
	}

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

}


