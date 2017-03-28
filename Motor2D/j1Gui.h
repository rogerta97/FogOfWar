#ifndef __j1GUI_H__
#define __j1GUI_H__

#include "j1Module.h"
#include "j1Fonts.h"
#include "p2PQueue.h"
#include "j1Render.h"

// -----------------------------------------
// -----------------------------------------

                 // UI :D //

// -----------------------------------------
// -----------------------------------------

#define CURSOR_WIDTH 2

enum ui_element
{
	ui_window,
	ui_button,
	ui_text,
	ui_text_input,
	ui_image,
	ui_scroll_bar,
	ui_colored_rect,
	ui_element_null
};


// Class Gui -------------------------
// -----------------------------------

struct TTF_Font;
class UI_Element;
class UI_Window;
class UI_Text;

class j1Gui : public j1Module
{
public:

	j1Gui();

	// Destructor
	virtual ~j1Gui();

	// Called when before render is available
	bool Awake(pugi::xml_node&);

	// Call before first frame
	bool Start();

	bool Update(float dt);

	// Called before all Updates
	bool PreUpdate();

	// Called after all Updates
	bool PostUpdate();

	// Called before quitting
	bool CleanUp();

	// Gets the atlas texture
	const void GetAtlas() const;

	UI_Window* UI_CreateWin(iPoint pos, int w, int h, int blit = 0, bool dinamic = false, bool is_ui = true);

	void GetChilds(UI_Element * element, list<UI_Element*>& visited);
	void GetParentElements(UI_Element * element, list<UI_Element*>& visited);
	void ReorderElements();
	bool Move_Elements();
	UI_Element* CheckClickMove(int x, int y);
	void DeleteElement(UI_Element * element);

private:

public:
	// Atlas --
	SDL_Texture*		   atlas = nullptr;
	string			       atlas_file_name;
	// --------

	// All elements
	p2PQueue<UI_Element*>  elements_list;
	double				   higher_layer = 0;

	// Elements that can tab
	list<UI_Element*>      tab_list;

	// All windows
	list<UI_Window*>       windows;

	// Debug when F1
	bool				   debug = false;

private:
	bool				   start = true;

	// Movement
	bool				   moving = false;
	int					   mouse_x = 0;
	int					   mouse_y = 0;
	UI_Element*			   to_move = nullptr;
	//

	int					   camera_x = 0;
	int					   camera_y = 0;
};

// -------------------------
// ------------------------- Class Gui

// -----------------------------------
// Element ---------------------------

class UI_Element
{
public:
	UI_Element();
	~UI_Element();

	virtual bool update();

	virtual bool cleanup();

	// Enable function
	void SetEnabled(bool set);
	void SetEnabledAndChilds(bool set);

	// Child functions
	void AddChild(UI_Element* child);
	void AddChildBoth(UI_Element* child);

	// Mouse functions
	bool MouseClickEnterLeftIntern();
	bool MouseClickOutLeftIntern();

	void SetDebugColor(SDL_Color color);

	bool PutWindowToTop();

protected:

	// Helper functions
	int  CheckClickOverlap(int x, int y);
	bool CheckClickRect(int x, int y);

public:
	ui_element          type = ui_element_null;
	SDL_Rect            rect = NULLRECT;

	bool                print = true;
	bool				dinamic = false;
	bool				started_dinamic = false;
	bool				enabled = true;
	bool				click_through = false;
	bool				is_ui = true;

	// Layers --
	double				layer = 0;
	int					blit_layer = 0;
	list<UI_Element*>   childs;
	UI_Window*			parent = nullptr;
	UI_Element*			parent_element = nullptr;
	// ----------

	// Intern mouse control
	int					mouse_x = 0;
	int					mouse_y = 0;

protected:
	SDL_Color			color = NULLCOLOR;

private:
	bool				clicked = false;
};


// ---------------------------
// --------------------------- Element





// -----------------------------------
// Button ----------------------------

struct rect_text
{
	rect_text() {};
	rect_text(char* _name, SDL_Rect _rect) 
	{
		name = _name;
		rect = { _rect.x, _rect.y, _rect.w, _rect.h };
	}
	SDL_Rect rect = NULLRECT;
	string name;
};

class UI_Button : public UI_Element
{
public:
	UI_Button();

	~UI_Button();

	void Set(iPoint _pos, int w, int h);

	bool update();

	bool MouseEnter();
	bool MouseOut();
	bool MouseClickEnterLeft();
	bool MouseClickOutLeft();
	bool MouseClickEnterRight();
	bool MouseClickOutRight();

	void AddImage(char* name, SDL_Rect rect);
	void SetImage(char* name);

private:
	void ChangeButtonStats();

public:

private:
	list<rect_text>   rect_list;
	SDL_Rect		  curr = NULLRECT;

	bool			  to_enter = false;
	bool			  enter = false;

	bool			  to_clicked_left = false;
	bool			  clicked_left = false;

	bool			  to_clicked_right = false;
	bool			  clicked_right = false;
};

// ----------------------------
// ---------------------------- Button

// -----------------------------------
// Text ------------------------------

struct tex_str
{
	tex_str() {};
	tex_str(string _text, SDL_Texture* _texture){text = _text, texture = _texture;}
	SDL_Texture* texture = nullptr;
	string text;
};

class UI_Text : public UI_Element
{
public:
	UI_Text();
	~UI_Text();

