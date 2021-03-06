#include "EntityManager.h"
#include "Ai.h"
#include "Application.h"
#include "PathFinding.h"
#include "Input.h"
#include "Map.h"
#include "Window.h"
#include "Gui.h"
#include "Audio.h"
#include "Fonts.h"
#include "Collisions.h"

Ai::Ai(AiType type, iPoint Position) : Entity(EntityType::TypeAi, { Position.x,Position.y,0,0 }), Atype(type), speed(0.0f),  DirectionAngle(270.0f), Damage(0), Range(0), UpgradedDamage(0), UpgradedRange(0), UpgradedSpeed(0.0f), IsMoving(false), OnDestination(true),Armed(false),Working(true),WorkingTime(0.0f),Building(true),BuildingTime(0.0f),TotalBuildingTime(0),collider(nullptr),RangeCollider(nullptr),shooting(false){
    switch (Atype) {
	case AiType::RedShip:
        MaxHealth = 430;
		health = (float)MaxHealth;
		Damage = 60;
		Range = 200;
        UpgradedDamage = 90;
        UpgradedRange = 200;
		speed = 4.0f;
        UpgradedSpeed = 5.0f;
		//IdleAnimaiton = App->entity->Animations.AttackShip;
        IdleAnimation.PushBack({ 19,23,66,66 });
        IdleAnimation.PushBack({ 116,21,66,66 });
        IdleAnimation.speed = 2.5f;
        ArmedIdleAnimation.PushBack({ 217,221,80,80 });
        ArmedIdleAnimation.PushBack({ 357,221,80,80 });
        ArmedIdleAnimation.speed = 2.5f;
		selectable = true;
		EntityRect.w = 66;
		EntityRect.h = 66;
        TilePos = App->map->WorldToMap(EntityRect.x, EntityRect.y);
        NextTile = TilePos;
        OnDestination = true;
        TotalBuildingTime = 10;
        BuildingTime = 10.0f;
		break;
    case AiType::BlueShip:
        MaxHealth = 370;
        health = (float)MaxHealth;
        Damage = 40;
        Range = 200;
        UpgradedDamage = 80;
        UpgradedRange = 300;
        speed = 6.0f;
        UpgradedSpeed = 7.0f;
        IdleAnimation.PushBack({ 236,28,61,61 });
        IdleAnimation.PushBack({ 371,20,61,61 });
        IdleAnimation.speed = 3.0f;
        ArmedIdleAnimation.PushBack({ 222,110,80,80 });
        ArmedIdleAnimation.PushBack({ 362,110,80,80 });
        ArmedIdleAnimation.speed = 3.0f;
        selectable = true;
        EntityRect.w = 61;
        EntityRect.h = 61;
        TilePos = App->map->WorldToMap(EntityRect.x, EntityRect.y);
        NextTile = TilePos;
        OnDestination = true;
        TotalBuildingTime = 10;
        BuildingTime = 10.0f;
        break;
	case AiType::Collector:
        MaxHealth = 300;
        health = (float)MaxHealth;
		Damage = 0;
		Range = 100;
        UpgradedDamage = 0;
        UpgradedRange = 100;
		speed = 3.0f;
        UpgradedSpeed = 4.0f;
        IdleAnimation.PushBack({ 24,147,52,52 });
        IdleAnimation.PushBack({ 126,147,52,52 });
        IdleAnimation.speed = 2.0f;
        ArmedIdleAnimation = IdleAnimation;
		selectable = true;
		EntityRect.w = 52;
		EntityRect.h = 52;
        TilePos = App->map->WorldToMap(EntityRect.x, EntityRect.y);
        NextTile = TilePos;
        OnDestination = true;
        TotalBuildingTime = 5;
        BuildingTime = 5.0f;
		break;
    case AiType::GreenShip:
        MaxHealth = 800;
        health = (float)MaxHealth;
        Damage = 120;
        Range = 200;
        UpgradedDamage = 170;
        UpgradedRange = 200;
        UpgradedSpeed = 3.0f;
        speed = 2.0f;
        IdleAnimation.PushBack({ 25,324,57,57 });
        IdleAnimation.PushBack({ 127,324,57,57 });
        IdleAnimation.speed = 1.5f;
        ArmedIdleAnimation.PushBack({ 218,332,80,80 });
        ArmedIdleAnimation.PushBack({ 357,332,80,80 });
        ArmedIdleAnimation.speed = 1.5f;
        selectable = true;
        EntityRect.w = 57;
        EntityRect.h = 57;
        TilePos = App->map->WorldToMap(EntityRect.x, EntityRect.y);
        NextTile = TilePos;
        OnDestination = true;
        TotalBuildingTime = 10;
        BuildingTime = 10.0f;
        break;
	}
    rechargetime = 1.0f;
    currentcharge = rechargetime;
}

