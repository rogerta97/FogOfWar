#ifndef _J1CONSOLE_
#define _J1CONSOLE_

#include "j1Module.h"
#include <string>
#include <list>

class UI_Window;
class UI_Text;
class UI_ColoredRect;
class UI_Scroll_Bar;
class UI_Text_Input;

class SDL_Color;

enum ConsoleTextType
{
	Output, Input, Error
};

class Command 
{
public:
	std::string command_str;
	std::string help;
	uint min_args = 0;
	uint max_args = 0;
private:
	j1Module* callback = nullptr;

public:
	Command(const char* txt, j1Module* callback, uint min_args, uint max_args, const char* help_txt);
	~Command();

	j1Module* GetCallback() const;
};

class CVar 
{
public:
	std::string cvar_str;
	std::string help;
	uint min_args = 0;
	uint max_args = 1;
private:
	j1Module* callback = nullptr;

public:
	CVar(const char* txt, j1Module* callback, const char* help_txt);
	~CVar();

	j1Module* GetCallback() const;
};

class j1Console :public j1Module 
{
public:
	j1Console();
	~j1Console();

	// Called when before render is available
	bool Awake(pugi::xml_node& config);

	// Call before first frame
	bool Start();

	// Called before all Updates
	bool PreUpdate();

	// Update Elements
	bool Update(float dt);

	// Called after all Updates
	bool PostUpdate();

	// Called before quitting
	bool CleanUp();

	bool Save(pugi::xml_node& node) const;

	void OnCommand(std::list<std::string>& tokens);

	void OnCVar(std::list<std::string>& tokens);

	void AddCommand(const char* command, j1Module* callback, uint min_args, uint max_args, const char* help_txt);

	void AddCVar(const char* cvar, j1Module* callback, const char* help_txt);

	void AddText(const char* txt, ConsoleTextType type = ConsoleTextType::Output);

	void ChangeCVarMaxArgs(const char* name, int args);

	// Loads logs and adds them to the console
	void LoadLogs();

public:
	bool                 ready = false;

private:
	std::list<Command*>  commands;
	std::list<CVar*>     cvars;

	std::list<UI_Text*>  labels;

	uint                 win_w = 0, win_h = 0;

	UI_Window*           window = nullptr;
	UI_ColoredRect*      console_background = nullptr;
	UI_ColoredRect*      console_background2 = nullptr;
	UI_Scroll_Bar*       scroll = nullptr;
	UI_ColoredRect*      input_background = nullptr;
	UI_Text_Input*       text_input = nullptr;
	UI_Text*             input_mark = nullptr;
	UI_ColoredRect*      button_h_background = nullptr;
	UI_ColoredRect*      button_v_background = nullptr;
	

	int currentLabel = -1;

private:
	void FastCommands();
	void Log(std::string text, SDL_Color color);

	bool FindCommand(std::string command, Command*& com) const;
	bool CheckCommandArguments(int num_args, Command* com) const;

	bool FindCVar(std::string cvar, CVar*& var) const;
	bool CheckCVarArguments(int num_args, CVar* var) const;

	bool stay_bottom = true;

};


#endif // !_J1CONSOLE_
