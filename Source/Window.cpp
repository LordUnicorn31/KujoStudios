#include "p2Defs.h"
#include "p2Log.h"
#include "Application.h"
#include "Window.h"
#include "icon_64x64.c"

#include "SDL.h"


Window::Window() : Module()
{
	window = NULL;
	screenSurface = NULL;
	name="window";
}

// Destructor
Window::~Window()
{
}

// Called before render is available
bool Window::Awake(pugi::xml_node& config)
{
	LOG("Init SDL window & surface");
	bool ret = true;

	if(SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		LOG("SDL_VIDEO could not initialize! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}
	else
	{
		//Create window
		Uint32 flags = SDL_WINDOW_SHOWN;
		fullscreen = config.child("fullscreen").attribute("value").as_bool(false);
		bool borderless = config.child("borderless").attribute("value").as_bool(false);
		bool resizable = config.child("resizable").attribute("value").as_bool(false);
		bool fullscreen_window = config.child("fullscreen_window").attribute("value").as_bool(false);

		width = config.child("resolution").attribute("width").as_int();
		height = config.child("resolution").attribute("height").as_int();
		scale = config.child("resolution").attribute("scale").as_int(1);

		if(fullscreen == true)
		{
			flags |= SDL_WINDOW_FULLSCREEN;
		}

		if(borderless == true)
		{
			flags |= SDL_WINDOW_BORDERLESS;
		}

		if(resizable == true)
		{
			flags |= SDL_WINDOW_RESIZABLE;
		}

		if(fullscreen_window == true)
		{
			flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
		}

		window = SDL_CreateWindow(App->GetTitle(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, flags);
	
		Uint32 rmask, gmask, bmask, amask;
		rmask = 0x000000ff;
		gmask = 0x0000ff00;
		bmask = 0x00ff0000;
		amask = (icon_64x64.bytes_per_pixel == 3) ? 0 : 0xff000000;

		SDL_Surface* icon = SDL_CreateRGBSurfaceFrom((void*)icon_64x64.pixel_data,
			icon_64x64.width, icon_64x64.height, icon_64x64.bytes_per_pixel * 8,
			icon_64x64.bytes_per_pixel * icon_64x64.width, rmask, gmask, bmask, amask);

		SetIcon(icon);

		SDL_FreeSurface(icon);

		if(window == NULL)
		{
			LOG("Window could not be created! SDL_Error: %s\n", SDL_GetError());
			ret = false;
		}
		else
		{
			//Get window surface
			screenSurface = SDL_GetWindowSurface(window);
		}
	}

	return ret;
}


// Called before quitting
bool Window::CleanUp()
{
	LOG("Destroying SDL window and quitting all SDL systems");

	//Destroy window
	if(window != NULL)
	{
		SDL_DestroyWindow(window);
	}

	//Quit SDL subsystems
	SDL_Quit();
	return true;
}

// Set new window title
void Window::SetTitle(const char* new_title)
{
	//title.create(new_title);
	SDL_SetWindowTitle(window, new_title);
}

void Window::SetIcon(SDL_Surface* new_icon)
{
	SDL_SetWindowIcon(window, new_icon);
}

void Window::GetWindowSize(uint& width, uint& height) const
{
	width = this->width;
	height = this->height;
}

uint Window::GetScale() const
{
	return scale;
}

int Window::GetWidth() const
{
	return width;
}

int Window::GetHeight() const
{
	return height;
}

void Window::Fullscreen()
{
	if (fullscreen == false) {
		Uint32 flags = SDL_WINDOW_SHOWN;
		flags = SDL_WINDOW_FULLSCREEN;
		SDL_SetWindowFullscreen(window, flags);
		fullscreen = true;
	}
	else {
		SDL_SetWindowFullscreen(window, 0);
		fullscreen = false;
	}


}
