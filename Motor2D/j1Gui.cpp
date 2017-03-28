#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Render.h"
#include "j1Textures.h"
#include "j1Fonts.h"
#include "j1Input.h"
#include "j1Gui.h"
#include "Functions.h"

#include <iostream> 
#include <sstream> 

// Class Gui -------------------------
// -----------------------------------

j1Gui::j1Gui() : j1Module()
{
	name = "gui";
}

// Destructor
j1Gui::~j1Gui()
{}

// Called before render is available
bool j1Gui::Awake(pugi::xml_node& conf)
{
	LOG("Loading GUI atlas");
	bool ret = true;

	atlas_file_name = conf.child("atlas").attribute("file").as_string("");

	return ret;
}

// Called before the first frame
bool j1Gui::Start()
{
	bool ret = false;

	LOG("Start module gui");

	// Load atlas
	if (atlas == nullptr)
		atlas = App->tex->LoadTexture(atlas_file_name.c_str());

	// Starting intern camera position
	camera_x = App->render->camera.x;
	camera_y = App->render->camera.y;

	if (atlas != nullptr)
		ret = true;

	return ret;
}

// ---------------------------------------------------------------------
// Update all UI_Elements
// ---------------------------------------------------------------------
bool j1Gui::Update(float dt)
{
	// Debug
	if (App->input->GetKey(SDL_SCANCODE_F2) == KEY_DOWN && App->debug_mode)
		debug = !debug;
	else if (!App->debug_mode)
		debug = false;


	// Start -------------------------------------------------

	if (start)
	{
		// Set variables that inherit from window to childs
		for (p2PQueue_item<UI_Element*>* elements = App->gui->elements_list.start; elements != nullptr; elements = elements->next)
		{
			if (elements->data->type == ui_element::ui_window)
			{
				list<UI_Element*> childs;
				App->gui->GetChilds(elements->data, childs);
				for (list<UI_Element*>::iterator it = childs.begin(); it != childs.end(); it++)
				{
					(*it)->blit_layer = elements->data->blit_layer;
					(*it)->is_ui = elements->data->is_ui;
				}
			}
		}

		start = false;
	}

	// Update
	// -------------------------------------------------------
	
	// Update all elements in order
	list<UI_Element*> to_top;
	p2PQueue<UI_Element*> to_update;

	for (p2PQueue_item<UI_Element*>* elements = App->gui->elements_list.start; elements != nullptr; elements = elements->next)
	{
		// Move elements if the camera is moving
		if (elements->data->is_ui && (camera_x != App->render->camera.x || camera_y != App->render->camera.y))
		{
			elements->data->rect.x += camera_x - App->render->camera.x;
			elements->data->rect.y += camera_y - App->render->camera.y;
		}

		// To update if enabled
		if (elements->data->enabled)
		{
			to_update.Push(elements->data, elements->data->blit_layer);

			// Debug lines ------------------------------------
			if (debug)
			{
				for (list<UI_Element*>::iterator it = elements->data->childs.begin(); it != elements->data->childs.end(); it++)
				{
					if ((*it)->enabled)
					{
						App->render->DrawLine(elements->data->rect.x + elements->data->rect.w * 0.5f,
							elements->data->rect.y + elements->data->rect.h * 0.5f,
							(*it)->rect.x + (*it)->rect.w * 0.5f,
							(*it)->rect.y + (*it)->rect.h * 0.5,
							255, 255, 255);
					}
				}
			}
			// -------------------------------------------------
		}

		//Take higher layer
		if (elements->next == nullptr)
			higher_layer = elements->priority;
	}

	// Update
	for (p2PQueue_item<UI_Element*>* up = to_update.start; up != nullptr; up = up->next)
		up->data->update();

	// Move clicked elements
	Move_Elements();

	// Update intern camera position
	camera_x = App->render->camera.x;
	camera_y = App->render->camera.y;

	return true;
}

// Update all guis
bool j1Gui::PreUpdate()
{
	return true;
}

// Called after all Updates
bool j1Gui::PostUpdate()
{
	return true;
}

// Called before quitting
bool j1Gui::CleanUp()
{
	LOG("Freeing GUI");

	App->tex->UnLoadTexture(atlas);

	while (elements_list.Count() > 0)
	{
		p2PQueue_item<UI_Element*>* elements = App->gui->elements_list.start;
		DeleteElement(elements->data);
	}

	return true;
}

const void j1Gui::GetAtlas() const
{
	App->gui->atlas = App->tex->LoadTexture(atlas_file_name.c_str());
}

// ---------------------------------------------------------------------
// Create a new Window
// ---------------------------------------------------------------------
UI_Window* j1Gui::UI_CreateWin(iPoint pos, int w, int h, int blit, bool _dinamic, bool _is_ui)
{
	UI_Window* ret = nullptr;
	ret = new UI_Window();

	if (ret != nullptr)
	{
		ret->Set(pos, w, h);
		ret->dinamic = _dinamic;
		ret->started_dinamic = _dinamic;
		ret->is_ui = _is_ui;

		// Layer

		ret->layer = elements_list.Count();
		ret->blit_layer = blit;

		// -----

		ret->type = ui_window;
		ret->parent = ret;

		elements_list.Push(ret, ret->layer);
		windows.push_back(ret);
	}

	return ret;
}


// ---------------------------------------------------------------------
// Gets all the childs of a UI_Element.
// ---------------------------------------------------------------------
void j1Gui::GetChilds(UI_Element * element, list<UI_Element*>& visited)
{
	list<UI_Element*> frontier;

	visited.push_back(element);

	// Add the current childs
	for (list<UI_Element*>::iterator it = element->childs.begin(); it != element->childs.end(); it++)
		frontier.push_back(*it);


	// Navigate through all the childs and add them

	int end = 0;
	while (!frontier.empty())
	{
		for (list<UI_Element*>::iterator fr = frontier.begin(); fr != frontier.end(); fr++)
		{
			list<UI_Element*>::iterator find = std::find(visited.begin(), visited.end(), *fr);
			if (find == visited.end() && *fr != element)
			{
				visited.push_back(*fr);
				for (list<UI_Element*>::iterator ch = (*fr)->childs.begin(); ch != (*fr)->childs.end(); ch++)
				{
					frontier.push_back(*ch);
				}
			}
			frontier.erase(fr);
		}
	}

	// ---------------------------------------
}

