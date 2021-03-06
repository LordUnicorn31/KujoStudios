#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#include <math.h>
#include "Application.h"
#include "Transitions.h"
#include "Render.h"
#include "Window.h"
#include "SDL_render.h"
#include "SDL_timer.h"

Transitions::Transitions() : Module()
{
	name = "transitions";
}

Transitions::~Transitions()
{}

// Load assets
bool Transitions::Start()
{
	screen = { 0, 0, App->win->width * App->win->scale, App->win->height * App->win->scale };
	SDL_SetRenderDrawBlendMode(App->render->renderer, SDL_BLENDMODE_BLEND);
	return true;
}

// Update: draw background
bool Transitions::Update(float dt)
{
	if (current_step == fade_step::none)
		return true;

	Uint32 now = SDL_GetTicks() - startTime;
	float normalized = MIN(1.0f, (float)now / (float)totalTime);

	switch (current_step)
	{
	case fade_step::fade_to_black:
	{
		if (now >= totalTime)
		{
			moduleOff->Disable();
			moduleOn->Enable();
			totalTime += totalTime;
			startTime = SDL_GetTicks();
			current_step = fade_step::fade_from_black;
		}
	} break;

	case fade_step::fade_from_black:
	{

		normalized = 1.0f - normalized;

		if (now >= totalTime)
			current_step = fade_step::none;

	} break;

	case fade_step::slide_in:

		normalized = 1.0f;
		if (screen.x >= 0)
			screen.x = 0;
		else
			screen.x += (int)(((float)(screen.w) / (float)totalTime)* dt * (float)now * 2);

		if (now >= totalTime)
		{
			current_step = fade_step::slide_change;
		}

		break;

	case fade_step::slide_change:

		normalized = 1.0f;
		screen.x;
		moduleOff->Disable();
		moduleOn->Enable();
		totalTime += totalTime;
		startTime = SDL_GetTicks();
		current_step = fade_step::slide_out;

		break;

	case fade_step::slide_out:

		normalized = 1.0f;
		screen.x += (int)(((float)(screen.w) / (float)totalTime) * dt * (float)now * 2);
		if (now >= totalTime) {
			current_step = fade_step::none;
		}
			
		break;
	}

	SDL_SetRenderDrawColor(App->render->renderer, (Uint8)0, (Uint8)0, (Uint8)0, (Uint8)(normalized * 255.0f));
	SDL_RenderFillRect(App->render->renderer, &screen);

	return true;
}

// Fade to black. At mid point deactivate one Module, then activate the other
bool Transitions::FadeToBlack(Module* Module_off, Module* Module_on, float time)
{
	bool ret = false;
	if (current_step == fade_step::none)
	{
		screen.x = 0;
		current_step = fade_step::fade_to_black;
		startTime = SDL_GetTicks();
		totalTime = (Uint32)(time * 0.5f * 1000.0f);
		moduleOff = Module_off;
		moduleOn = Module_on;
		ret = true;
	}

	return ret;
}

bool Transitions::Slide(Module* Module_off, Module* Module_on, float time)
{
	bool ret = false;

	if (current_step == fade_step::none)
	{
		screen.x -= App->win->width;
		current_step = fade_step::slide_in;
		startTime = SDL_GetTicks();
		totalTime = (Uint32) (time *0.5* 1000.0f);
		moduleOff = Module_off;
		moduleOn = Module_on;
		ret = true;
	}
	return ret;
}



bool Transitions::IsFading() const
{
	return current_step != fade_step::none;
}

bool Transitions::CleanUp() {
	return true;
}
