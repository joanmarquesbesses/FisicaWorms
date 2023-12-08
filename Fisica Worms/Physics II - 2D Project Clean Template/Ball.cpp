#include "Ball.h"

#include "Application.h"
#include "ModuleRender.h"
#include "ModuleTextures.h"
#include "ModuleSceneIntro.h"

Ball::Ball(fPoint position, float mass, float velocity, float radius, float angle) : PhysicEntity(position, mass, velocity)
{
	this->angle = angle;
	this->surface = 4 * 3.14f * pow(PIXEL_TO_METERS(radius), 2);
	this->volumne = 4 / 3 * 3.14f * pow(PIXEL_TO_METERS(radius), 3);
	this->radius = PIXEL_TO_METERS(radius);

	etype = EntityType::BALL;
	inrest = true;

	r = 255;
	g = 0;
	b = 0;

	texture = App->textures->Load("../Assets/asset hueso.png");
	tbola2 = App->textures->Load("../Assets/asset bomba.png");
}

void Ball::Draw()
{
	if (App->scene_intro->actualShooter == 0)
	{
		App->renderer->Blit(texture, position.x - 12, position.y - 11, NULL, 0, 0, 0, 0);
	}
	else
	{
		App->renderer->Blit(tbola2, position.x - 12, position.y - 11, NULL, 0, 0, 0, 0);
	}

	objectRect.x = position.x - 12; 
	objectRect.y = position.y - 12;
	objectRect.w = 24;
	objectRect.h = 24;
	App->renderer->DrawQuad(objectRect, 0, 255, 255, App->scene_intro->debugMode);
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