	bool update();
	bool cleanup();

	void Set(iPoint pos, _TTF_Font* font, int spacing,  uint r = 255, uint g = 255, uint b = 255);
	void SetText(string text);
	string GetText();

public:
	list<tex_str>          tex_str_list;  
	SDL_Color	           color = NULLCOLOR;
	_TTF_Font*	           font = nullptr;
	int                    spacing = 0;
};

// ------------------------------
// ------------------------------ Text

// -----------------------------------
// Image -----------------------------

class UI_Image : public UI_Element
{
public:
	UI_Image();
	~UI_Image();

	bool update();

	void Set(iPoint pos, SDL_Rect image);

	void ChangeImage(SDL_Rect rect);

public:
	SDL_Rect image = NULLRECT;
};

// -----------------------------
// ----------------------------- Image

// -----------------------------------
// Text Input ------------------------

class UI_Text_Input : public UI_Element
{
public:
	UI_Text_Input();
	~UI_Text_Input();

	bool update();
	bool cleanup();

	void Set(iPoint pos, int w, _TTF_Font* font, uint r, uint g, uint b);

	void Clear();

	void SetTextInput(string text);

private:
	void SetIsActive();

	bool MouseClick();

	bool TakeInput();
	bool Delete();
	bool MoveCursor();

	void SetBarPos(string text);	
	int  GetTextSize(string text);
	void SetPasword();
	void ChangeTextInput();

	void DrawBar();
	void DinamicViewport();

public:
	string	     intern_text;
	bool		 pasword = false;
	UI_Text*	 text = nullptr;

	bool		 active = false;

private:
	// Bar control
	SDL_Rect	 bar = NULLRECT;
	uint		 bar_pos = 0;
	int	     	 bar_x = 0;
	int          text_offset = 0;

	// Camera
	SDL_Rect     camera_before = NULLRECT;

	// Change text manually
	string       text_change;
	bool         change = false;
};

// ------------------------
// ------------------------ Text Input

// -----------------------------------
// Scroll Bar ------------------------

class scroll_element
{
public:
	scroll_element() {};
	~scroll_element() {};

	bool operator == (scroll_element sc)
	{
		if (sc.element == element && sc.starting_pos_x == starting_pos_x && sc.starting_pos_y == starting_pos_y)
			return true; 
		return false;
	}

	UI_Element*  element = nullptr;
	int          starting_pos_x = 0;
	int          starting_pos_y = 0;
};

class UI_Scroll_Bar : public UI_Element
{
public:
	UI_Scroll_Bar();
	~UI_Scroll_Bar();

	void Set(iPoint pos, int w, int h, int button_size = 11);

	bool update();
	bool cleanup();

	void AddElement(UI_Element* element);
	void DeleteScrollElement(UI_Element* element);

	void ClearElements();

private:
	void ChangeHeightMovingRect();
	void ChangeWidthMovingRect();
	void MoveBarV();
	void MoveBarH();

public:
	// Vertical
	UI_Button*           button_v = nullptr;
	int                  min_bar_v = 0;
	int                  max_bar_v = 0;

	// Horizontal
	UI_Button*           button_h = nullptr;
	int                  min_bar_h = 0;
	int                  max_bar_h = 0;

	SDL_Rect             moving_rect = NULLRECT;

	list<scroll_element> elements;

	int                  starting_h = 0;
	int                  button_starting_h = 0;

	int                  starting_v = 0;
	int                  button_starting_v = 0;

private:
	// Movement
	int                 mouse_x = 0;
	int                 mouse_y = 0;
	bool                is_scrolling_v = false;
	bool                is_scrolling_h = false;
	int                 scroll_v = 0;
	int                 scroll_h = 0;
};

// ------------------------
// ------------------------ Scroll Bar

// -----------------------------------
// Colored Rect ----------------------

class UI_ColoredRect : public UI_Element
{
public:
	UI_ColoredRect();
	~UI_ColoredRect();

	void Set(iPoint pos, int w, int h, SDL_Color color, bool filled = true);

	bool update();

	void SetColor(SDL_Color color);

public:
	
private:
	SDL_Color color = NULLCOLOR;
	bool      filled = true;
};

// ----------------------
// ---------------------- Colored Rect

// -----------------------------------
// Window ----------------------------


class UI_Window : public UI_Element
{
public:
	UI_Window();
	~UI_Window();

	bool update();

	void Set(iPoint pos, int w, int h);

	UI_Button* CreateButton(iPoint _pos, int w, int h, bool dinamic = false);
	UI_Text* CreateText(iPoint pos, _TTF_Font* font, int spacing = 0, bool dinamic = false, uint r = 255, uint g = 255, uint b = 255);
	UI_Image* CreateImage(iPoint pos, SDL_Rect image, bool dinamic = false);
	UI_Text_Input* CreateTextInput(iPoint pos, int w, _TTF_Font* font, bool dinamic = false, uint r = 255, uint g = 255, uint b = 255);
	UI_Scroll_Bar* CreateScrollBar(iPoint pos, int view_w, int view_h, int button_size = 11, bool dinamic = false);
	UI_ColoredRect* CreateColoredRect(iPoint pos, int view_w, int view_h, SDL_Color color, bool filled = true, bool dinamic = false);

public:

private:

};

// ----------------------------
// ---------------------------- Window

#endif // !_j1GUI_H__