Ai::~Ai() {

}

void Ai::Update(float dt) {
    if (Working) {
        selectable = false;
        WorkingTime -= dt;
        if (WorkingTime <= 0) {
            Working = false;
            selectable = true;
        }
    }

    if (Building) {
        selectable = false;
        BuildingTime -= dt;
        if (BuildingTime <= 0) {
            Building = false;
            selectable = true;
            collider = App->collisions->AddCollider(EntityRect, COLLIDER_ALLY, App->entity,(Entity*)this);
            if(Atype!=AiType::Collector)
                RangeCollider = App->collisions->AddCollider({ EntityRect.x + EntityRect.w / 2 - Range / 2, EntityRect.y + EntityRect.h / 2 - Range / 2, Range, Range }, COLLIDER_ALLY_RANGE, App->entity,(Entity*)this);
        }
    }
    
    if (!OnDestination)
        UpdateMovement();

    if (shooting) {
        currentcharge -= dt;
        if (currentcharge <= 0) {
            currentcharge = rechargetime;
            shooting = false;
        }
    }
    
    if (health <= 0) {
        todie = true;
        if(collider!=nullptr)
            collider->toDelete = true;
        if(RangeCollider!=nullptr)
            RangeCollider->toDelete = true;
    }
    /*if (health < 0)
        die*/
}

void Ai::UpdateLogic() {
    if (IsMoving && !shooting)
        DoMovement();
}

void Ai::Draw(float dt) {
    //TODO: quan la nau recorre a vegades les diagonals va tremolant al canviar d'angles molt rapid
    if (!Building) {
        if (!Armed)
            App->render->Blit(sprite, EntityRect.x, EntityRect.y, &IdleAnimation.GetCurrentFrame(dt), true, App->render->renderer, (float)App->win->GetScale(), 1.0f, DirectionAngle);
        else
            App->render->Blit(sprite, EntityRect.x, EntityRect.y, &ArmedIdleAnimation.GetCurrentFrame(dt), true, App->render->renderer, (float)App->win->GetScale(), 1.0f, DirectionAngle);
        
        if (selected) {
            App->render->DrawQuad(EntityRect, 0, 255, 0, 255, false);
        }
        for (int i = 0; i != path.size(); ++i) {
            App->render->DrawQuad({ path[i].x * 32,path[i].y * 32,32,32 }, 255, 0, 0, 127, true);
        }
    }
}

void Ai::DoMovement() {
    //state = movestate::movenon
    bool MovementPerformed= false;
    if (NextTile.x < TilePos.x)
    {
        DirectionAngle = 270.0f;
        MovementPerformed = true;
        EntityRect.x -= (int)speed;
        if ((EntityRect.x - NextTile.x * App->map->data.tileWidth) <= 0)
        {
            TilePos.x--;
        }
    }
    else if (NextTile.x > TilePos.x)
    {
        DirectionAngle = 90.0f;
        MovementPerformed = true;
        EntityRect.x += (int)speed;
        if ((EntityRect.x - NextTile.x * App->map->data.tileWidth) >= 0)
        {
            TilePos.x++;
        }
    }
    if (NextTile.y < TilePos.y)
    {
        if (MovementPerformed && DirectionAngle == 90.0f) {
            DirectionAngle = 45.0f;
        }
        else if (MovementPerformed && DirectionAngle == 270.0f) {
            DirectionAngle = 315.0f;
        }
        else
            DirectionAngle = 0.0f;
        MovementPerformed = true;
        EntityRect.y -= (int)speed;
        if ((EntityRect.y - NextTile.y * App->map->data.tileHeight) <= 0)
        {
            TilePos.y--;
        }
    }
    else if (NextTile.y > TilePos.y)
    {
        if (MovementPerformed && DirectionAngle == 90.0f) {
            DirectionAngle = 135.0f;
        }
        else if (MovementPerformed && DirectionAngle == 270.0f) {
            DirectionAngle = 225.0f;
        }
        else
            DirectionAngle = 180.0f;
        MovementPerformed = true;
        EntityRect.y += (int)speed;
        if ((EntityRect.y - NextTile.y * App->map->data.tileHeight) >= 0)
        {
            TilePos.y++;
        }
    }

    if (!MovementPerformed) {
        IsMoving = false;
    }
    else {
        if(collider!=nullptr)
            collider->SetPos(EntityRect.x, EntityRect.y);
        if(RangeCollider!=nullptr)
            RangeCollider->SetPos(EntityRect.x + EntityRect.w / 2 - Range / 2, EntityRect.y + EntityRect.h / 2 - Range / 2);
    }
    
}

