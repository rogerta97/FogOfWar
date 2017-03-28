#include "Parallax.h"
#include "p2Log.h"
#include "j1Viewports.h"

#define CHUNKS 8

Parallax::Parallax(int _layer, int _chunks, fPoint _pos, SDL_Texture* _texture, SDL_Rect _rect, float _speed) 
	: layer(_layer), target(_pos), texture(_texture), speed(_speed)
{
	rect = { _rect.x, _rect.y, _rect.w, _rect.h };

	// Chunks always pair number!
	if (_chunks % 2 != 0)
		_chunks++;
	chunk_number = _chunks;
}

Parallax::~Parallax()
{
}

void Parallax::Update(float dt, fPoint target_pos)
{
	// Start
	if (start)
	{
		chunks.push_back({ target.x, target.y });

		float to_print = target.x + rect.w;
		for (int i = 0; i < chunk_number / 2; i++)
		{
			chunks.push_back({ to_print, target.y });
			to_print += rect.w;
		}

		to_print = target.x - rect.w;

		for (int i = 0; i < chunk_number / 2; i++)
		{
			chunks.push_back({ to_print, target.y });
			to_print -= rect.w;
		}

		distance = rect.w*chunk_number;
		target = target_pos;

		start = false;
	}

	// Move
	if (target_pos.x != target.x)
	{
		for(list<fPoint>::iterator it = chunks.begin(); it != chunks.end(); it++)
		{
			if (target_pos.x - target.x > 0)
				(*it).x += ((target_pos.x - target.x + speed) * dt);
			else
				(*it).x += ((target_pos.x - target.x - speed) * dt);
		}
	}

	// Create or delte
	for (list<fPoint>::iterator it = chunks.begin(); it != chunks.end(); it++)
	{
		// Delete left
		if ((*it).x + rect.w < target_pos.x - (distance*0.5f))
		{
			chunks.push_back({ GetRightChunk().x + rect.w, (*it).y });
			chunks.remove((*it));
		}

		// Delete right
		if ((*it).x + rect.w > target_pos.x + (distance*0.5f))
		{
			chunks.push_back({ GetLeftChunk().x - rect.w, (*it).y });
			chunks.remove((*it));
		}
	}

	// Print 
	for (list<fPoint>::iterator it = chunks.begin(); it != chunks.end(); it++)
		App->view->LayerBlit(layer, texture, { (int)floor((*it).x), (int)floor((*it).y) }, rect);

	target = target_pos;
}

fPoint Parallax::GetLeftChunk()
{
	fPoint ret;
	float smallest = chunks.front().x;

	for (list<fPoint>::iterator it = chunks.begin(); it != chunks.end(); it++)
	{
		if ((*it).x < smallest)
		{
			smallest = (*it).x;
			ret = (*it);
		}
	}

	return ret;
}

fPoint Parallax::GetRightChunk()
{
	fPoint ret;
	float highest = chunks.front().x;

	for (list<fPoint>::iterator it = chunks.begin(); it != chunks.end(); it++)
	{
		if ((*it).x > highest)
		{
			highest = (*it).x;
			ret = (*it);
		}
	}

	return ret;
}
