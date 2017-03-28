
#ifndef _FOGOFWAR_H_
#define	_FOGOFWAR_H_

#include "p2Defs.h"
#include <list>
#include <vector>
#include "Entity.h"

class FogOfWar 
{
public:

	FogOfWar(); 

	~FogOfWar();

	bool AddEntity(Entity* new_entity);
	uint Get(int x, int y); 
	void GetEntitiesVisibleArea(vector<iPoint*>& current_visited_points);

private:

	uint* data; 

	list<Entity*>		entities_on_fog; 
	vector<iPoint*>		current_visited_points; 

};

#endif