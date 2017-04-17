
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

enum fow_directions
{
	fow_up,
	fow_down, 
	fow_left, 
	fow_right
};

struct player_frontier
{
	list<iPoint>	frontier; 
	iPoint			player_pos; 
};

class FogOfWar 
{
public:

	FogOfWar(); 

	void Start(); 

	void Update(iPoint prev_pos); 

	~FogOfWar();

	bool AddPlayer(Player* new_entity);
	uint Get(int x, int y); 

	// Called at the beggining for knowing the first clear tiles 

	void GetEntitiesVisibleArea(player_frontier& new_player);

	// This function will soft the edges

	uint RemoveDimJaggies();
	void RemoveDarkJaggies(iPoint curr); 

	// Called for modifying the position of the clear area (without BFS (opt.))

	void MoveFrontier(iPoint prev_pos);

	// This function updates the matrix and the frontier in order to draw properly 

	void FillFrontier(); 

	// This is the core function of moving areas

	void MoveArea(player_frontier& player_id, string direction);

	// This will get the rect corresponding to the tile 

	SDL_Rect GetRect(int fow_id); 

	// Check if one tile is the border for making it softer

	void DeletePicks(player_frontier& frontier);

	//This will switch between the players that has fog of war activated

	void ChangeCharacter(iPoint prev_pos);

	// This will hide the characters or not 

	void ManageCharacters();

	bool IsVisible(iPoint char_pos); 


	SDL_Texture*				fog_of_war_texture;

	Player*						curr_character = nullptr; 

private:

	vector<player_frontier>		players_on_fog;

	list<iPoint>				simple_char_on_fog_pos; 

	uint*				data;


	 

};

#endif