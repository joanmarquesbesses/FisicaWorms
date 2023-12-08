#pragma once
#include "Module.h"
#include "Animation.h"
#include "p2DynArray.h"
#include "Globals.h"



#define BOUNCER_TIME 200

enum lightTypes
{
	tiny,
	medium,
	big
};

class ModuleSceneIntro;

struct Light
{
	Light() : texture(NULL), on(false), fx(0)
	{}

	Light(ModuleSceneIntro* physics, int x, int y, lightTypes type);

	lightTypes type;
	SDL_Texture* texture;
	bool on;
	uint fx;
	int x, y;
};

class ModuleSceneIntro : public Module
{
public:
	ModuleSceneIntro(Application* app, bool start_enabled = true);
	~ModuleSceneIntro();

	bool Start();
	update_status Update();
	bool CleanUp();
	void resetball();

public:

	SDL_Texture* graphics;

	SDL_Texture* tex_light_tiny;
	SDL_Texture* tex_light_medium;
	SDL_Texture* tex_light_big;
	
	uint fx_light_tiny;
	uint fx_light_medium;
	uint fx_light_big;

	p2DynArray<Light> lights;

	uint player_lose_fx;
	SDL_Texture * textureBackground;
	SDL_Texture * textureC1;
	SDL_Texture * textureC2;

	bool c1flip = true;
	bool c2flip = false;

	SDL_Rect Canon;
	int C1Lives = 3;
	Animation* C1currentAnimation = nullptr;
	Animation C1idleAnimation;
	Animation C1walkAnimation;
	Animation C1attackAnimation;

	SDL_Rect Canon2;
	int C2Lives = 3;
	Animation* C2currentAnimation = nullptr;
	Animation C2idleAnimation;
	Animation C2walkAnimation;
	Animation C2attackAnimation;

	int debugMode = 0;

	Ball* bola;

	int actualShooter = 0;

	bool hit = false;

	float canonX;

	SDL_Texture* win1;
	SDL_Texture* win2;
};
