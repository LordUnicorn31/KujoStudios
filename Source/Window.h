#pragma once

#include "Module.h"

struct SDL_Window;
struct SDL_Surface;

class Window : public Module
{
public:

	Window();

	// Destructor
	virtual ~Window();

	// Called before render is available
	bool Awake(pugi::xml_node&);

	// Called before quitting
	bool CleanUp();

	// Changae title
	void SetTitle(const char* new_title);

	// Change icon
	void SetIcon(SDL_Surface* new_icon);

	// Retrive window size
	void GetWindowSize(uint& width, uint& height) const;

	// Retrieve window scale
	uint GetScale() const;

	int GetWidth() const;

	int GetHeight() const;

	void Fullscreen();

public:
	//The window we'll be rendering to
	SDL_Window* window;

	//The surface contained by the window
	SDL_Surface* screenSurface;
	eastl::string	title;

	int		        width;
	int		        height;
	int		        scale;

	bool fullscreen;
};