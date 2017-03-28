#include "j1Console.h"
#include "p2Defs.h"
#include "p2Point.h"
#include "j1Window.h"
#include "j1FileSystem.h"
#include "p2Log.h"
#include "j1Gui.h"
#include "j1App.h"
#include "j1Render.h"
#include "j1Input.h"
#include "Functions.h"
#include <sstream>

#define PADDING 15
#define OUTPUT_TEXT_COLOR { 0, 178, 255, 255 }
#define INPUT_TEXT_COLOR {235, 235, 235, 255}
#define ERROR_TEXT_COLOR { 255, 67, 67, 255 }
#define TOP_FRAME_SIZE 30
#define FRAMES_SIZE 15
#define SCROLL_BUTTON_SIZE 15
#define MAX_LABELS 40

#define CONSOLE_COLOR_1 { 30, 30, 30, 195 }
#define CONSOLE_COLOR_2 {60, 60, 60, 255}

// -------------------------------------------------
//                  Console
// -------------------------------------------------

j1Console::j1Console()
{
	name = "console";
}

j1Console::~j1Console()
{
}

bool j1Console::Awake(pugi::xml_node &config)
{
	return true;
}

bool j1Console::Start()
{
	LOG("Starting Console");
	
	App->win->GetWindowSize(win_w, win_h);

	window = App->gui->UI_CreateWin(iPoint(App->render->camera.x, App->render->camera.y),win_w, win_h/3, 2);

	console_background = window->CreateColoredRect(iPoint(window->rect.x, window->rect.y), window->rect.w, window->rect.h, CONSOLE_COLOR_1);
	console_background2 = window->CreateColoredRect(iPoint(window->rect.x + FRAMES_SIZE, window->rect.y + TOP_FRAME_SIZE), window->rect.x + window->rect.w - (FRAMES_SIZE * 2) - 15, win_h / 3 - FRAMES_SIZE - TOP_FRAME_SIZE, CONSOLE_COLOR_1);

	scroll = window->CreateScrollBar(iPoint(window->rect.x + FRAMES_SIZE, window->rect.y + TOP_FRAME_SIZE), window->rect.x + window->rect.w - (FRAMES_SIZE * 2) - 15, win_h / 3 - FRAMES_SIZE - TOP_FRAME_SIZE, SCROLL_BUTTON_SIZE);

	button_v_background = window->CreateColoredRect(iPoint(scroll->button_v->rect.x, scroll->min_bar_v), scroll->button_v->rect.w, scroll->rect.h, CONSOLE_COLOR_2);
	button_v_background->click_through = true;
	button_h_background = window->CreateColoredRect(iPoint(scroll->button_v->rect.x, scroll->button_v->rect.y), scroll->button_v->rect.w, scroll->button_v->rect.h, CONSOLE_COLOR_2);
	button_h_background->click_through = true;

	input_background = window->CreateColoredRect(iPoint(window->rect.x, scroll->rect.y + scroll->rect.h + FRAMES_SIZE), window->rect.w, FRAMES_SIZE * 2.5f, CONSOLE_COLOR_1);
	text_input = window->CreateTextInput(iPoint(window->rect.x + FRAMES_SIZE * 1.5f, scroll->rect.y + scroll->rect.h + (FRAMES_SIZE*1.5f)), window->rect.w - FRAMES_SIZE * 4, App->font->default_15);
	input_mark = window->CreateText(iPoint(10, scroll->rect.y + scroll->rect.h + (FRAMES_SIZE*1.5f)), App->font->default_15);
	input_mark->SetText(">");

	window->SetEnabledAndChilds(false);

	ready = true;

	AddCommand("list", this, 0, 2, "List all commands and/or cvars. Min_args: 0 Max_args: 2 Args: commands, cvars, [searchword]");
	AddCommand("clear", this, 0, 0, "Cleans console prompt. Min_args: 0 Max_args: 0");

	return true;
}

bool j1Console::PreUpdate()
{
	bool ret = true;

	return ret;
}

