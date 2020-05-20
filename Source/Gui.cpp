#include "p2Defs.h"
#include "p2Log.h"
#include "Application.h"
#include "Render.h"
#include "Textures.h"
#include "Fonts.h"
#include "Input.h"
#include "Gui.h"
#include "GameScene.h"
#include "Entity.h"
#include "Module.h"

Gui::Gui() : Module()
{
	name = "gui";
}

// Destructor
Gui::~Gui()
{}

// Called before render is available
bool Gui::Awake(pugi::xml_node& conf)
{
	LOG("Loading GUI atlas");
	bool ret = true;

	atlas_file_name = conf.child("atlas").attribute("file").as_string("");

	return ret;
}

// Called before the first frame
bool Gui::Start()
{
	atlas = App->tex->Load(atlas_file_name.data());
	focusedUi = nullptr;
	return true;
}

// Update all guis
bool Gui::PreUpdate()
{
	return true;
}

bool Gui::Update(float dt) {
	if (MouseClick() && UiUnderMouse() != nullptr && UiUnderMouse()->interactuable) {
		focusedUi = UiUnderMouse();
		if (focusedUi->module != nullptr && focusedUi->type!=UiTypes::EButton) {
			focusedUi->module->ui_callback(focusedUi);
		}
	}
	else if (MouseClick() && UiUnderMouse() == nullptr)
		focusedUi = nullptr;
	if (App->input->GetKey(SDL_SCANCODE_TAB) == KEY_DOWN)
		focusedUi = FocusNextElement(focusedUi);
	if (App->input->GetKey(SDL_SCANCODE_RETURN) == KEY_DOWN)
		if (focusedUi != nullptr && focusedUi->module != nullptr && focusedUi->type != UiTypes::EButton) {
			focusedUi->module->ui_callback(focusedUi);
		}
	Update_Ui();
	Draw_Ui();
	return true;
}

// Called after all Updates
bool Gui::PostUpdate()
{
	return true;
}

// Called before quitting
bool Gui::CleanUp()
{
	LOG("Freeing GUI");
	App->tex->UnLoad(atlas);
	DeleteAllUiElements();
	return true;
}

// const getter for atlas
const SDL_Texture* Gui::GetAtlas() const
{
	return atlas;
}

void Gui::DeleteAllUiElements() {
	eastl::list <UiElement*> ::iterator it;
	for (it = UiElementList.begin(); it != UiElementList.end(); it++)
	{
		delete (*it);
	}
	UiElementList.clear();
}

void Gui::RemoveUiElement(UiElement* element) {
	if (element != nullptr) {
		RemoveUiChilds(element);
		eastl::list<UiElement*>::iterator it = eastl::find(UiElementList.begin(), UiElementList.end(), element);
		if (it != UiElementList.end()) {
			delete (*it);
			UiElementList.erase(it);
		}
	}
}

void Gui::RemoveUiChilds(UiElement* element)
{
	eastl::list<UiElement*>::iterator it;
	for (it = UiElementList.begin(); it != UiElementList.end();) {
		if ((*it)->parent == element) {
			App->gui->RemoveUiChilds((*it));
			delete (*it);
			it = UiElementList.erase(it);
		}
		else
			it++;
	}
}

void Gui::Update_Ui() {
	int dx, dy;
	App->input->GetMouseMotion(dx, dy);
	eastl::list <UiElement*> ::iterator it;
	for (it = UiElementList.begin(); it != UiElementList.end(); it++)
	{
		(*it)->Update(dx, dy);
	}
}

void Gui::Draw_Ui() {
	eastl::list <UiElement*> ::iterator it;
	for (it = UiElementList.begin(); it != UiElementList.end(); it++)
	{
		(*it)->Draw(atlas);
	}
}

UiElement* Gui::UiUnderMouse() {
	int x, y;
	App->input->GetMousePosition(x, y);
	/*if (App->scene->IsEneabled()) {
		x -= App->render->camera.x;
		y -= App->render->camera.y;
	}*/
	UiElement*Element = nullptr;
	eastl::list <UiElement*> ::iterator it;
	for (it = UiElementList.begin(); it != UiElementList.end(); it++)
	{
		if ((*it)->GetScreenPos().x < x && x < (*it)->GetScreenPos().x + (*it)->GetScreenRect().w && (*it)->GetScreenPos().y < y && y < (*it)->GetScreenPos().y + (*it)->GetScreenRect().h && ((*it)->interactuable || (*it)->draggable))
			Element = (*it);
	}
	return Element;
}

