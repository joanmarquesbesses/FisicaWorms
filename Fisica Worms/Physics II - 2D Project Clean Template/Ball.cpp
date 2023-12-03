#include "Ball.h"

#include "Application.h"
#include "ModuleRender.h"
#include "ModuleTextures.h"
#include "ModuleSceneIntro.h"

Ball::Ball(fPoint position, float mass, float velocity, int radius, float angle) : PhysicEntity(position, mass, velocity)
{
	this->angle = angle;
	this->surface = radius;
	etype = EntityType::BALL;
	inrest = true;
	r = 255;
	g = 0;
	b = 0;
	texture = App->textures->Load("../Assets/bala.png");
}

void Ball::Draw()
{
	//App->renderer->Blit(texture, position.x - 12, position.y - 11, NULL, 0, 0, 0, 0);
	SDL_Rect water{ position.x - 12,position.y - 11,24,24 };
	App->renderer->DrawQuad(water, 0, 0, 255);
}

void Ball::Move()
{
	//position.x++;
}

void Ball::Recenter()
{
	this->position.x = App->scene_intro->Canon.x + (App->scene_intro->Canon.w / 2);
	this->position.y = App->scene_intro->Canon.y + (App->scene_intro->Canon.h / 2);
}