// ---------------------------------------------------------------------
// Gets all the parents of a UI_Element.
// ---------------------------------------------------------------------
void j1Gui::GetParentElements(UI_Element * element, list<UI_Element*>& visited)
{
	UI_Element* curr = element;
	
	while (curr != nullptr)
	{
		if(curr != nullptr)
			visited.push_back(curr);
		curr = curr->parent_element;
	}
}

// ---------------------------------------------------------------------
// Updates the PQ elements order.
// ---------------------------------------------------------------------
void j1Gui::ReorderElements()
{
	list<UI_Element*> copy;

	// Copy all elements of PQ and clean it
	while (App->gui->elements_list.Count() != 0)
	{
		UI_Element* tmp;
		App->gui->elements_list.Pop(tmp);
		copy.push_back(tmp);
	}

	App->gui->elements_list.Clear();

	// Place againt he elements on the PQ (now they are on the correct order)
	for (list<UI_Element*>::iterator it = copy.begin(); it != copy.end(); it++)
		App->gui->elements_list.Push(*it, (*it)->layer);
}

// ---------------------------------------------------------------------
// Moves the clicked UI_Element, and it's childs, with the mouse.
// ---------------------------------------------------------------------
bool j1Gui::Move_Elements()
{
	int ret = false;

	// Click
	if((App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_DOWN) && !moving)
	{
		App->input->GetMousePosition(mouse_x, mouse_y);
		mouse_x -= App->render->camera.x;
		mouse_y -= App->render->camera.y;

		// Get the object with the higher layer
		to_move = CheckClickMove(mouse_x, mouse_y);

		if (to_move != nullptr)
		{
			// Put window and childs to top
			to_move->PutWindowToTop();
			moving = true;
			ret = true;
		}
	}

	// Move ---------------------
	if (moving)
	{
		// Get Mouse ------------

		int curr_x; int curr_y;
		App->input->GetMousePosition(curr_x, curr_y);
		curr_x -= App->render->camera.x;
		curr_y -= App->render->camera.y;

		// ----------------------

		// Get childs 
		list<UI_Element*> visited;
		App->gui->GetChilds(to_move, visited);

		// Move all childs ------
		for (list<UI_Element*>::iterator it = visited.begin(); it != visited.end(); it++)
		{
			if (curr_x != mouse_x)
				(*it)->rect.x -= mouse_x - curr_x;

			if (curr_y != mouse_y)
				(*it)->rect.y -= mouse_y - curr_y;
		}

		// Update mouse stored in childs
		for (list<UI_Element*>::iterator it = visited.begin(); it != visited.end(); it++)
		{
			(*it)->mouse_x = curr_x;
			(*it)->mouse_y = curr_y;
		}

		// Update mouse stored in this element
		mouse_x = curr_x;
		mouse_y = curr_y;

		// ----------------------
	}

	// Release click
	if ((App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_UP))
	{
		to_move = nullptr;
		moving = false;
	}

	return ret;
}

// ---------------------------------------------------------------------
// Chooses the element that has to be moved.
// ---------------------------------------------------------------------
UI_Element* j1Gui::CheckClickMove(int x, int y)
{
	list<UI_Element*> elements_clicked;

	// Check the UI_Elements that are in the point
	for (p2PQueue_item<UI_Element*>* elements = App->gui->elements_list.start; elements != nullptr; elements = elements->next)
	{
		if (x > elements->data->rect.x && x < elements->data->rect.x + elements->data->rect.w)
		{
			if (y > elements->data->rect.y && y < elements->data->rect.y + elements->data->rect.h)
			{
				// Check if you can click through it and if it's enabled
				if (!elements->data->click_through && elements->data->enabled)
				{
					elements_clicked.push_back(elements->data);
				}
			}
		}
	}

	// Get the higher element
	int higher_layer = -1;
	int higher_blit_layer = -1;
	UI_Element* higher_element = nullptr;

	if (!elements_clicked.empty())
	{
		for (list<UI_Element*>::iterator it = elements_clicked.begin(); it != elements_clicked.end(); it++)
		{
			if ((*it)->layer > higher_layer && (*it)->blit_layer >= higher_blit_layer)
			{
				higher_layer = (*it)->layer;
				higher_blit_layer = (*it)->blit_layer;
				higher_element = *it;
			}
		}

		//  If the current it's not dynamic, check if there is dinamic parents
		if (!higher_element->dinamic)
		{
			list<UI_Element*> parents_list;
			App->gui->GetParentElements(higher_element, parents_list);

			higher_element = nullptr;

			for (list<UI_Element*>::iterator it = parents_list.begin(); it != parents_list.end(); it++)
			{
				if ((*it)->dinamic)
				{
					higher_element = *it;
					break;
				}
			}
		}
	}

	return higher_element;
}

// ---------------------------------------------------------------------
// Deletes and frees an UI_Element.
// ---------------------------------------------------------------------
void j1Gui::DeleteElement(UI_Element* element)
{
	if (element == nullptr || element == NULL)
		return;

	list<UI_Element*> childs;
	App->gui->GetChilds(element, childs);

	// Delete element and it's childs
	for (list<UI_Element*>::iterator ch = childs.begin(); ch != childs.end(); ch++)
	{
		if (*ch == nullptr && (*ch)->parent->childs.size() > 0)
			(*ch)->parent->childs.remove(*ch);

		if ((*ch)->parent_element != nullptr && (*ch)->parent_element->childs.size() > 0)
			(*ch)->parent_element->childs.remove(*ch);

		if ((*ch)->type == ui_window && windows.size() > 0)
			windows.remove((UI_Window*)*ch);

			// Delete from pQ
			list<UI_Element*> to_add;

			while (App->gui->elements_list.Count() > 0)
			{
				UI_Element* current = nullptr;
				App->gui->elements_list.Pop(current);

				if (current != *ch)
					to_add.push_back(current);
			}

			for (list<UI_Element*>::iterator ta = to_add.begin(); ta != to_add.end(); ta++)
				App->gui->elements_list.Push((*ta), (*ta)->layer);

		(*ch)->cleanup();
		RELEASE((*ch));
	}
}

