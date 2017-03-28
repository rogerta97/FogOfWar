#ifndef __j1SCENE_H__
#define __j1SCENE_H__

#include "j1Module.h"
#include "j1Gui.h"
#include "Scene.h"

struct SDL_Texture;
class MainScene;

class j1Scene : public j1Module
{
public:

	j1Scene();

	// Destructor
	virtual ~j1Scene();

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

	// Change scene
	void ChangeScene(Scene* new_scene);

	// Blit choosing the layer
	void OnCollision(PhysBody* bodyA, PhysBody* bodyB, b2Fixture* fixtureA, b2Fixture* fixtureB);

	void OnCommand(std::list<std::string>& tokens);

	void OnCVar(std::list<std::string>& tokens);

	void SaveCVar(std::string& cvar_name, pugi::xml_node& node) const;

public:
	// Scenes
	MainScene*           main_scene = nullptr;

private:
	// Scenes list
	list<Scene*>         scenes;

	// Current scene
	Scene*               current_scene = nullptr;

};

#endif // __j1SCENE_H__