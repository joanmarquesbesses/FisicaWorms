#include "Terrain.h"

Terrain::Terrain(SDL_Rect area, EntityType etype) : PhysicEntity(position, mass, velocity)
{
	this->position.y = area.y;
	this->width = area.w;
	this->height = area.h;
	this->quad = area;

	this->bounceCoef = -0.25;
	this->initial_bounceCoef = -0.25;

	this->active = false;
	this->etype = etype;

	if (this->etype == EntityType::GROUND)
	{
		r = 0; g = 255; b = 0;
	}
	else if (this->etype == EntityType::WATER)
	{
		r = 0;	g = 0;	b = 255;
	}

	texture; //= App->textures->Load("../Assets/bala.png");
}

void Terrain::Draw()
{
	App->renderer->DrawQuad(quad, r, g, b);
}
