#pragma once
#include "PhysicEntity.h"
#include "Animation.h"

class Ball : public PhysicEntity
{
public:
	Ball(fPoint position, float mass, float velocity, float radius, float angle);
	bool Start();

	//poner en PhysicEntity con texturas
	void Draw();
	void Move();
	void Recenter();
	
	bool inrest;

	SDL_Texture* tbola2;

private:
	int r;
	int g;
	int b;
};

