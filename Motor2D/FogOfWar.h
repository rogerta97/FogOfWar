
#ifndef _FOGOFWAR_H_
#define	_FOGOFWAR_H_

#include "p2Defs.h"
#include <list>
#include <vector>
#include "Entity.h"
#include "Player.h"

#define FOW_RADIUM 4

// This is not necessary, but will make a more readable code and will simplify stuff

enum fow_directions
{
	fow_up,
	fow_down, 
	fow_left, 
	fow_right
};

class FogOfWar 
{
public:

	FogOfWar(); 

	void Start(); 

	void Update(); 

	~FogOfWar();

	bool AddPlayer(Player* new_entity);
	uint Get(int x, int y); 

	// Called at the beggining for knowing the first clear tiles 

	void GetEntitiesVisibleArea();

	// This function will soft the edges

	uint RemoveJaggies();

	// Called for modifying the position of the clear area (without BFS (opt.))

	void MoveFrontier();

	// This function updates the matrix and the frontier in order to draw properly 

	void FillFrontier(); 

	// This is the core function of moving areas

	void MoveArea(int player_id, fow_directions axis);

	// This will get the rect corresponding to the tile 

	SDL_Rect GetRect(int fow_id); 

	// Check if one tile is the border for making it softer

	void DeletePicks();


	SDL_Texture*		fog_of_war_texture;

private:

	int	radium = FOW_RADIUM; 

	list<iPoint>		players_on_fog_pos;

	list<iPoint>		frontier; 

	vector<iPoint>		current_visited_points;
	uint*				data;


	 

};

#endif