#include "EntityManager.h"
#include "Building.h"
#include "Input.h"
#include "Application.h"
#include "Audio.h"
#include "Gui.h"
#include "Ai.h"
#include "Collisions.h"
#include "Fonts.h"
#include "Animation.h"

Building::Building(BuildingType type, iPoint Position): Entity(EntityType::TypeBuilding, { Position.x,Position.y,0,0 }), Btype(type),ConstructionTime(0.0f),level(1),OnConstruction(true), ToBuild(false), BuildingEntity(nullptr), BuildHUD(nullptr) {
	switch (type) {
	case BuildingType::Base:
		EntityRect.w = 96;
		EntityRect.h = 96;
		health = 5000;
		selected = false;
		IdleAnimation.PushBack({ 0,64,96,96 });
		IdleAnimation.PushBack({ 96,64,96,96 });
		IdleAnimation.PushBack({ 192,64,96,96 });
		IdleAnimation.speed = 2.0f;
		ConstructionAnimation = IdleAnimation;
		level = 1;
		OnConstruction = true;
		ConstructionTime = 0.0f;
		break;
	case BuildingType::Mine:
		EntityRect.w = 64;
		EntityRect.h = 64;
		health = 1500;
		selected = false;
		IdleAnimation.PushBack({ 0,0,64,64 });
		IdleAnimation.PushBack({ 64,0,64,64 });
		IdleAnimation.PushBack({ 128,0,64,64 });
		IdleAnimation.PushBack({ 192,0,64,64 });
		IdleAnimation.PushBack({ 256,0,64,64 });
		IdleAnimation.PushBack({ 320,0,64,64 });
		IdleAnimation.PushBack({ 384,0,64,64 });
		IdleAnimation.PushBack({ 448,0,64,64 });
		IdleAnimation.PushBack({ 0,64,64,64 });
		IdleAnimation.PushBack({ 64,64,64,64 });
		IdleAnimation.PushBack({ 128,64,64,64 });
		IdleAnimation.speed = 2.0f;
		ConstructionAnimation.PushBack({ 256,64,64,64 });
		level = 1;
		ConstructionTime = 10.0f;
		OnConstruction = true;
		break;
	case BuildingType::Cuartel:
		EntityRect.w = 64;
		EntityRect.h = 64;
		health = 300;
		selected = false;
		IdleAnimation.PushBack({ 126,64,64,64 });
		IdleAnimation.PushBack({ 0,0,64,64 });
		IdleAnimation.PushBack({ 126,64,64,64 });
		IdleAnimation.PushBack({ 128,0,64,64 });
		IdleAnimation.PushBack({ 126,64,64,64 });
		IdleAnimation.PushBack({ 0,64,64,64 });
		ConstructionAnimation.PushBack({ 64,0,64,64 });
		level = 1;
		ConstructionTime = 10.0f;
		OnConstruction = true;
		break;
	case BuildingType::Spaceship_factory:
		EntityRect.w = 64;
		EntityRect.h = 64;
		health = 3500;
		selected = false;
		IdleAnimation.PushBack({ 64,250,64,64 });
		IdleAnimation.PushBack({ 128,250,64,64 });
		ConstructionAnimation.PushBack({ 0,250,64,64 });
		level = 1;
		ConstructionTime = 10.0f;
		OnConstruction = true;
		break;
	case BuildingType::PowerGenerator:
		EntityRect.w = 64;
		EntityRect.h = 64;
		health = 3000;
		selected = false;
		IdleAnimation.PushBack({ 0,0,64,64 });
		IdleAnimation.PushBack({ 64,0,64,64 });
		IdleAnimation.speed = 4.0f;
		ConstructionAnimation.PushBack({ 128,0,64,64 });
		level = 1;
		ConstructionTime = 10.0f;
		OnConstruction = true;
		break;
	}
	collider = App->collisions->AddCollider(EntityRect,COLLIDER_ALLY,App->entity, (Entity*)this);
}

Building::~Building() {

}

