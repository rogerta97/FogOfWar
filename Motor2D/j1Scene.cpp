#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Input.h"
#include "j1Textures.h"
#include "j1Audio.h"
#include "j1Render.h"
#include "j1Window.h"
#include "j1Scene.h"
#include "MainScene.h"
#include "j1Console.h"

#define NUMBER_OF_PLAYERS 4

j1Scene::j1Scene() : j1Module()
{
	name = "scene";
}

// Destructor
j1Scene::~j1Scene()
{

}

// Called before render is available
bool j1Scene::Awake()
{
	bool ret = true;

	LOG("Loading SceneManager");

	return ret;
}

// Called before the first frame
bool j1Scene::Start()
{
	bool ret = false;

	LOG("Start module scene");

	// Create scenes
	main_scene = new MainScene();
	scenes.push_back(main_scene);
	// -------------

	// Starting scene
	current_scene = main_scene;

	if(current_scene != nullptr)
		ret = current_scene->Start();

	return ret;
}

// Called each loop iteration
bool j1Scene::PreUpdate()
{
	bool ret = false;

	if (current_scene != nullptr)
		ret = current_scene->PreUpdate();

	return ret;
}

// Called each loop iteration
bool j1Scene::Update(float dt)
{
	bool ret = false;

	if (current_scene != nullptr)
		ret = current_scene->Update(dt);
	
	return ret;
}

// Called each loop iteration
bool j1Scene::PostUpdate()
{
	bool ret = false;

	if (current_scene != nullptr)
		ret = current_scene->PostUpdate();

	if(App->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
		ret = false;

	return ret;
}

// Called before quitting
bool j1Scene::CleanUp()
{
	LOG("Freeing scene");

	bool ret = false;

	if (current_scene != nullptr)
		ret = current_scene->CleanUp();

	return ret;
}

void j1Scene::ChangeScene(Scene * new_scene)
{
	LOG("Changing current scene");

	current_scene->CleanUp();
	current_scene = new_scene;
	current_scene->Start();
}


void j1Scene::OnCollision(PhysBody * bodyA, PhysBody * bodyB, b2Fixture * fixtureA, b2Fixture * fixtureB)
{
	for(list<Scene*>::iterator it = scenes.begin(); it != scenes.end(); it++)
		(*it)->OnColl(bodyA, bodyB, fixtureA, fixtureB);
}

void j1Scene::OnCommand(std::list<std::string>& tokens)
{
	current_scene->OnCommand(tokens);
}

void j1Scene::OnCVar(std::list<std::string>& tokens)
{
	current_scene->OnCVar(tokens);
}

void j1Scene::SaveCVar(std::string & cvar_name, pugi::xml_node & node) const
{
	current_scene->SaveCVar(cvar_name,node);
}



