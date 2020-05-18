#include "j1Module.h"

struct SDL_Texture;


class j1Scene : public j1Module
{
public:

	j1Scene();

	// Destructor
	virtual ~j1Scene();

	// Called before render is available
	bool Awake(pugi::xml_node&config);

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

	void Tutorial();

	void ui_callback(UiElement* element);

	void cameramovement(float time);

	bool tutorialActive;

	UiElement* Info;
private:
	UiElement* tutorialImage;
	UiElement* tutorialTxt;
	SDL_Rect tutorialRect;

	UiElement* questImage;
	UiElement* questOne;
	UiElement* questTwo;
	UiElement* questThree;
	UiElement* questSquare;
	UiElement* questCheck;
	UiElement* questInfoImage;
	UiElement* questInfo;

	bool questOneActive;
	bool questTwoActive;
	bool questThreeActive;

	UiElement* Pause;
	UiElement* People;
	UiElement* Copper;
	UiElement* Titanium;
	UiElement* Settings_window;
	UiElement* Exit_button;
	UiElement* Continue_button;
	UiElement* fullScreen;
	UiElement* SaveButton;
	UiElement* options;
	UiElement* OptionsMenu;
	UiElement* BackButton;
	UiElement* FxSlider;
	UiElement* MusSlider;
	int buttonFx;
	bool exitGame;
	iPoint mousepos;
	int camspeed;
	uint width;
	uint height;

	float startTime;
	float currentTime;
};