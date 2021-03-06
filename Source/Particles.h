#ifndef __PARTICLES_H__
#define __PARTICLES_H__

#include "Module.h"
#include "p2Point.h"
#include "Animation.h"
#include "Collisions.h"

#define MAX_ACTIVE_PARTICLES 300

struct SDL_Texture;
struct Collider;
enum COLLIDER_TYPE;

enum class ParticleType 
{
	NONE,
	SHOT,
	SMOKE

};

struct Particle
{
	Collider* collider = nullptr;
	Animation anim;					//Particle animation
	SDL_Rect rect;					//Particle rectangle
	SDL_Texture* tex;				//Particle texture
	ParticleType type;				//Particle type
	float speed;					//Particle speed
	int size;						//Particle size
	float life;  					//Particle life
	double angle;					//Particle angle
	iPoint position;					//Particle position
	SDL_Color initialColor;			//Particle color inicial
	SDL_Color finalColor;			//Particle color final
	uint fx = 0;					//Particle fx
	bool fxPlayed = false;			//Particle fx active
	int damage;
	float delay;

	Particle();
	Particle(const Particle& p);
	~Particle();
	bool Update(float dt);
};

class Particles : public Module
{
public:
	Particles();
	~Particles();

	bool Start();
	bool Update(float dt);
	bool CleanUp();
	void OnCollision(Collider* c1, Collider* c2);

	void AddParticle(const Particle& particle, int x, int y, int size, float delay = 0, COLLIDER_TYPE colliderType = COLLIDER_TYPE::COLLIDER_NONE, ParticleType type = ParticleType::NONE, double angle = 0.0f, float life = 0, int damage = 0, float speed = 0);

	
	SDL_Texture* graphics = nullptr;
	Particle* active[MAX_ACTIVE_PARTICLES];
private:


public:

	Particle smoke;
	Particle shot;
};

#endif // __PARTICLES_H__