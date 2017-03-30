
#ifndef _FOGOFWAR_H_
#define	_FOGOFWAR_H_

#include "p2Defs.h"
#include <list>
#include <vector>
#include "Entity.h"
#include "Player.h"

#define FOW_RADIUM 4

class FogOfWar 
{
public:

	FogOfWar(); 

	~FogOfWar();

	bool AddPlayer(Player* new_entity);
	uint Get(int x, int y); 
	void GetEntitiesVisibleArea(vector<iPoint>& current_visited_points, int limit);

private:

	uint*				data; 

	list<Player*>		players_on_fog;
	vector<iPoint*>		current_visited_points; 

};

#endif