// -----------------------------------
// ------------------------- Class Gui

// -----------------------------------
// Element ---------------------------

UI_Element::UI_Element()
{
}

UI_Element::~UI_Element()
{
}

bool UI_Element::update()
{
	return true;
}

bool UI_Element::cleanup()
{
	return true;
}

void UI_Element::SetEnabled(bool set)
{
	enabled = set;
}

// ---------------------------------------------------------------------
// Enables or disables all the childs of an UI_Element.
// ---------------------------------------------------------------------
void UI_Element::SetEnabledAndChilds(bool set)
{
	list<UI_Element*> visited;
	App->gui->GetChilds(this, visited);

	for (list<UI_Element*>::iterator it = visited.begin(); it != visited.end(); it++)
		(*it)->enabled = set;
}


// ---------------------------------------------------------------------
// Put all elements of a window to the top of the PQ.
// ---------------------------------------------------------------------
bool UI_Element::PutWindowToTop()
{
	bool ret = true;

	list<UI_Element*> visited;
	list<UI_Element*> copy;

	// Get childs from the window parent
	App->gui->GetChilds(parent, visited);

	// Update layer
	int i = 0;
	for (list<UI_Element*>::iterator it = visited.begin(); it != visited.end(); it++, i++)
		(*it)->layer = App->gui->higher_layer + i + 1;
	
	// Rorded the elements of the PQ
	App->gui->ReorderElements();

	return ret;
}

// ---------------------------------------------------------------------
// Detects the highest layer of a clicked point.
// ---------------------------------------------------------------------
int UI_Element::CheckClickOverlap(int x, int y)
{
	list<UI_Element*> contactors;

	// Check the UI_Elements that are in the point
	for (p2PQueue_item<UI_Element*>* elements = App->gui->elements_list.start; elements != nullptr; elements = elements->next)
	{
		if (x > elements->data->rect.x && x < elements->data->rect.x + elements->data->rect.w)
		{
			if (y > elements->data->rect.y && y < elements->data->rect.y + elements->data->rect.h)
			{
				// Check if is dinamic
				if (!elements->data->click_through)
					contactors.push_back(elements->data);
			}
		}
	}

	// Get the higher layer
	int higher_layer = -1;
	int higher_blit_layer = -1;
	if (!contactors.empty())
	{
		for (list<UI_Element*>::iterator it = contactors.begin(); it != contactors.end(); it++)
		{
			if ((*it)->layer > higher_layer && (*it)->blit_layer >= higher_blit_layer)
			{
				higher_layer = (*it)->layer;
				higher_blit_layer = (*it)->blit_layer;
			}
		}
	}

	return higher_layer;
}

bool UI_Element::CheckClickRect(int x, int y)
{
	if (x > rect.x && x < rect.x + rect.w)
	{
		if (y > rect.y && y < rect.y + rect.h)
		{
			return true;
		}
	}

	return false;
}

// ---------------------------------------------------------------------
// Adds a child to an UI_Element.
// ---------------------------------------------------------------------
void UI_Element::AddChild(UI_Element * _child)
{
	childs.push_back(_child);
	_child->parent_element = this;
}

// ---------------------------------------------------------------------
// Adds both childs one to the other to avoid the overlaping check.
// ---------------------------------------------------------------------
void UI_Element::AddChildBoth(UI_Element * _child)
{
	childs.push_back(_child);
	_child->parent_element = this;
	_child->childs.push_back(this);
	this->parent_element = _child;
}

// ---------------------------------------------------------------------
// Mouse check functions.
// ---------------------------------------------------------------------

bool UI_Element::MouseClickEnterLeftIntern()
{
	if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_DOWN)
	{
		int mouse_x, mouse_y;
		App->input->GetMousePosition(mouse_x, mouse_y);
		mouse_x -= App->render->camera.x;
		mouse_y -= App->render->camera.y;

		if (mouse_x > rect.x && mouse_x < rect.x + rect.w)
		{
			if (mouse_y > rect.y && mouse_y < rect.y + rect.h)
			{
				clicked = true;
				return true;
			}
		}
	}
	return false;
}

bool UI_Element::MouseClickOutLeftIntern()
{
	if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_UP)
	{
		if (clicked)
		{
			clicked = false;
			return true;
		}
	}
	return false;
}

void UI_Element::SetDebugColor(SDL_Color _color)
{
	color.r = _color.r; color.g = _color.g; color.b = _color.b; color.a = _color.a;
}

// -----------------------------------
// --------------------------- Element

// -----------------------------------
// Window ----------------------------

UI_Window::UI_Window()
{
}

UI_Window::~UI_Window()
{
}

bool UI_Window::update()
{
	if (App->gui->debug)
		App->render->DrawQuad(rect, color.r, color.g, color.b, color.a, false);
	
	return true;
}

void UI_Window::Set(iPoint pos, int w, int h)
{
	rect.x = pos.x;
	rect.y = pos.y;
	rect.w = w;
	rect.h = h;

	color.r = color.g = color.b = color.a = 255;
}

// ---------------------------------------------------------------------
// Create a button linked to the current window
// ---------------------------------------------------------------------
UI_Button* UI_Window::CreateButton(iPoint pos, int w, int h, bool _dinamic)
{
	UI_Button* ret = nullptr;
	ret = new UI_Button();

	if (ret != nullptr)
	{
		ret->type = ui_button;
		ret->Set(pos, w, h);
		ret->parent = this;
		ret->parent_element = this;
		ret->dinamic = _dinamic;
		ret->started_dinamic = _dinamic;

		// Layers --

		ret->layer = childs.size() + layer + 1;

		// ---------

		App->gui->elements_list.Push(ret, ret->layer);
		childs.push_back((UI_Element*)ret);
	}
	return ret;
}


