#include <iostream> 
#include <sstream> 
#include "p2Defs.h"
#include "p2Log.h"
#include "j1Window.h"
#include "j1Input.h"
#include "j1Render.h"
#include "j1Textures.h"
#include "j1Audio.h"
#include "j1Scene.h"
#include "j1FileSystem.h"
#include "j1Map.h"
#include "j1Pathfinding.h"
#include "j1Fonts.h"
#include "j1Gui.h"
#include "j1App.h"
#include "j1Console.h"
#include "j1Physics.h"
#include "j1Entity.h"
#include "j1Viewports.h"

// Constructor
j1App::j1App(int argc, char* args[]) : argc(argc), args(args)
{
	PERF_START(ptimer);

	input = new j1Input();
	win = new j1Window();
	render = new j1Render();
	tex = new j1Textures();
	audio = new j1Audio();
	scene = new j1Scene();
	fs = new j1FileSystem();
	map = new j1Map();
	pathfinding = new j1PathFinding();
	font = new j1Fonts();
	gui = new j1Gui();
	console = new j1Console();
	physics = new j1Physics();
	entity = new j1Entity();
	view = new j1Viewports();

	// Ordered for awake / Start / Update
	// Reverse order of CleanUp
	AddModule(fs);
	AddModule(input);
	AddModule(win);
	AddModule(tex);
	AddModule(audio);
	AddModule(map);
	AddModule(pathfinding);
	AddModule(font);
	AddModule(physics);
	AddModule(console);
	AddModule(entity);
	AddModule(view);

	// Scene
	AddModule(scene);

	// Gui
	AddModule(gui);

	// render last to swap buffer
	AddModule(render);

	console->AddCommand("quit", console, 0, 0, "Exit application");
	console->AddCommand("save", console, 0, 0, "Save data");
	console->AddCommand("cap_fps", console, 0, 1, "Cap fps. Min_args: 0 Max_args: 1 Args: num > 0");

	cf = new collision_filters();
	PERF_PEEK(ptimer);
}

// Destructor
j1App::~j1App()
{
	// release modules
	for (list<j1Module*>::iterator it = modules.begin(); it != modules.end(); it++)
		RELEASE(*it);
	
	modules.clear();
}

void j1App::AddModule(j1Module* module)
{
	module->Init();
	modules.push_back(module);
}

// Called before render is available
bool j1App::Awake()
{
	PERF_START(ptimer);

	pugi::xml_document	config_file;
	pugi::xml_node		config;
	pugi::xml_node		app_config;

	bool ret = false;
		
	config = LoadConfig(config_file);

	if(config.empty() == false)
	{
		// self-config
		ret = true;
		app_config = config.child("app");
		title = app_config.child("title").attribute("name").as_string();
		organization = app_config.child("organization").child_value();

		int cap = app_config.attribute("framerate_cap").as_int(-1);

		CapFps(cap);
	}

	if(ret == true)
	{
		for (list<j1Module*>::iterator it = modules.begin(); it != modules.end(); it++)
			ret = (*it)->Awake(config.child((*it)->name.c_str()));
		
	}

	PERF_PEEK(ptimer);

	return ret;
}

// Called before the first frame
bool j1App::Start()
{
	PERF_START(ptimer);
	bool ret = true;

	for (list<j1Module*>::iterator it = modules.begin(); it != modules.end(); it++)
		ret = (*it)->Start();
	

	startup_time.Start();

	debug_mode = true;

	debug_window = (UI_Window*)App->gui->UI_CreateWin(iPoint(0, 0), 200, 115, 1);
	debug_colored_rect = (UI_ColoredRect*)debug_window->CreateColoredRect(iPoint(0, 0), 200, 115, { 20, 20, 20, 255 }, true);
	debug_text = (UI_Text*)debug_window->CreateText(iPoint(5, 5), App->font->default_15, 15);

	PERF_PEEK(ptimer);

	return ret;
}

