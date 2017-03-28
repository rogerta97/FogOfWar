#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1PathFinding.h"

j1PathFinding::j1PathFinding() : j1Module(), map(NULL), last_path(DEFAULT_PATH_LENGTH),width(0), height(0)
{
	name = "pathfinding";
}

// Destructor
j1PathFinding::~j1PathFinding()
{
	RELEASE_ARRAY(map);
}

// Called before quitting
bool j1PathFinding::CleanUp()
{
	LOG("Freeing pathfinding library");

	last_path.clear();
	RELEASE_ARRAY(map);
	return true;
}

// Sets up the walkability map
void j1PathFinding::SetMap(uint width, uint height, uchar* data)
{
	this->width = width;
	this->height = height;

	RELEASE_ARRAY(map);
	map = new uchar[width*height];
	memcpy(map, data, width*height);
}

// Utility: return true if pos is inside the map boundaries
bool j1PathFinding::CheckBoundaries(const iPoint& pos) const
{
	return (pos.x >= 0 && pos.x <= (int)width &&
			pos.y >= 0 && pos.y <= (int)height);
}

// Utility: returns true is the tile is walkable
bool j1PathFinding::IsWalkable(const iPoint& pos) const
{
	uchar t = GetTileAt(pos);
	return t != INVALID_WALK_CODE && t > 0;
}

// Utility: return the walkability value of a tile
uchar j1PathFinding::GetTileAt(const iPoint& pos) const
{
	if(CheckBoundaries(pos))
		return map[(pos.y*width) + pos.x];

	return INVALID_WALK_CODE;
}

// To request all tiles involved in the last generated path
const std::list<iPoint>* j1PathFinding::GetLastPath() const
{
	return &last_path;
}

// PathList ------------------------------------------------------------------------
// Looks for a node in this list and returns it's list node or NULL
// ---------------------------------------------------------------------------------
PathNode* PathList::Find(const iPoint& point) 
{
	for (std::list<PathNode*>::iterator it = list.begin(); it != list.end(); ++it) 
	{
		if ((*it)->pos == point) 
		{
			return *it;
		}
	}
	return nullptr;
}

// PathList ------------------------------------------------------------------------
// Returns the Pathnode with lowest score in this list or NULL if empty
// ---------------------------------------------------------------------------------
PathNode* PathList::GetNodeLowestScore()
{
	PathNode* ret = NULL;
	int min = 65535;

	for (std::list<PathNode*>::iterator it = list.begin(); it != list.end(); ++it)
	{
		if ((*it)->Score() < min)
		{
			min = (*it)->Score();
			ret = *it;
		}
	}

	return ret;
}

// PathNode -------------------------------------------------------------------------
// Convenient constructors
// ----------------------------------------------------------------------------------
PathNode::PathNode() : g(-1), h(-1), pos(-1, -1), parent(NULL)
{}

PathNode::PathNode(int g, int h, const iPoint& pos, const PathNode* parent) : g(g), h(h), pos(pos), parent(parent)
{}

PathNode::PathNode(const PathNode& node) : g(node.g), h(node.h), pos(node.pos), parent(node.parent)
{}

// PathNode -------------------------------------------------------------------------
// Fills a list (PathList) of all valid adjacent pathnodes
// ----------------------------------------------------------------------------------
uint PathNode::FindWalkableAdjacents(PathList& list_to_fill) const
{
	iPoint cell;
	uint before = list_to_fill.list.size();

	// north
	cell.create(pos.x, pos.y + 1);
	if(App->pathfinding->IsWalkable(cell))
		list_to_fill.list.push_back(new PathNode(-1, -1, cell, this));

	// south
	cell.create(pos.x, pos.y - 1);
	if(App->pathfinding->IsWalkable(cell))
		list_to_fill.list.push_back(new PathNode(-1, -1, cell, this));

	// east
	cell.create(pos.x + 1, pos.y);
	if(App->pathfinding->IsWalkable(cell))
		list_to_fill.list.push_back(new PathNode(-1, -1, cell, this));

	// west
	cell.create(pos.x - 1, pos.y);
	if(App->pathfinding->IsWalkable(cell))
		list_to_fill.list.push_back(new PathNode(-1, -1, cell, this));

	return list_to_fill.list.size();
}

// PathNode -------------------------------------------------------------------------
// Calculates this tile score
// ----------------------------------------------------------------------------------
int PathNode::Score() const
{
	return g + h;
}

// PathNode -------------------------------------------------------------------------
// Calculate the F for a specific destination tile
// ----------------------------------------------------------------------------------
int PathNode::CalculateF(const iPoint& destination)
{
	g = parent->g + 1;
	h = pos.DistanceTo(destination);

	return g + h;

}

// ----------------------------------------------------------------------------------
// Actual A* algorithm: return number of steps in the creation of the path or -1 ----
// ----------------------------------------------------------------------------------
int j1PathFinding::CreatePath(const iPoint& origin, const iPoint& destination)
{
	last_path.clear();
	int ret = -1;

	if (!IsWalkable(origin) || !IsWalkable(destination))
		return -1;

	PathList open;
	PathList close;
	PathNode origin_tile(0, origin.DistanceTo(destination), origin, nullptr);
	open.list.push_back(new PathNode(origin_tile));
	while (open.list.size() > 0) 
	{
		PathNode* next_tile = open.GetNodeLowestScore();
		close.list.push_back(new PathNode(*next_tile));


		if (close.Find(destination))
		{
			iPoint backtrack(destination);
			while (backtrack != origin)
			{
				last_path.push_front(backtrack);
				backtrack = close.Find(backtrack)->parent->pos;
			}
			last_path.push_front(backtrack);

			ret = last_path.size();

			//clean memory for open and close list
			for (std::list<PathNode*>::iterator curr_open = open.list.begin(); curr_open != open.list.end(); curr_open++)
			{
				RELEASE(*curr_open);
			}
			for (std::list<PathNode*>::iterator curr_close = close.list.begin(); curr_close != open.list.end(); curr_close++)
			{
				RELEASE(*curr_close);
			}

			open.list.clear();
			close.list.clear();

			break;
		}

		PathList adjacents;
		int walkables = next_tile->FindWalkableAdjacents(adjacents);

		for (std::list<PathNode*>::iterator curr_adjacent = adjacents.list.begin(); curr_adjacent != adjacents.list.end(); curr_adjacent++)
		{
			(*curr_adjacent)->parent = next_tile;

			if (close.Find((*curr_adjacent)->pos) != NULL)
				continue;
			if (open.Find((*curr_adjacent)->pos) == NULL) 
			{
				(*curr_adjacent)->CalculateF(destination);
				open.list.push_back(*curr_adjacent);
			}
			else
			{
				PathNode* old_node = open.Find((*curr_adjacent)->pos);
				if ((*curr_adjacent)->CalculateF(destination) < old_node->Score())
				{
					open.list.remove(old_node);
					open.list.push_back(*curr_adjacent);
				}
			}
		}

		open.list.remove(next_tile);
	}

	return ret;
}

