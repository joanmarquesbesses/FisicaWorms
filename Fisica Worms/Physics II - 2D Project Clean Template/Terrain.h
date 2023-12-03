#pragma once
#include "PhysicEntity.h"

#include "Application.h"
#include "ModuleRender.h"
#include "ModuleTextures.h"
#include "ModuleSceneIntro.h"

class Terrain : public PhysicEntity
{
public:
	Terrain(SDL_Rect area, EntityType etype);
	//poner en PhysicEntity con texturas
	void Draw();

	int width;
	int height;
	SDL_Rect quad;
private:
	int r;
	int g;
	int b;
};