// ---------------------------------------------------------------------
// Create text linked to the current window
// ---------------------------------------------------------------------
UI_Text* UI_Window::CreateText(iPoint pos, _TTF_Font * font, int spacing, bool _dinamic, uint r, uint g, uint b)
{
	UI_Text* ret = nullptr;
	ret = new UI_Text();

	if (ret != nullptr)
	{
		ret->type = ui_text;
		ret->Set(pos, font, spacing, r, g, b);
		ret->parent = this;
		ret->parent_element = this;
		ret->dinamic = _dinamic;
		ret->started_dinamic = _dinamic;

		// Layers --

		ret->layer = childs.size() + layer + 1;

		// ---------

		App->gui->elements_list.Push(ret, ret->layer);
		childs.push_back((UI_Element*)ret);
	}
	return ret;
}

// ---------------------------------------------------------------------
// Create an image linked to the current window
// ---------------------------------------------------------------------
UI_Image* UI_Window::CreateImage(iPoint pos, SDL_Rect image, bool _dinamic)
{
	UI_Image* ret = nullptr;
	ret = new UI_Image();
	
	if (ret != nullptr)
	{
		ret->type = ui_image;
		ret->Set(pos, image);
		ret->parent = this;
		ret->parent_element = this;
		ret->dinamic = _dinamic;
		ret->started_dinamic = _dinamic;

		// Layers --

		ret->layer = childs.size() + layer + 1;

		// ---------

		App->gui->elements_list.Push(ret, ret->layer);
		childs.push_back((UI_Element*)ret);
	}
	return ret;
}

// ---------------------------------------------------------------------
// Create a text input box to the current window
// ---------------------------------------------------------------------
UI_Text_Input* UI_Window::CreateTextInput(iPoint pos, int w, _TTF_Font* font, bool _dinamic, uint r, uint g, uint b)
{
	UI_Text_Input* ret = nullptr;
	ret = new UI_Text_Input();

	if (ret != nullptr)
	{
		ret->type = ui_text_input;
		ret->Set(pos, w, font, r, g, b);
		ret->parent = this;
		ret->parent_element = this;
		ret->dinamic = _dinamic;
		ret->started_dinamic = _dinamic;

		// Layers --

		ret->layer = childs.size() + layer + 1;

		// ---------

		App->gui->elements_list.Push(ret, ret->layer);
		childs.push_back((UI_Element*)ret);
	}
	return ret;
}

UI_Scroll_Bar * UI_Window::CreateScrollBar(iPoint pos, int view_w, int view_h, int button_size, bool _dinamic)
{
	UI_Scroll_Bar* ret = nullptr;
	ret = new UI_Scroll_Bar();

	if(ret != nullptr)
	{
		ret->type = ui_scroll_bar;
		ret->Set(pos, view_w, view_h, button_size);
		ret->parent = this;
		ret->parent_element = this;
		ret->dinamic = _dinamic;
		ret->started_dinamic = _dinamic;

		// Layers --

		ret->layer = childs.size() + layer + 1;

		// ---------

		App->gui->elements_list.Push(ret, ret->layer);
		childs.push_back((UI_Element*)ret);
	}

	return ret;
}

UI_ColoredRect * UI_Window::CreateColoredRect(iPoint pos, int w, int h, SDL_Color color, bool filled, bool _dinamic)
{
	UI_ColoredRect* ret = nullptr;
	ret = new UI_ColoredRect();

	if (ret != nullptr)
	{
		ret->type = ui_colored_rect;
		ret->Set(pos, w, h, color, filled);
		ret->parent = this;
		ret->parent_element = this;
		ret->dinamic = _dinamic;
		ret->started_dinamic = _dinamic;

		// Layers --

		ret->layer = childs.size() + layer + 1;

		// ---------

		App->gui->elements_list.Push(ret, ret->layer);
		childs.push_back((UI_Element*)ret);
	}

	return ret;
}

// -----------------------------------
// ---------------------------- Window

// -----------------------------------
// Button ----------------------------

UI_Button::UI_Button()
{
}

UI_Button::~UI_Button()
{
}

void UI_Button::Set(iPoint _pos, int w, int h)
{
	rect.x = _pos.x;
	rect.y = _pos.y;
	rect.w = w;
	rect.h = h;

	color.r = color.g = color.b = color.a = 255;
}

bool UI_Button::update()
{
	if (!enabled)
		return false;

	if (App->gui->debug)
		App->render->DrawQuad(rect, color.r, color.g, color.b, -1.0f, color.a, false);

	if(print)
		App->render->Blit(App->gui->atlas, rect.x, rect.y, &curr);

	ChangeButtonStats();

	return true;
}

void UI_Button::ChangeButtonStats()
{
	if (to_enter)
		enter = true;
	if (!to_enter)
		enter = false;
	if (to_clicked_left)
		clicked_left = true;
	if (!to_clicked_left)
		clicked_left = false;
	if (to_clicked_right)
		clicked_right = true;
	if (!to_clicked_right)
		clicked_right = false;
}

bool UI_Button::MouseEnter()
{
	if (!enabled)
		return false;

	int mouse_x, mouse_y;
	App->input->GetMousePosition(mouse_x, mouse_y);
	mouse_x -= App->render->camera.x;
	mouse_y -= App->render->camera.y;

	if (CheckClickOverlap(mouse_x, mouse_y) != layer)
		return false;

	if(CheckClickRect(mouse_x, mouse_y))
	{
		if (!enter)
		{
			to_enter = true;
			return true;
		}
		return false;
	}

	return false;
}

bool UI_Button::MouseOut()
{
	if (!enabled)
		return false;	
	
	int mouse_x, mouse_y;
	App->input->GetMousePosition(mouse_x, mouse_y);
	mouse_x -= App->render->camera.x;
	mouse_y -= App->render->camera.y;

	if (CheckClickOverlap(mouse_x, mouse_y) != layer && !enter)
		return true;

	if (CheckClickRect(mouse_x, mouse_y))
		return false;

	if(enter)
	{
		to_enter = false;
		return true;
	}
	else
		return false;
}

bool UI_Button::MouseClickEnterLeft()
{
	if (!enabled)
		return false;

	if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_DOWN)
	{
		int mouse_x, mouse_y;
		App->input->GetMousePosition(mouse_x, mouse_y);
		mouse_x -= App->render->camera.x;
		mouse_y -= App->render->camera.y;

		if (CheckClickOverlap(mouse_x, mouse_y) != layer)
			return false;

		if (CheckClickRect(mouse_x, mouse_y))
		{
			to_clicked_left = true;
			return true;
		}
	}
	return false;
}

