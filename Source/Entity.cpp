#include "Entity.h"

Entity::Entity(EntityType type, SDL_Rect entityrect): etype(type), sprite(nullptr), EntityRect(entityrect),selectable (false),todie(false) {}

Entity::~Entity() {

}

void Entity::Update(float dt) {

}

void Entity::Draw(float dt) {

}

void Entity::UpdateLogic() {

}

void Entity::UiFunctionallity(){

}
/*void Entity::Die() {

}*/