#ifndef __j1VIEWPORTS_H__
#define __j1VIEWPORTS_H__

#include "j1Module.h"
#include "j1Gui.h"
#include "Scene.h"
#include <vector>

struct SDL_Texture;
class MainScene;

// LayerBlit struct
struct layer_blit
{
	layer_blit() {};

	layer_blit(SDL_Texture* _texture, iPoint _pos, const SDL_Rect _section, int _viewport, float _scale, SDL_RendererFlip _flip, double _angle, int _pivot_x, int _pivot_y)
	{
		texture = _texture; pos = _pos; section.x = _section.x;  section.y = _section.y; section.w = _section.w; section.h = _section.h; scale = _scale;
		flip = _flip; angle = _angle; pivot_x = _pivot_x; pivot_y = _pivot_y; viewport = _viewport;
	};

	SDL_Texture*	 texture = nullptr;
	iPoint			 pos = NULLPOINT;
	SDL_Rect		 section = NULLRECT;
	float			 scale = 0.0f;
	SDL_RendererFlip flip;
	double           angle = 0;
	int              pivot_x = 0;
	int              pivot_y = 0;
	int				 viewport = 0;
};

struct layer_quad
{
	layer_quad() {};
	layer_quad(const SDL_Rect& _rect, Uint8 _r, Uint8 _g, Uint8 _b, Uint8 _a, bool _filled, bool _use_camera)
	{
		rect = { rect }; r = _r; g = _g; b = _b; a = _a; filled = _filled; use_camera = _use_camera;
	}
	SDL_Rect rect = NULLRECT;
	Uint8    r = 0;
	Uint8    g = 0;
	Uint8    b = 0;
	Uint8    a = 0;
	bool     filled;
	bool     use_camera = true;
};

struct layer_line
{
	layer_line() {};
	layer_line(int _x1, int _y1, int _x2, int _y2, Uint8 _r, Uint8 _g, Uint8 _b, Uint8 _a, bool _use_camera)
	{
		x1 = _x1; y1 = _y1; x2 = _x2; y2 = _y2; r = _r; g = _g; b = _b; a = _a; use_camera = _use_camera;
	}
	int   x1 = 0;
	int   y1 = 0;
	int   x2 = 0;
	int   y2 = 0;
	Uint8 r = 0;
	Uint8 g = 0;
	Uint8 b = 0;
	Uint8 a = 255; 
	bool  use_camera = true;
};

struct layer_circle
{
	layer_circle() {};
	layer_circle(int _x1, int _y1, int _redius, Uint8 _r, Uint8 _g, Uint8 _b, Uint8 _a, bool _use_camera)
	{
		x1 = _x1; y1 = _y1; redius = _redius; r = _r; g = _g; b = _b; a = _a; use_camera = _use_camera;
	}
	int   x1 = 0;
	int   y1 = 0;
	int   redius = 0;
	Uint8 r = 0;
	Uint8 g = 0;
	Uint8 b = 0;
	Uint8 a = 0;
	bool  use_camera = true;
};

class j1Viewports : public j1Module
{
public:

	j1Viewports();

	// Destructor
	virtual ~j1Viewports();

	// Called before render is available
	bool Awake();

	// Called before the first frame
	bool Start();

	// Called before all Updates
	bool PreUpdate();

	// Called each loop iteration
	bool Update(float dt);

	// Called before all Updates
	bool PostUpdate();

	// Called before quitting
	bool CleanUp();

	// Blit choosing the layer
	void LayerBlit(int layer, SDL_Texture* texture, iPoint pos, const SDL_Rect section = NULLRECT, int viewport = 0, float scale = -1.0f, SDL_RendererFlip _flip = SDL_FLIP_NONE, double angle = 0, int pivot_x = INT_MAX, int pivot_y = INT_MAX);
	void LayerDrawQuad(const SDL_Rect& rect, Uint8 r, Uint8 g, Uint8 b, Uint8 a, bool filled, bool use_camera = true);
	void LayerDrawLine(int x1, int y1, int x2, int y2, Uint8 r, Uint8 g, Uint8 b, Uint8 a = 255, bool use_camera = true);
	void LayerDrawCircle(int x1, int y1, int redius, Uint8 r, Uint8 g, Uint8 b, Uint8 a = 255, bool use_camera = true);
	void SetViews(uint number);
	uint GetViews();
	SDL_Rect GetViewportRect(uint viewport);

	void MoveCamera( int x, int y);
	void SetCamera(int x, int y);
	void CenterCamera( int x, int y);

	void OnCommand(std::list<std::string>& tokens);

	void OnCVar(std::list<std::string>& tokens);

	void SaveCVar(std::string& cvar_name, pugi::xml_node& node) const;

	// Win Size
	uint                 win_w = 0, win_h = 0;

private:
	// Blit in the layer order
	void DoLayerPrint();


public:
	iPoint				 camera1 = NULLPOINT;

private:
	// Layer Blit list
	vector<layer_quad>   quad_list;
	vector<layer_line>   line_list;
	vector<layer_circle> circle_list;

	p2PQueue<layer_blit> layer_list1;

	// Views
	uint				 number_of_views = 0;
	float				 scale = -1.0;

};

#endif // __j1VIEWPORTS_H__