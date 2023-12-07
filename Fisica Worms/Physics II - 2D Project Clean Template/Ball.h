#pragma once
#include "PhysicEntity.h"

class Ball : public PhysicEntity
{
public:
	Ball(fPoint position, float mass, float velocity, float radius, float angle);
	//poner en PhysicEntity con texturas
	void Draw();
	void Move();
	void Recenter();
	
	bool inrest;
private:
	int r;
	int g;
	int b;
};

