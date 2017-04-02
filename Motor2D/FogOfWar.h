
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

	void Update(vector<iPoint>& current_points); 

	~FogOfWar();

	bool AddPlayer(Player* new_entity);
	uint Get(int x, int y); 

	// Called at the beggining for knowing the first clear tiles 

	void GetEntitiesVisibleArea(int limit);

	// Called for modifying the position of the clear area (without BFS (opt.))

	void UpdateEntitiesVisibleArea();

	// This function updates the matrix in order to draw properly 

	void UpdateMatrix(); 

	// This is the core function of moving areas

	void MoveArea(int player_id, fow_directions axis, vector<iPoint>& current_points);

	vector<iPoint>		current_visited_points;

private:

	int					radium = FOW_RADIUM; 
	uint*				data; 
	vector<Player*>		players_on_fog;

	 

};

#endif