// Called each loop iteration
bool j1App::Update()
{
	bool ret = true;
	PrepareUpdate();

	if (App->input->GetKey(SDL_SCANCODE_F1) == KEY_DOWN)
		debug_mode = !debug_mode;

	if (App->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
		ret = false;

	if(input->GetWindowEvent(WE_QUIT) == true || end_program)
		ret = false;

	if(ret == true)
		ret = PreUpdate();

	if(ret == true)
		ret = DoUpdate();

	if(ret == true)
		ret = PostUpdate();

	FinishUpdate();
	return ret;
}

// ---------------------------------------------
pugi::xml_node j1App::LoadConfig(pugi::xml_document& config_file) const
{
	pugi::xml_node ret;

	char* buf;
	int size = App->fs->Load("config.xml", &buf);
	pugi::xml_parse_result result = config_file.load_buffer(buf, size);
	RELEASE(buf);

	if(result == NULL)
		LOG("Could not load map xml file config.xml. pugi error: %s", result.description());
	else
		ret = config_file.child("config");

	return ret;
}

// ---------------------------------------------
void j1App::PrepareUpdate()
{
	frame_count++;
	last_sec_frame_count++;

	dt = frame_time.ReadSec();
	frame_time.Start();
}

// ---------------------------------------------
void j1App::FinishUpdate()
{
	if(want_to_save == true)
		SavegameNow();

	if(want_to_load == true)
		LoadGameNow();

	FrameRateCalculations();
}

// Call modules before each loop iteration
bool j1App::PreUpdate()
{
	bool ret = true;

	for (list<j1Module*>::iterator it = modules.begin(); it != modules.end(); it++)
	{
		if((*it)->active)
			ret = (*it)->PreUpdate();
	}

	return ret;
}

// Call modules on each loop iteration
bool j1App::DoUpdate()
{
	bool ret = true;

	for (list<j1Module*>::iterator it = modules.begin(); it != modules.end(); it++)
	{
		if ((*it)->active)
			ret = (*it)->Update(dt);
	}

	return ret;
}

// Call modules after each loop iteration
bool j1App::PostUpdate()
{
	bool ret = true;

	for (list<j1Module*>::iterator it = modules.begin(); it != modules.end(); it++)
	{
		if ((*it)->active)
			ret = (*it)->PostUpdate();
	}

	return ret;
}

// Called before quitting
bool j1App::CleanUp()
{
	PERF_START(ptimer);
	bool ret = true;

	delete cf;

	// Cleaning up in reverse order
	for (list<j1Module*>::reverse_iterator it = modules.rbegin(); it != modules.rend(); it++)
		ret = (*it)->CleanUp();

	PERF_PEEK(ptimer);

	return ret;
}

// ---------------------------------------
int j1App::GetArgc() const
{
	return argc;
}

// ---------------------------------------
const char* j1App::GetArgv(int index) const
{
	if(index < argc)
		return args[index];
	else
		return NULL;
}

// ---------------------------------------
const char* j1App::GetTitle() const
{
	return title.c_str();
}

// ---------------------------------------
float j1App::GetDT() const
{
	return dt;
}

// ---------------------------------------
const char* j1App::GetOrganization() const
{
	return organization.c_str();
}

// Load / Save
void j1App::LoadGame(const char* file)
{
	// we should be checking if that file actually exist
	// from the "GetSaveGames" list
	want_to_load = true;
	load_game.create("%s%s", fs->GetSaveDirectory(), file);
}

// ---------------------------------------
void j1App::SaveGame(const char* file) const
{
	// we should be checking if that file actually exist
	// from the "GetSaveGames" list ... should we overwrite ?

	want_to_save = true;
	save_game.create(file);
}

// ---------------------------------------
void j1App::GetSaveGames(p2List<p2SString>& list_to_fill) const
{
	// need to add functionality to file_system module for this to work
}

void j1App::LoadXML(const char * path, pugi::xml_document & doc)
{
	char* buf = NULL;
	int size = App->fs->Load(path, &buf);
	pugi::xml_parse_result result = doc.load_buffer(buf, size);
	
	if (buf == NULL)
		LOG("Error loading '%s', probably wrong XML file name", path);
	else
		LOG("Succes loading '%s'", path);

	RELEASE(buf);
}

void j1App::CapFps(float fps)
{
	if(fps > 0)
		capped_ms = (1000 / fps);
}

bool j1App::LoadGameNow()
{
	bool ret = false;

	char* buffer;
	uint size = fs->Load(load_game.GetString(), &buffer);

	if(size > 0)
	{
		pugi::xml_document data;
		pugi::xml_node root;

		pugi::xml_parse_result result = data.load_buffer(buffer, size);
		RELEASE(buffer);

		if(result != NULL)
		{
			LOG("Loading new Game State from %s...", load_game.GetString());

			root = data.child("game_state");

			list<j1Module*>::iterator it;
			for (it = modules.begin(); it != modules.end(); it++)
			{
				ret = (*it)->Load(root.child((*it)->name.c_str()));
			}

			data.reset();
			if(ret == true)
				LOG("...finished loading");
			else
				LOG("...loading process interrupted with error on module %s", (*it)->name.c_str());

		}
		else
			LOG("Could not parse game state xml file %s. pugi error: %s", load_game.GetString(), result.description());
	}
	else
		LOG("Could not load game state xml file %s", load_game.GetString());

	want_to_load = false;
	return ret;
}

bool j1App::SavegameNow() const
{
	bool ret = true;

	LOG("Saving Game State to %s...", save_game.GetString());

	// xml object were we will store all data
	pugi::xml_document data;
	pugi::xml_node root;
	
	root = data.append_child("game_state");
	
	list<j1Module*>::const_iterator it;
	for (it = modules.begin(); it != modules.end(); it++)
	{
		ret = (*it)->Save(root.child((*it)->name.c_str()));
	}

	if(ret == true)
	{
		std::stringstream stream;
		data.save(stream);

		// we are done, so write data to disk
		fs->Save(save_game.GetString(), stream.str().c_str(), stream.str().length());
		LOG("... finished saving", save_game.GetString());
	}
	else
		LOG("Save process halted from an error in module %s", (*it)->name.c_str());

	data.reset();
	want_to_save = false;
	return ret;
}

void j1App::FrameRateCalculations()
{
	if (last_sec_frame_time.Read() > 1000)
	{
		last_sec_frame_time.Start();
		prev_last_sec_frame_count = last_sec_frame_count;
		last_sec_frame_count = 0;
	}

	float avg_fps = float(frame_count) / startup_time.ReadSec();
	float seconds_since_startup = startup_time.ReadSec();
	uint32 last_frame_ms = frame_time.Read();
	uint32 frames_on_last_update = prev_last_sec_frame_count;

	static char title[256];
	p2SString str("FPS: %i \nAv.FPS: %.2f \nLast Frame Ms: %u \nLast dt: %.3f \nTime since startup: %.2f \nFrame Count: %lu", frames_on_last_update, avg_fps, last_frame_ms, dt, seconds_since_startup, frame_count);
	string t = str.GetString();
	debug_text->SetText(t);

	if (capped_ms > 0 && last_frame_ms < capped_ms)
	{
		j1PerfTimer t;
		SDL_Delay(capped_ms - last_frame_ms);
	}

	if (debug_mode && !debug_window->enabled)
		debug_window->SetEnabledAndChilds(true);
	if(!debug_mode && debug_window->enabled)
		debug_window->SetEnabledAndChilds(false);
}

void j1App::EndSDL()
{
	end_program = true;
}