bool Gui::MouseClick() {
	return (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_UP);
}

void Gui::DraggUiElements(UiElement*parent, int dx, int dy) {
	eastl::list <UiElement*> ::iterator it;
	for (it = UiElementList.begin(); it != UiElementList.end(); it++)
	{
		if ((*it)->parent == parent) {
			(*it)->SetLocalPos((*it)->GetLocalPos().x + dx, (*it)->GetLocalPos().y + dy);
			DraggUiElements((*it), dx, dy);
		}
	}
}

UiElement* Gui::FocusNextElement(UiElement*current_element) {
	if (current_element == nullptr) {
		eastl::list <UiElement*> ::iterator it;
		for (it = UiElementList.begin(); it != UiElementList.end(); it++)
		{
			if ((*it)->interactuable)
				return (*it);
		}
	}
	else {
		bool iteration = true;
		eastl::list <UiElement*> ::iterator item = eastl::find(UiElementList.begin(), UiElementList.end(), current_element);
		++item;
		for (; item != UiElementList.end(); item++)
		{
			if ((*item)->interactuable)
				return (*item);
			if (item.next() == UiElementList.end() && iteration) {
				iteration = false;
				item = UiElementList.begin();
				if ((*item)->interactuable)
					return (*item);
			}
		}
	}
	return nullptr;
}

UiElement* Gui::AddImage(int x, int y, SDL_Rect source_rect, bool interactuable, bool draggeable, UiElement* parent, Module* elementmodule) {
	UiElement* Image = new UiImage(x, y, source_rect, interactuable, draggeable, parent, elementmodule);
	UiElementList.push_back(Image);
	return Image;
}

UiElement* Gui::AddText(int x, int y, const char*text, _TTF_Font*font, SDL_Color color, int size, bool interactuable, bool draggeable, UiElement* parent, Module* elementmodule) {
	UiElement* Text = new UiText(x, y, text, size, color, interactuable, draggeable, font, parent, elementmodule);
	UiElementList.push_back(Text);
	return Text;
}

UiElement* Gui::AddButton(int x, int y, SDL_Rect source_unhover, SDL_Rect source_hover, SDL_Rect source_click, bool interactuable, bool draggeable, UiElement* parent, Module* elementmodule) {
	UiElement* Button = new UiButton(x, y, source_unhover, source_hover, source_click, interactuable, draggeable, parent, elementmodule);
	UiElementList.push_back(Button);
	return Button;
}

UiElement* Gui::AddEntityButton(int x, int y, SDL_Rect source_unhover, SDL_Rect source_hover, SDL_Rect source_click,AviableEntities entity, EntityType etype, bool interactuable, bool draggeable, UiElement* parent, Module* elementmodule) {
	UiElement* EButton = new UiEntityButton(x, y, source_unhover, source_hover, source_click, entity, etype, interactuable, draggeable, parent, elementmodule);
	UiElementList.push_back(EButton);
	return EButton;
}

UiElement* Gui::AddHUDBar(int x, int y, int MaxValue, float* valueptr, bool usecamera, SDL_Rect bar, SDL_Rect fill, SDL_Rect border, bool interactuable, bool draggeable, UiElement* parent, Module* elementmodule) {
	UiElement* HUD = new UiHUDBars(x, y, MaxValue, valueptr, usecamera, bar, fill, border, interactuable, draggeable, parent, elementmodule);
	UiElementList.push_back(HUD);
	return HUD;
}

UiElement* Gui::AddSlider(int x, int y, bool active, bool draggable, UiElement* parent, Module* elementmodule, int sliderposition) {
	UiElement* Slider = new UiSlider(x, y, active, draggable, parent, elementmodule);
	UiElementList.push_back(Slider);
	return Slider;
}


UiElement::UiElement(int x, int y, int w, int h, bool interactuable, bool draggeable, UiTypes uitype, UiElement* parent, Module* elementmodule) : type(uitype), parent(parent), module(elementmodule), ui_rect({ x,y,w,h }), interactuable(interactuable), draggable(draggeable) { if (parent != nullptr)SetLocalPos(x, y); }