bool j1Console::Update(float dt)
{
	FastCommands();

	// Open/close console
	if (App->input->GetKey(SDL_SCANCODE_GRAVE) == KEY_DOWN)
	{
		window->SetEnabledAndChilds(!window->enabled);
		text_input->Clear();
		text_input->active = true;
	}

	// Enable/disable scroll buttons
	if (scroll->button_v->rect.h == scroll->button_starting_v || !scroll->parent->enabled)
		button_v_background->enabled = false;
	else
		button_v_background->enabled = true;

	if (scroll->button_h->rect.w == scroll->button_starting_h || !scroll->parent->enabled)
		button_h_background->enabled = false;
	else
		button_h_background->enabled = true;

	// Update size of scroll buttons
	button_v_background->rect = { scroll->button_v->rect.x, scroll->button_v->rect.y, scroll->button_v->rect.w, scroll->button_v->rect.h };
	button_h_background->rect = { scroll->button_h->rect.x, scroll->button_h->rect.y, scroll->button_h->rect.w, scroll->button_h->rect.h };

	// Scroll always down
	if (scroll->button_v->MouseClickEnterLeft())
		stay_bottom = false;

	if (stay_bottom)
		scroll->button_v->rect.y = scroll->rect.y + scroll->rect.h - scroll->button_v->rect.h;

	// Load logs
	LoadLogs();

	return true;
}

bool j1Console::PostUpdate()
{
	bool ret = true;

	return ret;
}

bool j1Console::CleanUp()
{
	bool ret = true;

	return ret;
}

bool j1Console::Save(pugi::xml_node & node) const
{
	//Console save all the parameters on the config.xml

	pugi::xml_document	config_file;
	pugi::xml_node		config;

	App->LoadXML("config.xml", config_file);

	config = config_file.child("config");

	for (std::list<CVar*>::const_iterator cvar = cvars.begin(); cvar != cvars.end(); cvar++) 
	{
		std::list<std::string> tokens;
		Tokenize((*cvar)->cvar_str, '.', tokens);

		if (tokens.size() < 2) 
			continue;
		
		pugi::xml_node node = config.child(tokens.front().c_str());
		if (node == NULL) 
			node = config.append_child(tokens.front().c_str());

		std::list<std::string>::iterator it = tokens.begin();
		it++;
		pugi::xml_node parameter = node.child((*it).c_str());
		if (parameter == NULL) 
			parameter = node.append_child((*it).c_str());

		(*cvar)->GetCallback()->SaveCVar((*it), parameter); //Each module is responsible to save their cvars
	}
	config_file.save_file("config.xml");

	return true;
}

