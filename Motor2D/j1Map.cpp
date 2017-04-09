#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Scene.h"
#include "MainScene.h"
#include "j1Render.h"
#include "j1FileSystem.h"
#include "FogOfWar.h"
#include "j1Textures.h"
#include "j1Map.h"
#include "j1Viewports.h"
#include <math.h>
#include <cmath>
#include <queue>
#include "j1Window.h"
#include "Functions.h"

j1Map::j1Map() : j1Module(), map_loaded(false)
{
	name = "map";
}

// Destructor
j1Map::~j1Map()
{}

// Called before render is available
bool j1Map::Awake(pugi::xml_node& config)
{
	LOG("Loading Map Parser");
	bool ret = true;

	folder = config.child("folder").child_value();
	draw_margin = config.child("draw_margin").attribute("value").as_int();

	offset = 0;

	fit_square.x = fit_square.y = 0;
	uint w, h;
	App->win->GetWindowSize(w, h);
	fit_square.w = w;
	fit_square.h = h;

	return ret;
}

void j1Map::Draw()
{
	if(map_loaded == false)
		return;

	for (int i = 1; i <= App->view->GetViews(); i++) {
		fit_square = App->view->GetViewportRect(i);
		for (std::list<MapLayer*>::iterator item = data.layers.begin(); item != data.layers.end(); item++)
		{
			MapLayer* layer = *item;

			if (layer->properties.Get("Nodraw") != 0)
				continue;

			int x_ini = 0, x_end = data.width;
			TilesToDraw_x(x_ini, x_end, *item);
			int count = 0;

			for (int x = x_ini; x < x_end + 1; ++x)
			{
				int y_ini, y_end;
				TilesToDraw_y(count, x, x_end, y_ini, y_end, *item);

				for (int y = y_ini; y < y_end + 1; ++y)
				{
					int tile_id = layer->Get(x, y);

					int visibility = App->scene->main_scene->fog_of_war->Get(x,y);

					if (tile_id > 0 && visibility != 0)
					{
						TileSet* tileset = GetTilesetFromTileId(tile_id);

						SDL_Rect r = tileset->GetTileRect(tile_id);
						iPoint pos = MapToWorld(x, y);

						App->view->LayerBlit(1, tileset->texture, pos, r, i);

						if(visibility != 18)
						{

							r = App->scene->main_scene->fog_of_war->GetRect(visibility);
							App->view->LayerBlit(1, App->scene->main_scene->fog_of_war->fog_of_war_texture, pos, r, i);
						}
											
					}
				}
				count++;
			}
		}
	}
}

int Properties::Get(const char* value, int default_value) const
{
	for (std::list<Property*>::const_iterator item = list.begin(); item != list.end(); ++item)
	{
		if ((*item)->name == value)
			return (*item)->value;
	}

	return default_value;
}

TileSet* j1Map::GetTilesetFromTileId(int id) const
{
	std::list<TileSet*>::const_iterator item = data.tilesets.begin();
	TileSet* set = *item;

	while(item != data.tilesets.end())
	{
		if(id < (*item)->firstgid)
		{
			set = *(--item);
			break;
		}
		set = *item;
		item++;
	}

	return set;
}

iPoint j1Map::MapToWorld(int x, int y) const
{
	iPoint ret;

	if(data.type == MAPTYPE_ORTHOGONAL)
	{
		ret.x = x * data.tile_width;
		ret.y = y * data.tile_height;
	}
	else if(data.type == MAPTYPE_ISOMETRIC)
	{
		ret.x = (x - y) * (data.tile_width * 0.5f);
		ret.y = (x + y) * (data.tile_height * 0.5f);
	}
	else
	{
		LOG("Unknown map type");
		ret.x = x; ret.y = y;
	}

	return ret;
}

iPoint j1Map::WorldToMap(int x, int y) const
{
	iPoint ret(0,0);

	if(data.type == MAPTYPE_ORTHOGONAL)
	{
		ret.x = x / data.tile_width;
		ret.y = y / data.tile_height;
	}
	else if(data.type == MAPTYPE_ISOMETRIC)
	{
		
		float half_width = data.tile_width * 0.5f;
		float half_height = data.tile_height * 0.5f;
		ret.x = int( (x / half_width + y / half_height) / 2) - 1;
		ret.y = int( (y / half_height - (x / half_width)) / 2);
	}
	else
	{
		LOG("Unknown map type");
		ret.x = x; ret.y = y;
	}

	return ret;
}

SDL_Rect TileSet::GetTileRect(int id) const
{
	int relative_id = id - firstgid;
	SDL_Rect rect;
	rect.w = tile_width;
	rect.h = tile_height;
	rect.x = margin + ((rect.w + spacing) * (relative_id % num_tiles_width));
	rect.y = margin + ((rect.h + spacing) * (relative_id / num_tiles_width));
	return rect;
}