UiElement::~UiElement() {};

const iPoint UiElement::GetScreenPos() {
	iPoint position(ui_rect.x, ui_rect.y);
	return position;
}

const iPoint UiElement::GetLocalPos() {
	if (parent == nullptr) {
		iPoint position(ui_rect.x, ui_rect.y);
		return position;
	}
	else {
		iPoint position(ui_rect.x - parent->GetScreenPos().x, ui_rect.y - parent->GetScreenPos().y);
		return position;
	}
}

const SDL_Rect UiElement::GetScreenRect() {
	return ui_rect;
}

const SDL_Rect UiElement::GetLocalRect() {
	return { GetLocalPos().x,GetLocalPos().y,ui_rect.w,ui_rect.h };
}

void UiElement::SetLocalPos(int x, int y) {
	if (parent == nullptr) {
		ui_rect.x = x; ui_rect.y = y;
	}
	else {
		ui_rect.x = parent->GetScreenPos().x + x;
		ui_rect.y = parent->GetScreenPos().y + y;
	}
}bool UiElement::outofparent() {
	return (GetScreenPos().x + GetScreenRect().w*0.5f<parent->GetScreenPos().x || GetScreenPos().x>parent->GetScreenPos().x + parent->GetScreenRect().w - GetScreenRect().w*0.5 || GetScreenPos().y + GetScreenRect().h*0.5f<parent->GetScreenPos().y || GetScreenPos().y>parent->GetScreenPos().y + parent->GetScreenRect().h - GetScreenRect().h*0.5f);
}



UiImage::UiImage(int x, int y, SDL_Rect source_rect, bool interactuable, bool draggeable, UiElement* parent, Module* elementmodule) :UiElement(x, y, source_rect.w, source_rect.h, interactuable, draggeable, UiTypes::Image, parent, elementmodule), atlas_rect(source_rect) {}
UiImage::~UiImage() {}

void UiImage::Update(int dx, int dy) {
	//fer que la imatge es mogui amb la camera
	if (App->input->GetKey(SDL_SCANCODE_RETURN) == KEY_DOWN && App->gui->focusedUi == this)
		App->gui->focusedUi = nullptr;
	if (App->gui->MouseClick() && App->gui->focusedUi == this)
		App->gui->focusedUi = nullptr;
	if (draggable && App->gui->MouseClick() && App->gui->UiUnderMouse() == this && dx != 0 && dy != 0) {
		SetLocalPos(GetLocalPos().x + dx, GetLocalPos().y + dy);
		App->gui->DraggUiElements(this, dx, dy);
	}
}

void UiImage::Draw(SDL_Texture* atlas) {
	if (parent == nullptr || !outofparent())
		App->render->Blit(atlas, GetScreenPos().x, GetScreenPos().y, &atlas_rect,false);
}

UiText::UiText(int x, int y, const char*text, int size, SDL_Color color, bool interactuable, bool draggeable, _TTF_Font*font, UiElement* parent, Module* elementmodule) : UiElement(x, y, size, size, interactuable, draggeable, UiTypes::Text, parent, elementmodule), font_type(font), message(text), color(color), texture(App->font->Print(message.c_str(), color, font_type)) {}
UiText::~UiText() { App->tex->UnLoad(texture); }

void UiText::Draw(SDL_Texture* atlas) {
	if (parent == nullptr || !outofparent())
		App->render->Blit(texture, GetScreenPos().x, GetScreenPos().y, NULL,false);
}

void UiText::Update(int dx, int dy) {
	//fer que el text es mogui amb la camera
	if (App->input->GetKey(SDL_SCANCODE_RETURN) == KEY_DOWN && App->gui->focusedUi == this)
		App->gui->focusedUi = nullptr;
	if (App->gui->MouseClick() && App->gui->focusedUi == this)
		App->gui->focusedUi = nullptr;
	if (draggable && App->gui->MouseClick() && App->gui->UiUnderMouse() == this && dx != 0 && dy != 0) {
		SetLocalPos(GetLocalPos().x + dx, GetLocalPos().y + dy);
		App->gui->DraggUiElements(this, dx, dy);
	}
}