bool UI_Button::MouseClickOutLeft()
{
	if (!enabled)
		return false;

	if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_UP)
	{
		if (clicked_left)
		{
			to_clicked_left = false;
			return true;
		}
	}
	return false;
}

bool UI_Button::MouseClickEnterRight()
{
	if (!enabled)
		return false;

	if (App->input->GetMouseButtonDown(SDL_BUTTON_RIGHT) == KEY_DOWN)
	{
		int mouse_x, mouse_y;
		App->input->GetMousePosition(mouse_x, mouse_y);
		mouse_x -= App->render->camera.x;
		mouse_y -= App->render->camera.y;

		if (CheckClickOverlap(mouse_x, mouse_y) != layer)
			return false;

		if (mouse_x > rect.x && mouse_x < rect.x + rect.w)
		{
			if (mouse_y > rect.y && mouse_y < rect.y + rect.h)
			{
				to_clicked_right = true;
				return true;
			}
		}
	}
	return false;
}

bool UI_Button::MouseClickOutRight()
{
	if (!enabled)
		return false;

	if (App->input->GetMouseButtonDown(SDL_BUTTON_RIGHT) == KEY_UP)
	{
		if (clicked_right)
		{
			to_clicked_right = false;
			return true;
		}

	}
	return false;
}

void UI_Button::AddImage(char* name, SDL_Rect rect)
{
	rect_text rt(name, rect);
	rect_list.push_back(rt);
}

void UI_Button::SetImage(char* name)
{
	for (list<rect_text>::iterator it = rect_list.begin(); it != rect_list.end(); it++)
	{
		if (TextCmp((*it).name.c_str(), name))
		{
			curr = (*it).rect;
			break;
		}
	}
}

// -----------------------------------
// ---------------------------- Button

// -----------------------------------
// Text ------------------------------

UI_Text::UI_Text()
{
}

UI_Text::~UI_Text()
{
}

void UI_Text::Set(iPoint _pos, _TTF_Font* _font, int _spacing, uint r, uint g, uint b)
{
	color.r = r;
	color.g = g;
	color.b = b;
	font = _font;

	rect.x = _pos.x;
	rect.y = _pos.y;
	rect.w = -1;
	rect.h = -1;

	spacing = _spacing;

	color.a = 255;
}

void UI_Text::SetText(string _text)
{
	// Clean last texts
	for (list<tex_str>::iterator it = tex_str_list.begin(); it != tex_str_list.end(); it++)
		App->tex->UnLoadTexture((*it).texture);

	tex_str_list.clear();

	string tmp = _text;

	int i = 0;
	string comp;
	while (tmp[i] != '\0')
	{
		comp.clear();
		int words_counter = 0;
		for (; tmp[i] != '\n' && tmp[i] != '\0';)
		{
			comp.insert(words_counter, 1, tmp[i]);
			i++, words_counter++;
		}
		if (tmp[i] != '\0')
			i++;

		comp[words_counter] = '\0';

		tex_str ts(comp.c_str(), App->font->Print(comp.c_str(), color, font));
		tex_str_list.push_back(ts);
	}
}
string UI_Text::GetText()
{
	string ret;

	int acumulated = 0;
	for (list<tex_str>::iterator it = tex_str_list.begin(); it != tex_str_list.end(); it++)
	{
		ret.insert(acumulated, (*it).text.c_str());
		acumulated += (*it).text.size() + 1;
	}
	ret[acumulated] = '\0';

	return ret;
}

bool UI_Text::update()
{
	if (!enabled)
		return false;
	
	// Get highest w and add all h
	int w = 0, h = 0;
	for (list<tex_str>::iterator it = tex_str_list.begin(); it != tex_str_list.end(); it++)
	{
		App->font->CalcSize((*it).text.c_str(), rect.w, rect.h, font);
		h += rect.h;
		if (rect.w > w)
			w = rect.w;
	}

	rect.w = w;
	rect.h = h;
	// ----------------------------

	if (App->gui->debug)
		App->render->DrawQuad(rect, color.r, color.g, color.b, -1.0f, color.a, false);
	
	if (print)
	{
		int space = 0;
		for (list<tex_str>::iterator it = tex_str_list.begin(); it != tex_str_list.end(); it++)
		{
			if (!TextCmp((*it).text.c_str(), ""))
			{
				App->render->Blit((*it).texture, rect.x, rect.y + space);
				space += spacing;
			}
		}
	}

	return true;
}

bool UI_Text::cleanup()
{
	for (list<tex_str>::iterator it = tex_str_list.begin(); it != tex_str_list.end(); it++)
		App->tex->UnLoadTexture((*it).texture);

	return true;
}

// -----------------------------------
// ------------------------------ Text

// -----------------------------------
// Image -----------------------------

UI_Image::UI_Image()
{
}

UI_Image::~UI_Image()
{
}

void UI_Image::Set(iPoint _pos, SDL_Rect _image)
{
	rect.x =  _pos.x;
	rect.y =  _pos.y;

	image.x = _image.x;
	image.y = _image.y;
	image.w = _image.w;
	image.h = _image.h;
	rect.w =  _image.w;
	rect.h =  _image.h;

	color.r = color.g = color.b = color.a = 255;
}

void UI_Image::ChangeImage(SDL_Rect _rect)
{
	image = { _rect.x, _rect.y, _rect.w, _rect.h };
	rect.w = _rect.w;
	rect.h = _rect.h;
}


bool UI_Image::update()
{
	if (!enabled)
		return false;

	if (App->gui->debug)
		App->render->DrawQuad(rect, color.r, color.g, color.b, -1.0f, color.a, false);
	
	if(print)
		App->render->Blit(App->gui->atlas, rect.x, rect.y, &image);

	return true;
}
// -----------------------------------
// ----------------------------- Image

// -----------------------------------
// Text Input ------------------------

UI_Text_Input::UI_Text_Input()
{
}

UI_Text_Input::~UI_Text_Input()
{
}

