
#ifndef _FOGOFWAR_H_
#define	_FOGOFWAR_H_

#include "p2Defs.h"
#include <list>
#include <vector>
#include "Entity.h"
#include "Player.h"

#define FOW_RADIUM 4

// This is not necessary, but will make a more readable code and will simplify stuff
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



	// No mask 

	dim_clear,

};

enum fow_directions
{
	fow_up,
	fow_down, 
	fow_left, 
	fow_right
};

struct player_unit
{
	list<iPoint>	frontier; 

	list<iPoint>	current_points; 

	iPoint			player_pos; 
	int				id = -1; 
};

struct simple_player_unit
{
	iPoint player_pos; 
	bool visible = false; 
	int id = -1; 
};

class FogOfWar 
{
public:

	FogOfWar(); 

	void Start(); 

	void Update(iPoint prev_pos, iPoint next_pos); 

	~FogOfWar();

	bool AddPlayer(Player* new_entity);

	uint RemoveDimJaggies();
	void RemoveDarkJaggies();

	// Update Methods

	void MoveFrontier(iPoint prev_pos, const char* direction);

	void MoveArea(player_unit& player, string direction);

	SDL_Rect GetRect(int fow_id); 

	// Create Areas ---

	void FillFrontier();

	void GetEntitiesCircleArea(player_unit& new_player);
	void GetEntitiesRectangleFrontier(player_unit& new_player, int with, int height);

	void DeletePicks(player_unit& frontier);

	void GetCurrentPointsFromFrontier(player_unit& player);

	// Characters --

	void ManageCharacters();

	// Utilility --

	bool IsVisible(iPoint char_pos); 

	bool IsFrontier(iPoint point, player_unit& player);

	uint Get(int x, int y);

	// Atlas ---

	SDL_Texture*				fog_of_war_texture;

	list<simple_player_unit>	simple_char_on_fog_pos;

private:

	vector<player_unit>			players_on_fog;

	uint*						data;


	 

};

#endif