void Building::Update(float dt) {
	if (health <= 0) {
		//die
	}

	if (OnConstruction) {
		selectable = false;
		ConstructionTime -= dt;
		if (ConstructionTime <= 0) {
			OnConstruction = false;
			selectable = true;
		}
	}
	if (!BuildingQueue.empty()) {
		if (BuildingEntity == nullptr) {
			if (ToBuild)
				BuildingEntity = (Ai*)App->entity->CreateEntity(BuildingQueue.front(), iPoint(EntityRect.x, EntityRect.y));
		}
		else if (!BuildingEntity->Building) {
			BuildingQueue.pop_front();
			BuildingEntity = (Ai*)App->entity->CreateEntity(BuildingQueue.front(), iPoint(EntityRect.x, EntityRect.y));
			if (BuildingQueue.empty()) {
				ToBuild = false;
				BuildingEntity = nullptr;
			}
		}
		//IF I DIE, KILL THE ENTITY IM BUILDING
	}
	if (health <= 0) {
		if (collider != nullptr)
			collider->toDelete = true;
		todie = true;
	}
}

void Building::Draw(float dt) {
	if (OnConstruction) {
		//App->audio->PlayFx(App->audio->LoadFx("Resources/audio/fx/Building.wav"));
		App->render->Blit(sprite, EntityRect.x, EntityRect.y, &ConstructionAnimation.GetCurrentFrame(dt));
	}
	else {
		App->render->Blit(sprite, EntityRect.x, EntityRect.y, &IdleAnimation.GetCurrentFrame(dt));
		if (selected) {
			App->render->DrawQuad(EntityRect, 0, 255, 0, 255, false);
		}
	}
	if (selected) {
		App->render->DrawQuad(EntityRect, 0, 255, 0, 255, false);
	}
}

/*void Building::Die() {

}*/

void Building::LevelUp() {

}

void Building::UiFunctionallity() {
	switch (Btype) {
	case BuildingType::Cuartel:
		App->gui->AddText(18, 240, "Barracks", App->font->defaultFont, { 16, 173, 171,255 }, 32, false, false, false, App->entity->Panel);
		App->gui->AddText(70, 310, "Lazer", App->font->smallFont, { 255,255,255,255 }, 32, false, false, false, App->entity->Panel);
		App->gui->AddText(70, 390, "Dual", App->font->smallFont, { 255,255,255,255 }, 32, false, false, false, App->entity->Panel);
		App->gui->AddText(70, 465, "Assault", App->font->smallFont, { 255,255,255,255 }, 32, false, false, false, App->entity->Panel);
		App->gui->AddEntityButton(20, 300, { 1344,84,39,39 }, { 1290,83,39,39 }, { 1397,83,39,39 },AviableEntities::redship, EntityType::TypeAi, true, false, false, App->entity->Panel, App->entity);
		App->gui->AddEntityButton(20, 375, { 1345,28,39,39 }, { 1291,28,39,39 }, { 1397,28,39,39 }, AviableEntities::blueship, EntityType::TypeAi, true, false, false, App->entity->Panel, App->entity);
		App->gui->AddEntityButton(20, 450, { 1344,199,39,39 }, { 1290,198,39,39 }, { 1397,198,39,39 }, AviableEntities::greenship, EntityType::TypeAi, true, false, false, App->entity->Panel, App->entity);
		if (!BuildingQueue.empty()) {
			switch (BuildingEntity->Atype) {
			case AiType::RedShip:
				BuildHUD = App->gui->AddHUDBar(9, 341, BuildingEntity->TotalBuildingTime, &BuildingEntity->BuildingTime, { 1279,431,61,5 }, { 1279,399,61,5 }, { 1278,382,64,8 }, false, false, false, App->entity->Panel, nullptr);
				break;
			case AiType::BlueShip:
				BuildHUD = App->gui->AddHUDBar(9, 416, BuildingEntity->TotalBuildingTime, &BuildingEntity->BuildingTime, { 1279,431,61,5 }, { 1279,399,61,5 }, { 1278,382,64,8 }, false, false, false, App->entity->Panel, nullptr);
				break;
			case AiType::GreenShip:
				BuildHUD = App->gui->AddHUDBar(9, 491, BuildingEntity->TotalBuildingTime, &BuildingEntity->BuildingTime, { 1279,431,61,5 }, { 1279,399,61,5 }, { 1278,382,64,8 }, false, false, false, App->entity->Panel, nullptr);
				break;
			}
		}
		break;
	case BuildingType::Base:
	    App->gui->AddText(56, 240, "Base", App->font->defaultFont, { 16, 173, 171,255 }, 32, false, false, false, App->entity->Panel);
		App->gui->AddText(70, 310, "Builder", App->font->smallFont, { 255,255,255,255 }, 32, false, false, false, App->entity->Panel);
		App->gui->AddEntityButton(20, 300, { 1344,136,39,39 }, { 1291,135,39,39 }, { 1397,136,39,39 },AviableEntities::collector, EntityType::TypeAi, true, false, false, App->entity->Panel, App->entity);
		if(!BuildingQueue.empty())
			BuildHUD = App->gui->AddHUDBar(9,341, BuildingEntity->TotalBuildingTime, &BuildingEntity->BuildingTime, { 1279,431,61,5 }, { 1279,399,61,5 }, { 1278,382,64,8 }, false, false, false, App->entity->Panel, nullptr);
		break;
	case BuildingType::Mine:
		App->gui->AddText(56, 240, "Mine", App->font->defaultFont, { 16, 173, 171,255 }, 32, false, false, false,App->entity->Panel);
		break;
	case BuildingType::Spaceship_factory:
		App->gui->AddText(66, 240, "Lab", App->font->defaultFont, { 16, 173, 171,255 }, 32, false, false, false, App->entity->Panel);
		App->gui->AddText(70, 310, "Lazer 2.0", App->font->smallFont, { 255,255,255,255 }, 32, false, false, false, App->entity->Panel);
		App->gui->AddText(70, 390, "Dual 2.0", App->font->smallFont, { 255,255,255,255 }, 32, false, false, false, App->entity->Panel);
		App->gui->AddText(70, 465, "Assault 2.0", App->font->smallFont, { 255,255,255,255 }, 32, false, false, false, App->entity->Panel);
		App->gui->AddEntityButton(20, 300, { 1344,84,39,39 }, { 1290,83,39,39 }, { 1397,83,39,39 }, AviableEntities::upgradedredship, EntityType::TypeAi, true, false, false, App->entity->Panel, App->entity);
		App->gui->AddEntityButton(20, 375, { 1345,28,39,39 }, { 1291,28,39,39 }, { 1397,28,39,39 }, AviableEntities::upgradedblueship, EntityType::TypeAi, true, false, false, App->entity->Panel, App->entity);
		App->gui->AddEntityButton(20, 450, { 1344,199,39,39 }, { 1290,198,39,39 }, { 1397,198,39,39 }, AviableEntities::upgradedgreenship, EntityType::TypeAi, true, false, false, App->entity->Panel, App->entity);
		break;
	case BuildingType::PowerGenerator:
		App->gui->AddText(16, 240, "Generator", App->font->defaultFont, { 16, 173, 171,255 }, 32, false, false, false, App->entity->Panel);
		break;
	}
}