void UI_Text_Input::Set(iPoint pos, int w, _TTF_Font* font, uint r, uint g, uint b)
{
	rect.x = pos.x;
	rect.y = pos.y;
	rect.w = w;
	text = new UI_Text();
	text->Set(iPoint(0, 0), font, 0, r, g, b);

	// Set bar size
	App->font->CalcSize("@", bar.x, bar.h, font);
	bar.w = 1;
	rect.h = bar.h;

	camera_before.x = App->render->camera.x;
	camera_before.y = App->render->camera.y;

	color.r = color.g = color.b = color.a = 255;

	text_offset = 0;
}

bool UI_Text_Input::update()
{
	if (!enabled)
		return false;

	if (App->gui->debug)
		App->render->DrawQuad(rect, color.r, color.g, color.b, -1.0f, color.a, false);

	 string test = intern_text;

	// Print
	if (print)
	{
		SetIsActive();

		if (intern_text.size() == 0 && active)
		text->SetText("");

		// Manuall change text
		ChangeTextInput();

		// Input
		if (active)
		{
			// Take and print input
			if (TakeInput() || Delete() || MoveCursor())
			{
				// Update words position list
				if(!pasword)
					SetBarPos(intern_text.substr(0, bar_pos));
				else
					SetPasword();

				//LOG("%d %s %d", bar_pos, intern_text.substr(0, bar_pos).c_str(), text_offset);
			}

			// Move cursor
			DrawBar();
		}

		// Viewport -----------
		App->render->SetViewPort({ rect.x + App->render->camera.x, rect.y + App->render->camera.y, rect.w, rect.h });

			text->update();

		App->render->ResetViewPort();
		// --------------------

		// Camera
		if (camera_before.x != App->render->camera.x)
		{
			text->rect.x += camera_before.x - App->render->camera.x;
			camera_before.x = App->render->camera.x;
		}
		if (camera_before.y != App->render->camera.y)
		{
			text->rect.y += camera_before.y - App->render->camera.y;
			camera_before.y = App->render->camera.y;
		}

	}

	return true;
}

bool UI_Text_Input::cleanup()
{
	App->gui->DeleteElement(text);
	return true;
}

void UI_Text_Input::SetTextInput(string text)
{
	text_change = text;
	change = true;
}

bool UI_Text_Input::TakeInput()
{
	bool ret = false;

	if (App->input->input_text.size() > 0 && App->input->input_text.size() < 2)
	{
		intern_text.insert(bar_pos, App->input->input_text.c_str());

		// Set new text
		text->SetText(intern_text);

		// Clean input
		App->input->input_text.clear(); 

		// Increase bar positon
		bar_pos++;

		ret = true;
	}

	return ret;
}

bool UI_Text_Input::Delete()
{
	bool ret = false;

	if (App->input->GetKey(SDL_SCANCODE_BACKSPACE) == KEY_DOWN)
	{
		if (intern_text.size() > 0 && bar_pos > 0)
		{
			// Dynamic movement
			if (text_offset < 0)
			{
				int to_erase = GetTextSize(intern_text.substr(bar_pos - 1, 1));
				if (text_offset + to_erase > 0)
				{
					text_offset = 0;
					text->rect.x = 0;
				}
				else
				{
					text_offset += to_erase;
					text->rect.x += to_erase;
				}
			}
			// ---------------

			intern_text.erase(bar_pos-1, 1);
			bar_pos--;

			text->SetText(intern_text);

			ret = true;
		}
	}
	else if (App->input->GetKey(SDL_SCANCODE_DELETE) == KEY_DOWN)
	{
		if (intern_text.size() > 0 && bar_pos < intern_text.size())
		{
			// Dynamic movement
			if (text_offset < 0)
			{
				int to_erase = GetTextSize(intern_text.substr(bar_pos, 1));
				if (text_offset + to_erase > 0)
				{
					text_offset = 0;
					text->rect.x = 0;
				}
				else
				{
					text_offset += to_erase;
					text->rect.x += to_erase;
				}
			}
			// ---------------

			intern_text.erase(bar_pos, 1);

			text->SetText(intern_text);

			ret = true;
		}
	}

	return ret;
}

bool UI_Text_Input::MoveCursor()
{
	bool ret = false;

	if (App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_DOWN)
	{
		if (bar_pos > 0)
		{
			bar_pos--;
			ret = true;
		}
	}

	if (App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_DOWN)
	{
		if (bar_pos < intern_text.size())
		{
			bar_pos++;
			ret = true;
		}
	}

	return ret;
}

void UI_Text_Input::SetBarPos(string _text)
{
	int width, height;
	App->font->CalcSize(_text.c_str(), width, height, text->font);
	bar_x = width + text_offset;

	DinamicViewport();

	bar_x = width + text_offset;
}

int UI_Text_Input::GetTextSize(string _text)
{
	int width, height;
	App->font->CalcSize(_text.c_str(), width, height, text->font);

	return width;
}


void UI_Text_Input::DrawBar()
{
	bar.x = rect.x + bar_x;
	bar.y = rect.y;

	App->render->DrawQuad(bar, color.r, color.g, color.b, -1.0f, color.a, true);
}

void UI_Text_Input::DinamicViewport()
{
	// Right
	if (bar_x > rect.w)
	{
		text_offset -= bar_x - rect.w;
		text->rect.x -= bar_x - rect.w;
	}

	// Left
	if (bar_pos >= 0 && text_offset < 0 && bar_x <= 0)
	{
		text_offset -= bar_x;
		text->rect.x -= bar_x;
	}
}

void UI_Text_Input::SetPasword()
{
	string tmp;
	for (int i = 0; i < intern_text.size(); i++)
		tmp.insert(i, 1, '*');

	text->SetText(tmp);

	SetBarPos(tmp);
}

void UI_Text_Input::ChangeTextInput()
{
	if (change)
	{
		text->SetText(text_change);
		intern_text = text_change.c_str();
		SetBarPos(intern_text);
		bar_pos = intern_text.size();
		change = false;
	}
}

void UI_Text_Input::Clear()
{
	intern_text.clear();

	bar_pos = 0;
	bar_x = 0;
}


void UI_Text_Input::SetIsActive()
{
	if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_DOWN)
	{
		int mouse_x, mouse_y;
		App->input->GetMousePosition(mouse_x, mouse_y);
		mouse_x -= App->render->camera.x;
		mouse_y -= App->render->camera.y;

		//Check if there is another object on the same point
		if (CheckClickOverlap(mouse_x, mouse_y) == layer)
		{
			active = true;
		}
		else
		{
			active = false;
		}
	}
}

