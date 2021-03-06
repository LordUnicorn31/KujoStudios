#pragma once
#include "Module.h"
#include "EASTL/list.h"
#include "EASTL/array.h"
#include "p2Point.h"
#include "Render.h"
#include "Animation.h"

#define MAXSELECTEDUNITS 33
#define MAXRESOURCES 9999

class Entity;
struct SDL_Texture;
enum class EntityType : unsigned char;
enum class AiType : unsigned char;
class UiElement;
class Ai;

enum InputActions {
	ActionNone,
	ActionMoving,
	ActionSelection,
	ActionConstruction,
	ActionTraining,
	ActionUpgrading
};

enum class AviableEntities: unsigned char {
	none,
	base,
	mine,
	cuartel,
	ship_factory,
	collector,
	greenship,
	blueship,
	redship,
	PowerGenerator,
	upgradedredship,
	upgradedblueship,
	upgradedgreenship,
	gold,
	ore,

	numentities
};

/*struct EntitiesAnimations {
	Animation BuildMine;
	Animation MineIdle;
	Animation Base1Idle;
	Animation Base2Idle;
	Animation Base3Idle;
	Animation Base4Idle;
	Animation Base5Idle;
	Animation Upgrade1;
	Animation Upgrade2;
	Animation Upgrade3;
	Animation Upgrade4;
	Animation BuildCuartel;
	Animation CuartelIdle;
	Animation BuildLab;
	Animation LabIdle;
	Animation FarmerShip;
	Animation AttackShip;
	Animation AttackShip2;
	Animation SpecialShip;
	Animation ArmedAttackShip;
	Animation ArmedAttackShip2;
	Animation ArmedSpecialShip;
	Animation PowerGeneratorIdle;
	Animation BuildPowerGenerator;
};*/

class EntityManager :public Module {
public:

	EntityManager();
	~EntityManager();

	void Init();
	bool Start();
	bool Update(float dt);
	void UpdateAll(float dt,bool DoLogic);
	bool PostUpdate();
	bool CleanUp();
	void ui_callback(UiElement* element);
	void OnCollision(Collider* c1, Collider* c2);

	Entity* CreateEntity(AviableEntities type, iPoint position);
	void DestroyEntity(Entity* entity);

	const eastl::list<Entity*>& GetEntities()const;
	const Entity* GetBase() const;

	//EntitiesAnimations Animations;
	UiElement* Panel;
	Ai* CurrentBuildingEntity;
	eastl::array<float,3> Resources;

private:
	bool newgame;
	void GenerateResources(int n_gold, int n_ore);
	bool EnaughResources(AviableEntities toBuild);
	void PayCost(AviableEntities toPay);
	bool Load(pugi::xml_node&);
	bool Save(pugi::xml_node&);
	AiType AviableToAi(AviableEntities aviable);
	eastl::list <Entity*>entities;
	eastl::list<Entity*>SelectedEntities;
	bool DoLogic;
	float AccumulatedTime;
	float UpdateMsCycle;
	SDL_Texture* MineSprite;
	SDL_Texture* CuartelLab;
	SDL_Texture* BaseSprite;
	SDL_Texture* ShipsSprite;
	SDL_Texture* Copper;
	SDL_Texture* Titanium;
	SDL_Texture* PowerGeneratorSprite;
	void HandleInput();
	AviableEntities ToCreate;
	UiElement* BuildButton;
	UiElement* NotEnaughResourcesText;
	InputActions CurrentAction;
	UiElement* CopperString;
	UiElement* TitaniumString;
	UiElement* CopperIcon;
	UiElement* TitaniumIcon;
	UiElement* Coppernum;
	UiElement* Titaniumnum;
	eastl::array<eastl::array<float, 2>, (size_t)AviableEntities::numentities>BuildCost;
	const eastl::array<float, 2> GetCost(AviableEntities Entity) const;
	Entity* FirstBuilding;
	int buildFx;
	int upgradeFx;
	int shipFx;
	//max_resouces[]
	//current_resouces[]
};