void UiText::ChangeMessage(const char* newmessage) {
	App->tex->UnLoad(texture);
	message = newmessage;
	texture = App->font->Print(message.c_str(), color, font_type);
}

void UiText::ChangeColor(SDL_Color newcolor) {
	App->tex->UnLoad(texture);
	color = newcolor;
	texture = App->font->Print(message.c_str(), color, font_type);
}

UiButton::UiButton(int x, int y, SDL_Rect source_unhover, SDL_Rect source_hover, SDL_Rect source_click, bool interactuable, bool draggeable, UiElement* parent, Module* elementmodule) :UiElement(x, y, source_unhover.w, source_unhover.h, interactuable, draggeable, UiTypes::Button, parent, elementmodule), unhover(source_unhover), hover(source_hover), click(source_click), current_state(Button_state::unhovered) {}
UiButton::~UiButton() {}

void UiButton::Update(int dx, int dy) {
	if (App->gui->focusedUi == this)
		current_state = Button_state::hovered;
	else if (App->gui->UiUnderMouse() == this)
		current_state = Button_state::hovered;
	else
		current_state = Button_state::unhovered;
	if (App->gui->MouseClick() && App->gui->focusedUi == this) {
		current_state = Button_state::clicked;
		App->gui->focusedUi = nullptr;
	}
	if (App->input->GetKey(SDL_SCANCODE_RETURN) == KEY_DOWN && App->gui->focusedUi == this) {
		current_state = Button_state::clicked;
		App->gui->focusedUi = nullptr;
	}
	if (draggable && App->gui->MouseClick() && App->gui->UiUnderMouse() == this && dx != 0 && dy != 0) {
		SetLocalPos(GetLocalPos().x + dx, GetLocalPos().y + dy);
		App->gui->DraggUiElements(this, dx, dy);
	}
}

void UiButton::Draw(SDL_Texture*atlas) {
	if (parent == nullptr || !outofparent()) {
		switch (current_state) {
		case Button_state::unhovered:
			App->render->Blit(atlas, GetScreenPos().x, GetScreenPos().y, &unhover,false);
			break;
		case Button_state::hovered:
			App->render->Blit(atlas, GetScreenPos().x, GetScreenPos().y, &hover,false);
			break;
		case Button_state::clicked:
			App->render->Blit(atlas, GetScreenPos().x, GetScreenPos().y, &click,false);
			break;
		}
	}
}

UiEntityButton::UiEntityButton(int x, int y, SDL_Rect source_unhover, SDL_Rect source_hover, SDL_Rect source_selected,AviableEntities entity,EntityType etype, bool interactuable, bool draggeable, UiElement* parent, Module* elementmodule) :UiElement(x, y, source_unhover.w, source_unhover.h, interactuable, draggeable, UiTypes::EButton, parent, elementmodule), unhover(source_unhover), hover(source_hover), click(source_selected), entity(entity), current_state(Button_state::unhovered), entitytype(etype), selected(false) {}

UiEntityButton::~UiEntityButton(){}

void UiEntityButton::Update(int dx, int dy) {
	if (App->gui->focusedUi == this) {
		if (selected)
			current_state = Button_state::clicked;
		else
			current_state = Button_state::hovered;
	}
	else if (App->gui->UiUnderMouse() == this) {
		if (selected)
			current_state = Button_state::clicked;
		else
			current_state = Button_state::hovered;
	}
	else {
		if (selected)
			current_state = Button_state::clicked;
		else
			current_state = Button_state::unhovered;
	}
	if ((App->gui->MouseClick() || App->input->GetKey(SDL_SCANCODE_RETURN) == KEY_DOWN) && App->gui->focusedUi == this) {
		selected = !selected;
		if (selected) {
			current_state = Button_state::clicked;
			module->ui_callback(this);
		}
		else {
			current_state = Button_state::unhovered;
			App->gui->focusedUi = nullptr;
		}
	}
	else if ((App->gui->MouseClick() || App->input->GetKey(SDL_SCANCODE_RETURN) == KEY_DOWN) && App->gui->focusedUi != this) {
		selected = false;
		current_state = Button_state::unhovered;
	}
	if (draggable && App->gui->MouseClick() && App->gui->UiUnderMouse() == this && dx != 0 && dy != 0) {
		SetLocalPos(GetLocalPos().x + dx, GetLocalPos().y + dy);
		App->gui->DraggUiElements(this, dx, dy);
	}
}