bool UI_Text_Input::MouseClick()
{
	if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_DOWN)
	{
		int mouse_x, mouse_y;
		App->input->GetMousePosition(mouse_x, mouse_y);

		if (CheckClickOverlap(mouse_x, mouse_y) != layer)
			return false;

		mouse_x -= App->render->camera.x;
		mouse_y -= App->render->camera.y;

		if (mouse_x > rect.x && mouse_x < rect.x + rect.w)
		{
			if (mouse_y > rect.y && mouse_y < rect.y + rect.h)
			{
				return true;
			}
		}
	}
	return false;
}

UI_Scroll_Bar::UI_Scroll_Bar()
{
}

UI_Scroll_Bar::~UI_Scroll_Bar()
{
}

void UI_Scroll_Bar::Set(iPoint pos, int view_w, int view_h, int button_size)
{
	// Viewport
	rect.x = pos.x;
	rect.y = pos.y;
	rect.w = view_w;
	rect.h = view_h;

	starting_v = view_h;
	starting_h = view_w;

	moving_rect.x = pos.x;
	moving_rect.y = pos.y;
	moving_rect.w = view_w;
	moving_rect.h = view_h;

	// Button vertical ---
	button_v = new UI_Button();
	button_v->Set(iPoint(view_w + button_size, pos.y), button_size, view_h);
	button_v->layer = App->gui->elements_list.Count() + 1;
	AddChild(button_v);
	button_starting_v = button_v->rect.h;
	App->gui->elements_list.Push(button_v, button_v->layer);
	// ----------

	// Button horizontal ---
	button_h = new UI_Button();
	button_h->Set(iPoint(pos.x, pos.y + view_h), view_w, button_size);
	button_h->layer = App->gui->elements_list.Count() + 2;
	AddChild(button_h);
	button_starting_h = button_h->rect.w;
	App->gui->elements_list.Push(button_h, button_h->layer);
	// ----------

	// Min and max bar movement allowed
	min_bar_v = pos.y;
	max_bar_v = min_bar_v + view_h;

	min_bar_h = pos.x;
	max_bar_h = min_bar_h + view_w;

	color.r = color.g = color.b = color.a = 255;
}

bool UI_Scroll_Bar::update()
{
	if (!enabled)
		return false;

	if (App->gui->debug)
	{
		App->render->DrawQuad(moving_rect, color.r, color.g, color.b, -1.0f, color.a, false);
		App->render->DrawQuad(rect, 255, 0, 0, -1.0f, 255, false);
		App->render->DrawLine(button_v->rect.x + (button_v->rect.w / 2), min_bar_v, button_v->rect.x + (button_v->rect.w / 2), max_bar_v, color.r, color.g, color.b, -1.0f, color.a);
		App->render->DrawLine(min_bar_h, button_h->rect.y + (button_h->rect.h/2), max_bar_h, button_h->rect.y + (button_h->rect.h / 2), color.r, color.g, color.b, -1.0f, color.a);
	}

	// Viewport -----------
	App->render->SetViewPort({ rect.x + App->render->camera.x, rect.y + App->render->camera.y, rect.w, rect.h});
	//  rect.x + rect.w + App->render->camera.x

	for (list<scroll_element>::iterator it = elements.begin(); it != elements.end(); it++)
		(*it).element->update();
	

	App->render->ResetViewPort();
	// --------------------

	ChangeHeightMovingRect();
	ChangeWidthMovingRect();

	MoveBarV();
	MoveBarH();
	
	// Lock element when moving scroll bars
	if (parent->started_dinamic)
	{
		if (button_v->MouseClickEnterLeft() || button_h->MouseClickEnterLeft())
			parent->dinamic = false;
		if (button_v->MouseClickOutLeft() || button_h->MouseClickOutLeft())
			parent->dinamic = true;
	}

	return true;
}

bool UI_Scroll_Bar::cleanup()
{
	ClearElements();

	return true;
}

void UI_Scroll_Bar::AddElement(UI_Element * element)
{
	scroll_element el;
	el.element = element;
	el.element->parent = parent;
	el.starting_pos_x = element->rect.x;
	el.starting_pos_y = element->rect.y;
	elements.push_back(el);
}

void UI_Scroll_Bar::DeleteScrollElement(UI_Element * element)
{
	for (list<scroll_element>::iterator it = elements.begin(); it != elements.end(); it++)
	{
		if ((*it).element == element)
		{
			elements.remove(*it);
			App->gui->DeleteElement(element);
			break;
		}
	}
}

void UI_Scroll_Bar::ClearElements()
{
	while (!elements.empty())
	{
		for (list<scroll_element>::iterator it = elements.begin(); it != elements.end(); it++)
		{
			App->gui->DeleteElement((*it).element);
			elements.remove(*it);
			break;
		}
	}
	
	elements.clear();
}

void UI_Scroll_Bar::ChangeHeightMovingRect()
{
	// Taking lowest element vertical --
	int lowest = 0;

	for (list<scroll_element>::iterator it = elements.begin(); it != elements.end(); it++)
	{
		if (((min_bar_v - moving_rect.y) + (*it).element->rect.y + (*it).element->rect.h + App->render->camera.y) > lowest)
			lowest = ((min_bar_v - moving_rect.y) + (*it).element->rect.y + (*it).element->rect.h) + App->render->camera.y;
	}
	// ----------------------------------

	moving_rect.h = lowest;

	// Inverse rule of thirds
	if (moving_rect.h > 0)
	{
		button_v->rect.h = (button_starting_v * starting_v) / moving_rect.h;
		if (button_v->rect.h < 20)
			button_v->rect.h = 20;
		if (button_v->rect.h > button_starting_v)
			button_v->rect.h = button_starting_v;
	}
	else
	{
		button_v->rect.y = min_bar_v;
		button_v->rect.h = button_starting_v;
	}
	
	// Update min and max bar positions
	min_bar_v = rect.y;
	max_bar_v = rect.y + rect.h;
}

