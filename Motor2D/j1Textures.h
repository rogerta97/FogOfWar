#ifndef __j1TEXTURES_H__
#define __j1TEXTURES_H__

#include "j1Module.h"
#include <list>

struct SDL_Texture;
struct SDL_Surface;

struct Texture 
{
	SDL_Texture* tex = nullptr;
	string path;

	Texture();
	Texture(const char* path);
};

class j1Textures : public j1Module
{
public:

	j1Textures();

	// Destructor
	virtual ~j1Textures();

	// Called before render is available
	bool Awake(pugi::xml_node&);

	// Called before the first frame
	bool Start();

	// Called before quitting
	bool CleanUp();

	// Load Texture
	SDL_Texture* const	LoadTexture(const char* path);
	SDL_Surface* const  LoadSurface(const char* path);
	bool				UnLoadTexture(SDL_Texture* texture);
	SDL_Texture* const	SurfaceToTexture(SDL_Surface* surface);
	void				GetSize(const SDL_Texture* texture, uint& width, uint& height) const;

public:

	std::list<Texture*>	textures;
};


#endif // __j1TEXTURES_H__