// Called before quitting
bool j1Map::CleanUp()
{
	LOG("Unloading map");

	// Remove all tilesets
	std::list<TileSet*>::iterator item;
	item = data.tilesets.begin();

	while(item != data.tilesets.end())
	{
		RELEASE(*item);
		item++;
	}
	data.tilesets.clear();

	// Remove all layers
	std::list<MapLayer*>::iterator item2;
	item2 = data.layers.begin();

	while(item2 != data.layers.end())
	{
		RELEASE(*item2);
		item2++;
	}
	data.layers.clear();

	// Clean up the pugui tree
	map_file.reset();

	return true;
}

// Load new map
bool j1Map::Load(const char* file_name)
{
	bool ret = true;
	p2SString tmp("%s%s", folder.c_str(), file_name);

	char* buf;
	int size = App->fs->Load(tmp.GetString(), &buf);
	pugi::xml_parse_result result = map_file.load_buffer(buf, size);

	RELEASE(buf);

	if(result == NULL)
	{
		LOG("Could not load map xml file %s. pugi error: %s", file_name, result.description());
		ret = false;
	}

	// Load general info ----------------------------------------------
	if(ret == true)
	{
		ret = LoadMap();
	}

	// Load all tilesets info ----------------------------------------------
	pugi::xml_node tileset;
	for(tileset = map_file.child("map").child("tileset"); tileset && ret; tileset = tileset.next_sibling("tileset"))
	{
		TileSet* set = new TileSet();

		if(ret == true)
		{
			ret = LoadTilesetDetails(tileset, set);
		}

		if(ret == true)
		{
			ret = LoadTilesetImage(tileset, set);
		}

		data.tilesets.push_back(set);
	}

	// Load layer info ----------------------------------------------
	pugi::xml_node layer;
	for(layer = map_file.child("map").child("layer"); layer && ret; layer = layer.next_sibling("layer"))
	{
		MapLayer* lay = new MapLayer();

		ret = LoadLayer(layer, lay);

		if(ret == true)
			data.layers.push_back(lay);
	}

	if(ret == true)
	{
		LOG("Successfully parsed map XML file: %s", file_name);
		LOG("width: %d height: %d", data.width, data.height);
		LOG("tile_width: %d tile_height: %d", data.tile_width, data.tile_height);

		std::list<TileSet*>::iterator item = data.tilesets.begin();
		while(item != data.tilesets.end())
		{
			TileSet* s = *item;
			LOG("Tileset ----");
			LOG("name: %s firstgid: %d", s->name.c_str(), s->firstgid);
			LOG("tile width: %d tile height: %d", s->tile_width, s->tile_height);
			LOG("spacing: %d margin: %d", s->spacing, s->margin);
			item++;
		}

		std::list<MapLayer*>::iterator item_layer = data.layers.begin();
		while(item_layer != data.layers.end())
		{
			MapLayer* l = *item_layer;
			LOG("Layer ----");
			LOG("name: %s", l->name.c_str());
			LOG("tile width: %d tile height: %d", l->width, l->height);
			item_layer++;
		}
	}

	map_loaded = ret;


	return ret;
}

// Load map general properties
bool j1Map::LoadMap()
{
	bool ret = true;
	pugi::xml_node map = map_file.child("map");

	if(map == NULL)
	{
		LOG("Error parsing map xml file: Cannot find 'map' tag.");
		ret = false;
	}
	else
	{
		data.width = map.attribute("width").as_int();
		data.height = map.attribute("height").as_int();
		data.tile_width = map.attribute("tilewidth").as_int();
		data.tile_height = map.attribute("tileheight").as_int();
		std::string bg_color(map.attribute("backgroundcolor").as_string());

		data.background_color.r = 0;
		data.background_color.g = 0;
		data.background_color.b = 0;
		data.background_color.a = 0;

		if(bg_color.length() > 0)
		{
			std::string red, green, blue;
			red = bg_color.substr(1, 2);
			green = bg_color.substr(3, 2);
			blue = bg_color.substr(5, 2);

			int v = 0;

			sscanf_s(red.c_str(), "%x", &v);
			if(v >= 0 && v <= 255) data.background_color.r = v;

			sscanf_s(green.c_str(), "%x", &v);
			if(v >= 0 && v <= 255) data.background_color.g = v;

			sscanf_s(blue.c_str(), "%x", &v);
			if(v >= 0 && v <= 255) data.background_color.b = v;
		}

		std::string orientation(map.attribute("orientation").as_string());

		if(orientation == "orthogonal")
		{
			data.type = MAPTYPE_ORTHOGONAL;
		}
		else if(orientation == "isometric")
		{
			data.type = MAPTYPE_ISOMETRIC;
		}
		else if(orientation == "staggered")
		{
			data.type = MAPTYPE_STAGGERED;
		}
		else
		{
			data.type = MAPTYPE_UNKNOWN;
		}
	}

	return ret;
}

