#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Textures.h"
#include "j1Render.h"
#include "j1Window.h"
#include "j1Scene.h"

j1Scene::j1Scene() : j1Module()
{
	name="scene";
}

// Destructor
j1Scene::~j1Scene()
{}

void j1Scene::Init() {
	enabled = true;
	active = true;
}

// Called before render is available
bool j1Scene::Awake(pugi::xml_node&config)
{
	LOG("Loading Scene");
	bool ret = true;
	return ret;
}

// Called before the first frame
bool j1Scene::Start()
{
	prova = App->tex->Load("Resources/maps/isometric_grass_and_water.png");
	return true;
}

// Called each loop iteration
bool j1Scene::PreUpdate()
{
	return true;
}

// Called each loop iteration
bool j1Scene::Update(float dt)
{
	bool ret = true;
	App->render->Blit(prova, 100, 100);
	return ret;
}

// Called each loop iteration
bool j1Scene::PostUpdate()
{
	bool ret = true;
	return ret;
}


// Called before quitting
bool j1Scene::CleanUp()
{
	LOG("Freeing scene");
	App->tex->UnLoad(prova);

	return true;
}