void Ai::Move(int x, int y) {
    NextTile.x += x;
    NextTile.y += y;
    IsMoving = true;
    //_speed = speed;
}

void Ai::UpdateMovement()
{
    if (!IsMoving) {

        if (!path.empty())
        {
            //TODO: Update units walkabilitiy position in the pathfinder
            int wantedXTile = path[0].x;
            int wantedYTile = path[0].y;

            if (TilePos.x < wantedXTile)
            {
                Move(1, 0);
            }
            else if (TilePos.x > wantedXTile)
            {
                Move(-1, 0);
            }
            if (TilePos.y < wantedYTile)
            {
                Move(0, 1);
            }
            else if (TilePos.y > wantedYTile)
            {
                Move(0, -1);
            }

            path.erase(path.begin());
            if (path.empty()) {
                path.shrink_to_fit();
                /*if (App->pathfinding->CreatePath(_goalX, _goalY) !=- 1)
                    path = App->pathfinding->GetLastPath();*/
                //TODO: Arreglar aquest if i el que calgui de aquesta classe per el correcte funcionament del group movement
            }
        }
        else
        {
            OnDestination = true;
        }
    }

    /*if (IsMoving) {

        
        App->audio->PlayFx(App->audio->LoadFx("Resources/audio/fx/spaceshipmov.wav"), -1);
        App->audio->FxVolume(1);
        
    }

    else {
        if (OnDestination == true) App->audio->UnloadFx();
    }*/
    
       
    
        
}

void Ai::UiFunctionallity() {
    switch (Atype) {
    case AiType::Collector:
        //App->gui->AddText(82,240,)
        App->gui->AddText(70, 250, "Barracks", App->font->smallFont, { 255,255,255,255 }, 32, false, false, false, App->entity->Panel);
        App->gui->AddText(70, 330, "Lab", App->font->smallFont, { 255,255,255,255 }, 32, false, false, false, App->entity->Panel);
        App->gui->AddText(70, 405, "Mine", App->font->smallFont, { 255,255,255,255 }, 32, false, false, false, App->entity->Panel);
        App->gui->AddText(70, 480, "Generator", App->font->smallFont, { 255,255,255,255 }, 32, false, false, false, App->entity->Panel);
        App->gui->AddEntityButton(20, 240, { 1344,251,39,39 }, { 1290,250,39,39 }, { 1398,251,39,39 },AviableEntities::cuartel,EntityType::TypeBuilding, true, false, false, App->entity->Panel, App->entity);
        App->gui->AddEntityButton(20, 315, { 1346,306,39,39 }, { 1290,305,39,39 }, { 1398,306,39,39 }, AviableEntities::ship_factory, EntityType::TypeBuilding, true, false, false, App->entity->Panel, App->entity);
        App->gui->AddEntityButton(20, 390, { 1645,308,39,39 }, { 1590,307,39,39 }, { 1698,308,39,39 }, AviableEntities::mine, EntityType::TypeBuilding, true, false, false, App->entity->Panel, App->entity);
        App->gui->AddEntityButton(20, 465, { 1645,250,39,39 }, { 1590,249,39,39 }, { 1698,250,39,39 }, AviableEntities::PowerGenerator, EntityType::TypeBuilding, true, false, false, App->entity->Panel, App->entity);
        break;
    }
}

void Ai::Upgrade() {
    Damage = UpgradedDamage;
    Range = UpgradedRange;
    speed = UpgradedSpeed;
    Armed = true;
    EntityRect.w = 80;
    EntityRect.h = 80;
    collider->toDelete = true;
    RangeCollider->toDelete = true;
    collider = App->collisions->AddCollider(EntityRect, COLLIDER_ALLY, App->entity,(Entity*)this);
    RangeCollider = App->collisions->AddCollider({ EntityRect.x + EntityRect.w / 2 - Range / 2, EntityRect.y + EntityRect.h / 2 - Range / 2, Range, Range }, COLLIDER_ALLY_RANGE, App->entity, (Entity*)this);
}