void UI_Scroll_Bar::ChangeWidthMovingRect()
{
	// Take higher element horizontal --
	int higher = 0;
	for (list<scroll_element>::iterator it = elements.begin(); it != elements.end(); it++)
	{
		if (((min_bar_h - moving_rect.x) + (*it).element->rect.x + (*it).element->rect.w + App->render->camera.x) > higher)
			higher = ((min_bar_h - moving_rect.x) + (*it).element->rect.x + (*it).element->rect.w) + App->render->camera.x;
	}
	// ----------------------------------

	if (starting_h < higher)
	{
		moving_rect.w = higher;

		// Inverse rule of thirds
		button_h->rect.w = (button_starting_h * starting_h) / moving_rect.w;
		if (button_h->rect.w < 20)
			button_h->rect.w = 20;
		if (button_h->rect.w > button_starting_h)
			button_h->rect.w = button_starting_h;
	}
	else
	{
		button_h->rect.x = min_bar_h;
		button_h->rect.w = button_starting_h;
	}


	// Update min and max bar positions
	min_bar_h = rect.x;
	max_bar_h = min_bar_h + rect.w;
}

void UI_Scroll_Bar::MoveBarV()
{
	if (button_v->MouseClickEnterLeft())
	{
		int mouse_x_tmp;
		App->input->GetMousePosition(mouse_x_tmp, mouse_y);

		is_scrolling_v = true;
	}

	if (is_scrolling_v)
	{
		int curr_x; int curr_y;
		App->input->GetMousePosition(curr_x, curr_y);

		// ----------------------

		// Move buttons
		if (curr_y != mouse_y)
		{
			if (((button_v->rect.y + button_v->rect.h) - (mouse_y - curr_y)) <= max_bar_v && (button_v->rect.y - (mouse_y - curr_y)) >= min_bar_v)
			{
				button_v->rect.y -= mouse_y - curr_y;
			}
			else if(((button_v->rect.y + button_v->rect.h) - (mouse_y - curr_y)) > max_bar_v)
			{
				button_v->rect.y += max_bar_v - (button_v->rect.y + button_v->rect.h);
			}
			else if ((button_v->rect.y - (mouse_y - curr_y)) < min_bar_v)
			{
				button_v->rect.y -= button_v->rect.y - min_bar_v;
			}

			mouse_y = curr_y;
		}
	}

	if (button_v->MouseClickOutLeft())
	{
		is_scrolling_v = false;
	}

	// Move elements inside the scroll 
	int bar_distance = (min_bar_v + button_v->rect.h) - max_bar_v;
	int moving_distance = (min_bar_v + moving_rect.h) - max_bar_v;
	int position_bar = button_v->rect.y - min_bar_v;

	if (bar_distance < 0)
	{
		// Rule of thirds
		scroll_v = -floor((float)(position_bar * moving_distance) / bar_distance);
		moving_rect.y = min_bar_v - scroll_v;

		for (list<scroll_element>::iterator it = elements.begin(); it != elements.end(); it++)
		{
			(*it).element->rect.y = (*it).starting_pos_y - scroll_v - App->render->camera.y;
		}
	}
	else
	{
		moving_rect.y = min_bar_v - scroll_v;

		for (list<scroll_element>::iterator it = elements.begin(); it != elements.end(); it++)
		{
			(*it).element->rect.y = (*it).starting_pos_y - scroll_v - App->render->camera.y;
		}
	}

}

void UI_Scroll_Bar::MoveBarH()
{
	if (button_h->MouseClickEnterLeft())
	{
		int mouse_y_tmp;
		App->input->GetMousePosition(mouse_x, mouse_y_tmp);

		is_scrolling_h = true;
	}

	if (is_scrolling_h)
	{
		int curr_x; int curr_y;
		App->input->GetMousePosition(curr_x, curr_y);

		// ----------------------

		if (curr_x != mouse_x)
		{
			if (((button_h->rect.x + button_h->rect.w) - (mouse_x - curr_x)) <= max_bar_h && (button_h->rect.x - (mouse_x - curr_x)) >= min_bar_h)
			{
				button_h->rect.x -= mouse_x - curr_x;
			}
			else if (((button_h->rect.x + button_h->rect.w) - (mouse_x - curr_x)) > max_bar_h)
			{
				button_h->rect.x += max_bar_h - (button_h->rect.x + button_h->rect.w);
			}
			else if ((button_h->rect.x - (mouse_x - curr_x)) < min_bar_h)
			{
				button_h->rect.x -= button_h->rect.x - min_bar_h;
			}

			mouse_x = curr_x;
		}
	}

	if (button_h->MouseClickOutLeft())
	{
		is_scrolling_h = false;
	}

	int bar_distance = (min_bar_h + button_h->rect.w) - max_bar_h;
	int moving_distance = (min_bar_h + moving_rect.w) - max_bar_h;
	int position_bar = button_h->rect.x - min_bar_h;

	if (bar_distance < 0)
	{
		scroll_h = -floor((float)(position_bar * moving_distance) / bar_distance);
		moving_rect.x = min_bar_h - scroll_h;

		for (list<scroll_element>::iterator it = elements.begin(); it != elements.end(); it++)
		{
			(*it).element->rect.x = (*it).starting_pos_x - scroll_h - App->render->camera.x;
		}
	}
	else
	{
		moving_rect.x = min_bar_h - scroll_h;

		for (list<scroll_element>::iterator it = elements.begin(); it != elements.end(); it++)
		{
			(*it).element->rect.x = (*it).starting_pos_x - scroll_h - App->render->camera.x;
		}
	}
}

UI_ColoredRect::UI_ColoredRect()
{
}

UI_ColoredRect::~UI_ColoredRect()
{
}

void UI_ColoredRect::Set(iPoint pos, int w, int h, SDL_Color _color, bool filled)
{
	rect.x = pos.x;
	rect.y = pos.y;
	rect.w = w;
	rect.h = h;
	color = _color;
}

bool UI_ColoredRect::update()
{
	if (!enabled)
		return false;

	App->render->DrawQuad(rect, color.r, color.g, color.b, -1.0f, color.a, filled);

	return true;
}

void UI_ColoredRect::SetColor(SDL_Color _color)
{
	color = _color;
}