void j1Console::OnCommand(std::list<std::string>& tokens)
{
	std::list<std::string>::iterator it = tokens.begin();
	switch (tokens.size())
	{
	case 1:
		if ((*it) == "list") {
			AddText("--- COMMANDS ---", Output);
			for (std::list<Command*>::iterator item = commands.begin(); item != commands.end(); item++){
				std::ostringstream oss;
				oss << (*item)->command_str.c_str() << ": " << (*item)->help.c_str() << ".";
				std::string command_text = oss.str();
				AddText(command_text.c_str(), Output);
			}
			AddText("     --- CVARS ---", Output);
			for (std::list<CVar*>::iterator item = cvars.begin(); item != cvars.end(); item++) {
				std::ostringstream oss;
				oss << (*item)->cvar_str.c_str() << ": " << (*item)->help.c_str() << ".";
				std::string cvar_text = oss.str();
				AddText(cvar_text.c_str(), Output);
			}
		}
		else if ((*it) == "quit") {
			App->EndSDL();
			ready = false;
		}
		else if ((*it) == "save") {
			App->SaveGame("config.xml");
		}
		else if ((*it) == "clear") {
			scroll->ClearElements();
			labels.clear();
			currentLabel = -1;
		}
		break;
	case 2:
		if ((*it) == "list") {
			it++;
			if ((*it) == "commands") {
				for (std::list<Command*>::iterator item = commands.begin(); item != commands.end(); item++) {
					std::ostringstream oss;
					oss << (*item)->command_str.c_str() << ": " << (*item)->help.c_str() << ".";
					std::string command_text = oss.str();
					AddText(command_text.c_str(), Output);
				}
			}
			else if ((*it) == "cvars") {
				for (std::list<CVar*>::iterator item = cvars.begin(); item != cvars.end(); item++) {
					std::ostringstream oss;
					oss << (*item)->cvar_str.c_str() << ": " << (*item)->help.c_str() << ".";
					std::string cvar_text = oss.str();
					AddText(cvar_text.c_str(), Output);
				}
			}
			else {
				AddText("--- COMMANDS ---", Output);
				for (std::list<Command*>::iterator item = commands.begin(); item != commands.end(); item++) {
					if((*item)->command_str.find((*it).c_str()) != std::string::npos){
						std::ostringstream oss;
						oss << (*item)->command_str.c_str() << ": " << (*item)->help.c_str() << ".";
						std::string command_text = oss.str();
						AddText(command_text.c_str(), Output);
					}
				}
				AddText("     --- CVARS ---", Output);
				for (std::list<CVar*>::iterator item = cvars.begin(); item != cvars.end(); item++) {
					if((*item)->cvar_str.find((*it).c_str()) != std::string::npos){
						std::ostringstream oss;
						oss << (*item)->cvar_str.c_str() << ": " << (*item)->help.c_str() << ".";
						std::string cvar_text = oss.str();
						AddText(cvar_text.c_str(), Output);
					}
				}
			}
		}
		else if ((*it) == "cap_fps")
		{
			it++;
			float value = atof((*it).c_str());
			if (value > 0)
			{
				App->CapFps(value);
				std::ostringstream oss;
				oss.precision(3);
				oss<<fixed;
				oss << "FPS limited to " << value;
				std::string str = oss.str();
				AddText(str.c_str());
			}
			else
				AddText("Invalid framerate", ConsoleTextType::Error);
		}
	case 3:
		if ((*it) == "list") {
			it++;
			if ((*it) == "commands") {
				it++;
				for (std::list<Command*>::iterator item = commands.begin(); item != commands.end(); item++) {
					if ((*item)->command_str.find((*it).c_str()) != std::string::npos) {
						std::ostringstream oss;
						oss << (*item)->command_str.c_str() << ": " << (*item)->help.c_str() << ".";
						std::string command_text = oss.str();
						AddText(command_text.c_str(), Output);
					}
				}
			}
			else if ((*it) == "cvars") {
				it++;
				for (std::list<CVar*>::iterator item = cvars.begin(); item != cvars.end(); item++) {
					if ((*item)->cvar_str.find((*it).c_str()) != std::string::npos) {
						std::ostringstream oss;
						oss << (*item)->cvar_str.c_str() << ": " << (*item)->help.c_str() << ".";
						std::string cvar_text = oss.str();
						AddText(cvar_text.c_str(), Output);
					}
				}
			}
		}
		break;
	default:
		break;
	}
}

void j1Console::OnCVar(std::list<std::string>& tokens)
{
	std::list<std::string>::iterator it = tokens.begin();
	switch (tokens.size())
	{
	case 2:
		
		break;
	}
}

// Create a new command for the console. 
void j1Console::AddCommand(const char * command, j1Module * callback, uint min_args, uint max_args, const char* help_txt)
{
	std::string name(command);

	ToLowerCase(name);

	Command* a;
	if(!FindCommand(name, a))
		commands.push_back(new Command(name.c_str(), callback, min_args, max_args, help_txt));
}

// Create a new cvar for the console. 
void j1Console::AddCVar(const char * cvar, j1Module * callback, const char* help_txt)
{
	std::string name(cvar);

	ToLowerCase(name);

	CVar* a;
	if (!FindCVar(name, a))
		cvars.push_back(new CVar(name.c_str(), callback, help_txt));
}