bool j1Map::LoadTilesetDetails(pugi::xml_node& tileset_node, TileSet* set)
{
	bool ret = true;
	set->name = (tileset_node.attribute("name").as_string());
	set->firstgid = tileset_node.attribute("firstgid").as_int();
	set->tile_width = tileset_node.attribute("tilewidth").as_int();
	set->tile_height = tileset_node.attribute("tileheight").as_int();
	set->margin = tileset_node.attribute("margin").as_int();
	set->spacing = tileset_node.attribute("spacing").as_int();
	pugi::xml_node offset = tileset_node.child("tileoffset");

	if(offset != NULL)
	{
		set->offset_x = offset.attribute("x").as_int();
		set->offset_y = offset.attribute("y").as_int();
	}
	else
	{
		set->offset_x = 0;
		set->offset_y = 0;
	}

	return ret;
}

bool j1Map::LoadTilesetImage(pugi::xml_node& tileset_node, TileSet* set)
{
	bool ret = true;
	pugi::xml_node image = tileset_node.child("image");

	if(image == NULL)
	{
		LOG("Error parsing tileset xml file: Cannot find 'image' tag.");
		ret = false;
	}
	else
	{
		set->texture = App->tex->LoadTexture(PATH(folder.c_str(), image.attribute("source").as_string()));
		int w, h;
		SDL_QueryTexture(set->texture, NULL, NULL, &w, &h);
		set->tex_width = image.attribute("width").as_int();

		if(set->tex_width <= 0)
		{
			set->tex_width = w;
		}

		set->tex_height = image.attribute("height").as_int();

		if(set->tex_height <= 0)
		{
			set->tex_height = h;
		}

		set->num_tiles_width = set->tex_width / set->tile_width;
		set->num_tiles_height = set->tex_height / set->tile_height;
	}

	return ret;
}

bool j1Map::LoadLayer(pugi::xml_node& node, MapLayer* layer)
{
	bool ret = true;

	layer->name = node.attribute("name").as_string();
	layer->width = node.attribute("width").as_int();
	layer->height = node.attribute("height").as_int();
	LoadProperties(node, layer->properties);
	pugi::xml_node layer_data = node.child("data");

	if(layer_data == NULL)
	{
		LOG("Error parsing map xml file: Cannot find 'layer/data' tag.");
		ret = false;
		RELEASE(layer);
	}
	else
	{
		layer->data = new uint[layer->width*layer->height];
		memset(layer->data, 0, layer->width*layer->height);

		if (TextCmp(layer_data.attribute("encoding").as_string("no_enc"), "csv"))
		{
			std::string data = layer_data.child_value();

			std::list<std::string> tiles;
			Tokenize(data, ',', tiles);

			int i = 0;
			for (std::list<std::string>::iterator tile = tiles.begin(); tile != tiles.end(); tile++)
			{
				if(*tile != "")
					layer->data[i++] = atoi(tile->c_str());
			}
		}
		else
		{
			int i = 0;
			for (pugi::xml_node tile = layer_data.child("tile"); tile; tile = tile.next_sibling("tile"))
			{
				layer->data[i++] = tile.attribute("gid").as_int(0);
			}
		}
	}

	return ret;
}

// Load a group of properties from a node and fill a list with it
bool j1Map::LoadProperties(pugi::xml_node& node, Properties& properties)
{
	bool ret = false;

	pugi::xml_node data = node.child("properties");

	if(data != NULL)
	{
		pugi::xml_node prop;

		for(prop = data.child("property"); prop; prop = prop.next_sibling("property"))
		{
			Properties::Property* p = new Properties::Property();

			p->name = prop.attribute("name").as_string();
			p->value = prop.attribute("value").as_int();

			properties.list.push_back(p);
		}
	}

	return ret;
}

void j1Map::TilesToDraw_x(int & x_ini, int & x_end, MapLayer * layer) const
{
	iPoint ini = WorldToMap(fit_square.x, fit_square.y);
	iPoint end = WorldToMap(fit_square.x + fit_square.w, fit_square.y + fit_square.h);
	switch (data.type)
	{
	case MAPTYPE_UNKNOWN:
		break;
	case MAPTYPE_ORTHOGONAL:
		x_ini = (ini.x - draw_margin > 0) ? (ini.x - draw_margin) : 0;
		x_end = (end.x + draw_margin < layer->width) ? (end.x + draw_margin) : layer->width;
		break;
	case MAPTYPE_ISOMETRIC:
		x_ini = (ini.x - draw_margin > 0) ? (ini.x - draw_margin) : 0;
		x_end = (end.x + draw_margin < layer->width) ? (end.x + draw_margin) : layer->width;
		break;
	case MAPTYPE_STAGGERED:
		break;
	default:
		break;
	}
}

