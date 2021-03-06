#pragma once

#include "EASTL/string.h"
#include "pugixml.hpp"


enum update_status
{
	UPDATE_CONTINUE = 1,
	UPDATE_STOP,
	UPDATE_ERROR
};

struct Collider;
class App;
class UiElement;

class Module
{
public:

	Module() : active(false)
	{}

	virtual void Init()
	{
		active = true;
	}

	// Called before render is available
	virtual bool Awake(pugi::xml_node&)
	{
		return true;
	}

	// Called before the first frame
	virtual bool Start()
	{
		return true;
	}

	// Called each loop iteration
	virtual bool PreUpdate()
	{
		return true;
	}

	// Called each loop iteration
	virtual bool Update(float dt)
	{
		return true;
	}

	// Called each loop iteration
	virtual bool PostUpdate()
	{
		return true;
	}

	// Called before quitting
	virtual bool CleanUp()
	{
		return true;
	}

	virtual bool Load(pugi::xml_node&)
	{
		return true;
	}

	virtual bool Save(pugi::xml_node&)
	{
		return true;
	}

	virtual void ui_callback(UiElement*element) {

	}
	void Enable()
	{
		if (enabled == false)
		{
			enabled = true;
			Start();
		}
	}

	void Disable()
	{
		if (enabled) {
			enabled = false;
			CleanUp();
		}
	}

	const bool IsEneabled() {
		return enabled;
	}


public:

	eastl::string	name;
	bool		active;
	virtual void OnCollision(Collider*, Collider*) {};
protected:
	bool enabled = true;

};