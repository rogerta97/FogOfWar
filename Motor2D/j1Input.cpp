#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Input.h"
#include "j1Window.h"
#include "SDL/include/SDL.h"

#define MAX_KEYS 300

j1Input::j1Input() : j1Module()
{
	name = "input";

	keyboard = new j1KeyState[MAX_KEYS];
	memset(keyboard, KEY_IDLE, sizeof(j1KeyState) * MAX_KEYS);
	memset(mouse_buttons, KEY_IDLE, sizeof(j1KeyState) * NUM_MOUSE_BUTTONS);

	memset(gamepad_connected, -1, sizeof(int)*MAX_GAMECONTROLLERS);
}

// Destructor
j1Input::~j1Input()
{
	delete[] keyboard;
}

// Called before render is available
bool j1Input::Awake(pugi::xml_node& config)
{
	LOG("Init SDL input event system");
	bool ret = true;
	SDL_Init(0);

	if(SDL_InitSubSystem(SDL_INIT_EVENTS) < 0)
	{
		LOG("SDL_EVENTS could not initialize! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}

	// GUI -------------------------
	if (SDL_Init(SDL_INIT_VIDEO) != 0)
		LOG("Error on SDL_Init");
	// -----------------------------

	// GameController --------------
	/// To use PS3 Controller install this driver https://github.com/nefarius/ScpToolkit/releases/tag/v1.6.238.16010
	if(SDL_Init(SDL_INIT_GAMECONTROLLER) != 0)
		LOG("Error on SDL_Init");
	// -----------------------------

	return ret;
}

// Called before the first frame
bool j1Input::Start()
{
	LOG("Start module input");
	SDL_StopTextInput();

	return true;
}

// Called each loop iteration
bool j1Input::PreUpdate()
{
	static SDL_Event event;
	
	const Uint8* keys = SDL_GetKeyboardState(NULL);

	for(int i = 0; i < MAX_KEYS; ++i)
	{
		if(keys[i] == 1)
		{
			if(keyboard[i] == KEY_IDLE)
				keyboard[i] = KEY_DOWN;
			else
				keyboard[i] = KEY_REPEAT;
		}
		else
		{
			if(keyboard[i] == KEY_REPEAT || keyboard[i] == KEY_DOWN)
				keyboard[i] = KEY_UP;
			else
				keyboard[i] = KEY_IDLE;
		}
	}

	for(int i = 0; i < NUM_MOUSE_BUTTONS; ++i)
	{
		if(mouse_buttons[i] == KEY_DOWN)
			mouse_buttons[i] = KEY_REPEAT;

		if(mouse_buttons[i] == KEY_UP)
			mouse_buttons[i] = KEY_IDLE;
	}

	for (std::vector<GamePad*>::iterator it = gamepads.begin(); it != gamepads.end(); it++) 
	{
		for (int i = 0; i < NUM_CONTROLLER_BUTTONS; ++i) 
		{
			if ((*it)->gamecontroller_buttons[i] == KEY_DOWN)
				(*it)->gamecontroller_buttons[i] = KEY_REPEAT;
						 
			if ((*it)->gamecontroller_buttons[i] == KEY_UP)
				(*it)->gamecontroller_buttons[i] = KEY_IDLE;
		}
	}

	// GUI -------------------------
	SDL_StartTextInput();
	input_text.clear();
	// -----------------------------

	while(SDL_PollEvent(&event) != 0)
	{
		switch(event.type)
		{
			case SDL_QUIT:
				windowEvents[WE_QUIT] = true;
			break;

			case SDL_WINDOWEVENT:
				switch(event.window.event)
				{
					//case SDL_WINDOWEVENT_LEAVE:
					case SDL_WINDOWEVENT_HIDDEN:
					case SDL_WINDOWEVENT_MINIMIZED:
					case SDL_WINDOWEVENT_FOCUS_LOST:
					windowEvents[WE_HIDE] = true;
					break;

					//case SDL_WINDOWEVENT_ENTER:
					case SDL_WINDOWEVENT_SHOWN:
					case SDL_WINDOWEVENT_FOCUS_GAINED:
					case SDL_WINDOWEVENT_MAXIMIZED:
					case SDL_WINDOWEVENT_RESTORED:
					windowEvents[WE_SHOW] = true;
					break;
				}
			break;

			case SDL_MOUSEBUTTONDOWN:
				mouse_buttons[event.button.button - 1] = KEY_DOWN;
				//LOG("Mouse button %d down", event.button.button-1);
			break;

			case SDL_MOUSEBUTTONUP:
				mouse_buttons[event.button.button - 1] = KEY_UP;
				//LOG("Mouse button %d up", event.button.button-1);
			break;

			case SDL_MOUSEMOTION:
			{
				int scale = App->win->GetScale();
				mouse_motion_x = event.motion.xrel / scale;
				mouse_motion_y = event.motion.yrel / scale;
				mouse_x = event.motion.x / scale;
				mouse_y = event.motion.y / scale;
				//LOG("Mouse motion x %d y %d", mouse_motion_x, mouse_motion_y);
			}
			break;

			// GUI -------------------
			case SDL_TEXTINPUT:
			{
				input_text.insert(input_text.size(), event.text.text);
			}
			break;
			// ------------------------

			// GamePads Events --------
			case SDL_CONTROLLERDEVICEADDED:
				AddController(event.cdevice.which);
				break;

			case SDL_CONTROLLERDEVICEREMOVED:
				RemoveController(event.cdevice.which);
				break;

			case SDL_CONTROLLERBUTTONDOWN:
			{
				for (std::vector<GamePad*>::iterator it = gamepads.begin(); it != gamepads.end(); it++)
				{
					if ((*it)->id == event.cbutton.which)
					{
						(*it)->gamecontroller_buttons[event.cbutton.button] = KEY_DOWN;
						break;
					}
				}
			}
			break;

			case SDL_CONTROLLERBUTTONUP:
			{
				for (std::vector<GamePad*>::iterator it = gamepads.begin(); it != gamepads.end(); it++)
				{
					if ((*it)->id == event.cbutton.which)
					{
						(*it)->gamecontroller_buttons[event.cbutton.button] = KEY_UP;
						break;
					}
				}
			}
			break;

			case SDL_CONTROLLERAXISMOTION:
			{
				AxisReaction(event.caxis.which, event.caxis.axis, event.caxis.value);
			}
			break;
			// ------------------------
			
		}
	}

	// GUI -------------------------
	SDL_StopTextInput();
	// -----------------------------

	return true;
}

// Called before quitting
bool j1Input::CleanUp()
{
	// Clean GamePads ------
	for (std::vector<GamePad*>::iterator it = gamepads.begin(); it != gamepads.end(); it++) {

		SDL_GameControllerClose((*it)->pad);
		RELEASE(*it);
	}
	gamepads.clear();

	SDL_QuitSubSystem(SDL_INIT_GAMECONTROLLER);
	// --------------------

	LOG("Quitting SDL event subsystem");
	SDL_QuitSubSystem(SDL_INIT_EVENTS);
	return true;
}

// ---------
bool j1Input::GetWindowEvent(j1EventWindow ev)
{
	return windowEvents[ev];
}

j1KeyState j1Input::GetControllerButton(int pad, int id) const
{
	for (std::vector<GamePad*>::const_iterator it = gamepads.begin(); it != gamepads.end(); it++) 
	{
		if ((*it)->id == gamepad_connected[pad]) {
			return (*it)->gamecontroller_buttons[id];
		}
	}
	return j1KeyState_null;
}

uint j1Input::GetControllerJoystickMove(int pad, int id) const
{
	for (std::vector<GamePad*>::const_iterator it = gamepads.begin(); it != gamepads.end(); it++)
	{
		if ((*it)->id == gamepad_connected[pad]) {
			return (*it)->joystick_moves[id];
		}
	}
	return 0;
}

void j1Input::GetMousePosition(int& x, int& y)
{
	x = mouse_x;
	y = mouse_y;
}

void j1Input::GetMouseMotion(int& x, int& y)
{
	x = mouse_motion_x;
	y = mouse_motion_y;
}

void j1Input::AxisReaction(int pad, int axis, int value)
{
	for (std::vector<GamePad*>::iterator it = gamepads.begin(); it != gamepads.end(); it++) 
	{
		if ((*it)->id == pad) {
			switch (axis)
			{
			case 0:
				if (value > 0)
				{
					(*it)->joystick_moves[LEFTJOY_RIGHT] = value;
				}
				else
				{
					(*it)->joystick_moves[LEFTJOY_LEFT] = -value;
				}
				break;
			case 1:
				if (value > 0)
				{
					(*it)->joystick_moves[LEFTJOY_DOWN] = value;
				}
				else
				{
					(*it)->joystick_moves[LEFTJOY_UP] = -value;
				}
				break;
			case 2:
				if (value > 0)
				{
					(*it)->joystick_moves[RIGHTJOY_RIGHT] = value;
				}
				else
				{
					(*it)->joystick_moves[RIGHTJOY_LEFT] = -value;
				}
				break;
			case 3:
				if (value > 0)
				{
					(*it)->joystick_moves[RIGHTJOY_DOWN] = value;
				}
				else
				{
					(*it)->joystick_moves[RIGHTJOY_UP] = -value;
				}
				break;
			case 4:
				(*it)->joystick_moves[LEFT_TRIGGER] = value;
				break;
			case 5:
				(*it)->joystick_moves[RIGHT_TRIGGER] = value;
				break;
			}
			break;
		}
	}
}

void j1Input::AddController(int id)
{
	if (SDL_IsGameController(id) && connected_gamepads < MAX_GAMECONTROLLERS) 
	{
		SDL_GameController *pad = SDL_GameControllerOpen(id);

		if (pad) 
		{
			SDL_Joystick *joy = SDL_GameControllerGetJoystick(pad);
			int instanceID = SDL_JoystickInstanceID(joy);
			GamePad* new_pad = new GamePad();
			new_pad->id = instanceID;
			ConnectGamePad(instanceID);
			memset(new_pad->gamecontroller_buttons, KEY_IDLE, sizeof(j1KeyState)*NUM_CONTROLLER_BUTTONS);
			memset(new_pad->joystick_moves, 0, sizeof(uint)*JOY_MOVES_NULL);
			new_pad->pad = pad;
			new_pad->pad_num = connected_gamepads;
			gamepads.push_back(new_pad);
			connected_gamepads++;
		}
	}
}

void j1Input::RemoveController(int id)
{
	for (std::vector<GamePad*>::iterator it = gamepads.begin(); it != gamepads.end() && gamepads.size()>0; it++)
	{
		if ((*it)->id == id)
		{
			DisconectGamePad(id);
			SDL_GameControllerClose((*it)->pad);
			RELEASE(*it);
			gamepads.erase(it);
			connected_gamepads--;
			break;
		}
	}
}

void j1Input::ConnectGamePad(int instanceID)
{
	for (int i = 0; i < MAX_GAMECONTROLLERS; i++) {
		if (gamepad_connected[i] == -1)
		{
			gamepad_connected[i] = instanceID;
			break;
		}
	}
}

void j1Input::DisconectGamePad(int instanceID)
{
	for (int i = 0; i < MAX_GAMECONTROLLERS; i++) {
		if (gamepad_connected[i] == instanceID)
		{
			gamepad_connected[i] = -1;
			break;
		}
	}
}