void UiEntityButton::Draw(SDL_Texture* atlas) {
	if (parent == nullptr || !outofparent()) {
		switch (current_state) {
		case Button_state::unhovered:
			App->render->Blit(atlas, GetScreenPos().x, GetScreenPos().y, &unhover, false);
			break;
		case Button_state::hovered:
			App->render->Blit(atlas, GetScreenPos().x, GetScreenPos().y, &hover, false);
			break;
		case Button_state::clicked:
			App->render->Blit(atlas, GetScreenPos().x, GetScreenPos().y, &click, false);
			break;
		}
	}
}

UiHUDBars::UiHUDBars(int x, int y, uint MaxValue,float*valueptr, bool usecamera, SDL_Rect bar, SDL_Rect fill, SDL_Rect border, bool interactuable, bool draggeable, UiElement* parent, Module* elementmodule): UiElement(x, y, bar.w, bar.h, interactuable, draggeable, UiTypes::HUDBar, parent, elementmodule), Border(border), Fill(fill), FullBar(bar), Value(valueptr), MaxValue(MaxValue),CurrentBar(bar),UseCamera(usecamera) {}

UiHUDBars::~UiHUDBars() {}

void UiHUDBars::Update(int dx, int dy) {
	CurrentBar.w = (int)((*Value) / ((float)MaxValue/(float)FullBar.w));
	if (CurrentBar.w < 0)
		CurrentBar.w = 0;
}

void UiHUDBars::Draw(SDL_Texture* atlas) {
	App->render->Blit(atlas, GetScreenPos().x, GetScreenPos().y, &Border, UseCamera);
	App->render->Blit(atlas, GetScreenPos().x+1, GetScreenPos().y+1, &Fill, UseCamera);
	App->render->Blit(atlas, GetScreenPos().x+1, GetScreenPos().y+1, &CurrentBar, UseCamera);
}

UiSlider::UiSlider(int x, int y, bool active, bool draggable,UiElement* parent, Module* elementmodule, int sliderposition) : UiElement(x, y, bar.w, bar.h, active, draggable, UiTypes::Slider, parent, elementmodule) {

	clickable_rect = { GetScreenPos().x -46, GetScreenPos().y, 250, 40 };

	thumb_offset = 46;

	sliderpos = sliderposition;

	texture = App->tex->Load("gui/UI_Slider.png");

	bar = { 1280, 560, 170, 15 };
	thumb = { 1280, 580, 30, 30 };
	thumb_hovered = { 1280, 580, 30, 30 };

	clickable_rect = { GetScreenPos().x -46, GetScreenPos().y, 250, 40 };
};

UiSlider::~UiSlider() {};

void UiSlider::Update(int dx, int dy) {

	clickable_rect = { GetScreenPos().x -46, GetScreenPos().y , 250, 40 };
	hovered = false;
	int x, y;//Mouse coords

	App->input->GetMousePosition(x, y);

	if (x > clickable_rect.x && x < clickable_rect.x + clickable_rect.w
		&& y > clickable_rect.y && y < clickable_rect.y + clickable_rect.h)
	{
		if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_REPEAT || App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_DOWN)
		{
			hovered = true;
			sliderpos = x - GetScreenPos().x - thumb_offset;
			if (sliderpos < 0)	sliderpos = 0;
			else if (sliderpos > 152) sliderpos = 152;
		}
	}
}

void UiSlider::Draw(SDL_Texture* atlas) {

	App->render->Blit(atlas, GetScreenPos().x ,GetScreenPos().y, &bar, false);

	if (hovered) {
		App->render->Blit(atlas, GetScreenPos().x + thumb_offset + sliderpos -46, GetScreenPos().y -4, &thumb_hovered, false);
	}
	else App->render->Blit(atlas, GetScreenPos().x + thumb_offset + sliderpos -46, GetScreenPos().y -4, &thumb, false);

	if (debug)
	{
		App->render->DrawQuad(clickable_rect, 0, 255, 255, 50, true, false);
	}
}

float UiSlider::SliderValue() {

	return ((float)sliderpos / 152.0f);
}