// Add text to console. 
// txt: text console should add.
// type: input (check commands and cvars) or output (console return message).
void j1Console::AddText(const char * txt, ConsoleTextType type)
{
	if (txt[0] != '\0' && ready) {
		switch (type)
		{
		case Output: 
		{
			std::string text(txt);
			std::list<std::string> tokens;
			Tokenize(text, '\n', tokens);

			for (std::list<std::string>::iterator token = tokens.begin(); token != tokens.end(); token++) 
			{
				if ((*token).length() > 1) 
				{
					Log((*token).c_str(), OUTPUT_TEXT_COLOR);
				}
			}
			tokens.clear();
		}
			break;
		case Input:
		{
			bool command_found = false;
			bool command_valid = false;
			Command* command = nullptr;

			bool cvar_found = false;
			bool cvar_valid = false;
			CVar* cvar = nullptr;

			std::string text(txt);
			std::list<std::string> tokens;
			Tokenize(text, ' ', tokens);

			ToLowerCase(tokens.front());

			command_found = FindCommand(tokens.front(), command);

			if (command_found) 
				command_valid = CheckCommandArguments(tokens.size() - 1, command);
			else
			{
				cvar_found = FindCVar(tokens.front(), cvar);
				if (cvar_found) cvar_valid = CheckCVarArguments(tokens.size() - 1, cvar);
			}

			if ((command_found && command_valid) || (cvar_found && cvar_valid)) {
				Log(txt, INPUT_TEXT_COLOR);

				if (command_found) 
					command->GetCallback()->OnCommand(tokens);
				else if (cvar_found) 
					cvar->GetCallback()->OnCVar(tokens);
				
			}
			else 
			{
				if (!command_found && !cvar_found) {
					std::ostringstream oss;
					oss << "Error: " << text << " don't exist.";
					Log(std::string(oss.str()), ERROR_TEXT_COLOR);
				}
					
				else if (!command_valid || !cvar_valid)
				{
					if (command_found)
						Log("Error: Invalid command arguments.", ERROR_TEXT_COLOR);
					else if (cvar_found) 
						Log("Error: Invalid cvar arguments.", ERROR_TEXT_COLOR);
				}
			}	
		}
		break;
		case Error:
		{
			std::string text(txt);
			std::list<std::string> tokens;
			Tokenize(text, '\n', tokens);

			for (std::list<std::string>::iterator token = tokens.begin(); token != tokens.end(); token++) 
			{
				if ((*token).length() > 1) 
					Log((*token), ERROR_TEXT_COLOR);
			}
			tokens.clear();
		}
		break;

		default:
			break;
		}

		// Delete labels when more than defined
		if (labels.size() > MAX_LABELS)
		{
			scroll->DeleteScrollElement(scroll->elements.begin()->element);
			labels.remove(labels.front());

			// Change to STD when UI is done
			for (list<scroll_element>::iterator it = scroll->elements.begin(); it != scroll->elements.end(); it++)
			{
				(*it).starting_pos_y -= 20;
				(*it).element->rect.y -= 20;
			}
		}
	}
}

void j1Console::ChangeCVarMaxArgs(const char* name, int args)
{
	for (std::list<CVar*>::iterator cvar = cvars.begin(); cvar != cvars.end(); cvar++) 
	{
		if ((*cvar)->cvar_str == name) 
		{
			(*cvar)->max_args = args;
			break;
		}
	}
}

void j1Console::LoadLogs()
{
	for(list<string>::iterator it = App->logs.begin(); it != App->logs.end(); it++)
		AddText((*it).c_str());
	
	App->logs.clear();
}

