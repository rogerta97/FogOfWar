#ifndef __j1INPUT_H__
#define __j1INPUT_H__

#include "j1Module.h"
#include <vector>

//#define NUM_KEYS 352
#define NUM_MOUSE_BUTTONS 5
//#define LAST_KEYS_PRESSED_BUFFER 50

#define MAX_GAMECONTROLLERS 4
#define NUM_CONTROLLER_BUTTONS 15

struct SDL_Rect;
struct _SDL_GameController;

enum JOYSTICK_MOVES
{
	LEFTJOY_LEFT = 0,
	LEFTJOY_RIGHT,
	LEFTJOY_UP,
	LEFTJOY_DOWN,
	RIGHTJOY_LEFT,
	RIGHTJOY_RIGHT,
	RIGHTJOY_UP,
	RIGHTJOY_DOWN,
	LEFT_TRIGGER,
	RIGHT_TRIGGER,
	JOY_MOVES_NULL
};

enum j1EventWindow
{
	WE_QUIT = 0,
	WE_HIDE = 1,
	WE_SHOW = 2,
	WE_COUNT
};

enum j1KeyState
{
	KEY_IDLE = 0,
	KEY_DOWN,
	KEY_REPEAT,
	KEY_UP,
	j1KeyState_null
};

struct GamePad {
	j1KeyState				gamecontroller_buttons[NUM_CONTROLLER_BUTTONS];
	uint					joystick_moves[JOY_MOVES_NULL];
	int						pad_num = -1;
	int						id = -1;
	_SDL_GameController*	pad = nullptr;
};

class j1Input : public j1Module
{

public:

	j1Input();

	// Destructor
	virtual ~j1Input();

	// Called before render is available
	bool Awake(pugi::xml_node&);

	// Called before the first frame
	bool Start();

	// Called each loop iteration
	bool PreUpdate();

	// Called before quitting
	bool CleanUp();

	// Gather relevant win events
	bool GetWindowEvent(j1EventWindow ev);

	// Check key states (includes mouse and joy buttons)
	j1KeyState GetKey(int id) const
	{
		return keyboard[id];
	}

	// Return mouse button state
	j1KeyState GetMouseButtonDown(int id) const
	{
		return mouse_buttons[id - 1];
	}

	// Return GameController button state
	j1KeyState GetControllerButton(int pad, int id) const;

	// Return the motion value (form 0 to 32767) for a joystick direction
	uint GetControllerJoystickMove(int pad, int id) const;

	// Check if a certain window event happened
	bool GetWindowEvent(int code);

	// Get mouse / axis position
	void GetMousePosition(int &x, int &y);
	void GetMouseMotion(int& x, int& y);

private:

	void AxisReaction(int pad, int axis, int value);
	void AddController(int id);
	void RemoveController(int id);

	void ConnectGamePad(int instanceID);
	void DisconectGamePad(int instanceID);

public:

	string		input_text;

private:

	bool		windowEvents[WE_COUNT];
	j1KeyState*	keyboard = nullptr;
	j1KeyState	mouse_buttons[NUM_MOUSE_BUTTONS];
	int			mouse_motion_x = 0;
	int			mouse_motion_y = 0;
	int			mouse_x = 0;
	int			mouse_y = 0;

	std::vector<GamePad*>	gamepads;
	int			connected_gamepads = 0;
	int			gamepad_connected[MAX_GAMECONTROLLERS];
};

#endif // __j1INPUT_H__