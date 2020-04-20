#pragma once
#include "j1Module.h"

struct SDL_Texture;
class UiElement;

class j1WinScene : public j1Module
{
public:

	j1WinScene();

	// Destructor
	virtual ~j1WinScene();

	// Called before render is available
	bool Awake(pugi::xml_node& config);

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

	void Init();



private:

	SDL_Texture* Copy;
	SDL_Texture* background;
	SDL_Texture* wintext;

	bool Exit;


};