void j1Console::FastCommands()
{
	if (text_input->active)
	{
		if (text_input->intern_text.size() > 0)
		{
			if (App->input->GetKey(SDL_SCANCODE_RETURN) == KEY_DOWN)
			{
				AddText(text_input->intern_text.c_str(), Input);
				text_input->Clear();
			}

			// Get first command from list more similar to actual input text 
			if (App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_DOWN)
			{
				bool found = false;
				for (std::list<Command*>::iterator command = commands.begin(); command != commands.end(); command++)
				{
					if ((*command)->command_str.find(text_input->intern_text.c_str()) != std::string::npos)
					{
						found = true;
						text_input->SetTextInput((*command)->command_str.c_str());
						break;
					}
				}
				if (!found)
				{
					for (std::list<CVar*>::iterator cvar = cvars.begin(); cvar != cvars.end(); cvar++)
					{
						if ((*cvar)->cvar_str.find(text_input->intern_text.c_str()) != std::string::npos)
						{
							text_input->SetTextInput((*cvar)->cvar_str.c_str());
							break;
						}
					}
				}
			}
		}

		// Get last command from the commands used
		if (App->input->GetKey(SDL_SCANCODE_UP) == KEY_DOWN)
		{
			if (currentLabel == -1)
				currentLabel = labels.size()-1;

			// iterator backwards does not work, must change, code crash
			int index = currentLabel;
			std::list<UI_Text*>::iterator entry = labels.begin();
			std::advance(entry, currentLabel);
			for (; entry != labels.begin(); entry--)
			{
				if ((*entry)->color.r == 235 && (*entry)->color.g == 235 && (*entry)->color.b == 235 && (*entry)->color.a == 255)
				{
					if (labels.size() == 1)
					{
						text_input->SetTextInput((*entry)->GetText());
					}
					else
					{
						if (currentLabel != index)
						{
							text_input->SetTextInput((*entry)->GetText());
							currentLabel = index;
							break;
						}
					}
				}
				index--;
			}
		}

		// Get first command from the commands used
		if (App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_DOWN)
		{
			if (currentLabel == -1)
				currentLabel = 0;

			int index = currentLabel;
			std::list<UI_Text*>::iterator entry = labels.begin();
			std::advance(entry, currentLabel);
			for (; entry != labels.end(); entry++)
			{
				if ((*entry)->color.r == 235 && (*entry)->color.g == 235 && (*entry)->color.b == 235 && (*entry)->color.a == 255)
				{
					if (labels.size() == 1)
					{
						text_input->SetTextInput((*entry)->GetText());
					}
					else
					{
						if (currentLabel != index)
						{
							text_input->SetTextInput((*entry)->GetText());
							currentLabel = index;
							break;
						}
					}
				}
				index++;
			}
		}
	}
}

// Print to console
void j1Console::Log(std::string text, SDL_Color color)
{
	UI_Text* new_label = new UI_Text();
	new_label->Set(iPoint(PADDING * 2, labels.size() * 20), App->font->default_15, 15);
	new_label->color = color;
	new_label->SetText(text.c_str());
	labels.push_back(new_label);
	scroll->AddElement(new_label);
	stay_bottom = true;
}

// Return true if command exist and fill pos with the index on the list.
bool j1Console::FindCommand(std::string command, Command*& com) const
{
	bool ret = false;

	int i = 0;
	for (std::list<Command*>::const_iterator item = commands.begin(); item != commands.end(); item++)
	{
		if ((*item)->command_str == command) 
		{
			ret = true;
			com = *item;
			break;
		}
	}

	return ret;
}

// Check if the number of arguments inputed is correct for the command.
bool j1Console::CheckCommandArguments(int num_args, Command* com) const
{
	std::list<Command*>::const_iterator it = find(commands.begin(), commands.end(), com);
	return (num_args <= (*it)->max_args && num_args >= (*it)->min_args);
}

// Return true if cvar exist and fill pos with the index on the list.
bool j1Console::FindCVar(std::string cvar, CVar*& var) const
{
	bool ret = false;

	for (std::list<CVar*>::const_iterator item = cvars.begin(); item != cvars.end(); item++) 
	{
		if ((*item)->cvar_str == cvar) 
		{
			ret = true;
			var = *item;
			break;
		}
	}

	return ret;
}

// Check if the number of arguments inputed is correct for the cvar.
bool j1Console::CheckCVarArguments(int num_args, CVar* var) const
{
	std::list<CVar*>::const_iterator it = find(cvars.begin(), cvars.end(), var);
	return (num_args <= (*it)->max_args && num_args >= (*it)->min_args);
}

// -------------------------------------------------
//                  Command
// -------------------------------------------------

Command::Command(const char * txt, j1Module * callback, uint min_args, uint max_args, const char* help_txt) : command_str(txt), callback(callback), min_args(min_args), max_args(max_args), help(help_txt)
{
}

Command::~Command()
{
}

// Return callback module
j1Module * Command::GetCallback() const
{
	return callback;
}

// -------------------------------------------------
//                    CVar
// -------------------------------------------------

CVar::CVar(const char * txt, j1Module * callback, const char* help_txt) : cvar_str(txt), callback(callback), help(help_txt)
{
}

CVar::~CVar()
{
}

// Return callback module
j1Module * CVar::GetCallback() const
{
	return callback;
}