void Building::UpdateUiFunctionallity() {
	App->gui->RemoveUiElement(BuildHUD);
	switch (Btype) {
	case BuildingType::Base:
		if (!BuildingQueue.empty())
			BuildHUD = App->gui->AddHUDBar(9, 341, BuildingEntity->TotalBuildingTime, &BuildingEntity->BuildingTime, { 1279,431,61,5 }, { 1279,399,61,5 }, { 1278,382,64,8 }, false, false, false, App->entity->Panel, nullptr);
		break;
	case BuildingType::Cuartel:
		if (!BuildingQueue.empty()) {
			switch (BuildingEntity->Atype) {
			case AiType::RedShip:
				BuildHUD = App->gui->AddHUDBar(9, 341, BuildingEntity->TotalBuildingTime, &BuildingEntity->BuildingTime, { 1279,431,61,5 }, { 1279,399,61,5 }, { 1278,382,64,8 }, false, false, false, App->entity->Panel, nullptr);
				break;
			case AiType::BlueShip:
				BuildHUD = App->gui->AddHUDBar(9, 416, BuildingEntity->TotalBuildingTime, &BuildingEntity->BuildingTime, { 1279,431,61,5 }, { 1279,399,61,5 }, { 1278,382,64,8 }, false, false, false, App->entity->Panel, nullptr);
				break;
			case AiType::GreenShip:
				BuildHUD = App->gui->AddHUDBar(9, 491, BuildingEntity->TotalBuildingTime, &BuildingEntity->BuildingTime, { 1279,431,61,5 }, { 1279,399,61,5 }, { 1278,382,64,8 }, false, false, false, App->entity->Panel, nullptr);
				break;
			}
		}
	}
}