void j1Map::TilesToDraw_y(int count, int x, int x_end, int & y_ini, int & y_end, MapLayer * layer) const
{
	iPoint top_right = WorldToMap(fit_square.x + fit_square.w, fit_square.y);
	iPoint top_left = WorldToMap(fit_square.x, fit_square.y);
	iPoint bot_right = WorldToMap(fit_square.x + fit_square.w, fit_square.y + fit_square.h);
	iPoint bot_left = WorldToMap(fit_square.x, fit_square.y + fit_square.h);
	switch (data.type)
	{
	case MAPTYPE_UNKNOWN:
		break;
	case MAPTYPE_ORTHOGONAL:
		y_ini = (top_left.y - draw_margin > 0) ? (top_left.y - draw_margin) : 0;
		y_end = (bot_right.y + draw_margin < layer->height) ? bot_right.y + draw_margin : layer->height;
		break;
	case MAPTYPE_ISOMETRIC: {
		float screen_prop = (float)fit_square.w / (float)fit_square.h;
		int diag1 = bot_left.x, diag2 = top_right.x;
		if (x <= diag1) {
			y_ini = top_left.y - (count + 1);
			y_end = top_left.y + count + 2;
			if (top_left.x <= 0) {
				y_ini += (top_left.x - 3);
				y_end -= (top_left.x - 3);
			}
		}
		else if (x > diag1 && x < diag2) {
			int diff = x - diag1;
			y_ini = top_left.y - count - diff;
			y_end = top_left.y + diff;
		}
		else {
			int rev_count = bot_right.x - x;
			y_ini = bot_right.y - rev_count;
			y_end = bot_right.y + (rev_count + 1);
		}

		y_ini -= draw_margin;
		y_end += draw_margin;

		if (y_ini < 0) y_ini = 0;
		if (y_end > layer->height) y_end = layer->height;

		break;
	}
	case MAPTYPE_STAGGERED:
		break;
	default:
		break;
	}
}

bool j1Map::CreateWalkabilityMap(int& width, int& height, uchar** buffer) const
{
	bool ret = false;
	std::list<MapLayer*>::const_iterator item;
	item = data.layers.begin();

	for(; item != data.layers.end(); item++)
	{
		MapLayer* layer = *item;

		if(layer->properties.Get("Navigation", 0) == 0)
			continue;

		uchar* map = new uchar[layer->width*layer->height];
		memset(map, 1, layer->width*layer->height);

		for(int y = 0; y < data.height; ++y)
		{
			for(int x = 0; x < data.width; ++x)
			{
				int i = (y*layer->width) + x;

				int tile_id = layer->Get(x, y);
				TileSet* tileset = (tile_id > 0) ? GetTilesetFromTileId(tile_id) : NULL;
				
				if(tileset != NULL)
				{
					map[i] = (tile_id - tileset->firstgid) > 0 ? 0 : 1;
					/*TileType* ts = tileset->GetTileType(tile_id);
					if(ts != NULL)
					{
						map[i] = ts->properties.Get("walkable", 1);
					}*/
				}
			}
		}

		*buffer = map;
		width = data.width;
		height = data.height;
		ret = true;

		break;
	}

	return ret;
}

list<iPoint> j1Map::PropagateBFS(iPoint origin, vector<iPoint>& seen_nodes, int field_of_view)
{

	list<iPoint>		frontier; 
	vector<iPoint>		visited; 

	frontier.push_back(origin); 
	visited.push_back(origin); 

	int count = 0; 

	int current_layer = 0; 
	int layer_done = 4;

	while(current_layer < field_of_view)
	{
		iPoint curr = frontier.front(); 
		bool is_on_list = false;

		if (curr != iPoint(0,0))
		{
			iPoint neighbors[4];
			neighbors[0].create(curr.x + 1, curr.y);
			neighbors[1].create(curr.x, curr.y + 1);
			neighbors[2].create(curr.x - 1, curr.y);
			neighbors[3].create(curr.x, curr.y - 1);

			frontier.pop_front();

			for (uint i = 0; i < 4; i++)
			{		
				for(vector<iPoint>::const_iterator it = visited.cbegin(); it != visited.cend(); it++)
				{
					is_on_list = false;

					if (neighbors[i] == *it)
					{
						is_on_list = true;
						break;
					}						
				}

				if (!is_on_list)
				{
					frontier.push_back(neighbors[i]);
					visited.push_back(neighbors[i]);
					count++;
				}				
			}			
		}

		if (count == layer_done)
		{
			layer_done = layer_done + 4;
			count = 0;
			current_layer++;
		}
	}

	seen_nodes = visited; 
	return frontier; 

	}
	
