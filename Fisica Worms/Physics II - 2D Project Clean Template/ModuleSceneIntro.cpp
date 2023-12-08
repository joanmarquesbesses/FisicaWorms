#include "Globals.h"
#include "Application.h"
#include "ModuleSceneIntro.h"


ModuleSceneIntro::ModuleSceneIntro(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	graphics = NULL;
}

ModuleSceneIntro::~ModuleSceneIntro()
{

}

// Load assets
bool ModuleSceneIntro::Start()
{
	LOG("Loading Intro assets");
	bool ret = true;
	texture = App->textures->Load("../Assets/des_bg.png");
	App->renderer->camera.x = App->renderer->camera.y = 0;

	Canon.x = 200;
	Canon.y = 850;
	Canon.w = 20;
	Canon.h = 20;

	Canon2.x = 1300;
	Canon2.y = 850;
	Canon2.w = 20;
	Canon2.h = 20;

	fPoint initial_pos;
	initial_pos.x = 30.0f;
	initial_pos.y = 680.0f;

	bola = new Ball(initial_pos, 10, 10, 40, 70);

	App->physics->pObjects.push_back(bola);
	App->physics->setUpVelocity();
	bola->active = false;

	SDL_Rect ground1{ 0,950,600,100 };
	App->physics->pObjects.push_back(new Terrain(ground1, EntityType::GROUND));

	SDL_Rect ground2{ 1150,950,800,100 };
	App->physics->pObjects.push_back(new Terrain(ground2, EntityType::GROUND));

	SDL_Rect ground3{ 0,250,750,100 };
	App->physics->pObjects.push_back(new Terrain(ground3, EntityType::GROUND));

	SDL_Rect cascada{ 800,500,200,600 };
	App->physics->pObjects.push_back(new Terrain(cascada, EntityType::WATER));

	SDL_Rect charco{ 600,950,550,100 };
	App->physics->pObjects.push_back(new Terrain(charco, EntityType::WATER));

	SDL_Rect wall{ 0,270,100,800 };
	App->physics->pObjects.push_back(new Terrain(wall, EntityType::GROUND));

	return ret;
}

// Load assets
bool ModuleSceneIntro::CleanUp()
{
	LOG("Unloading Intro scene");

	return true;
}

void ModuleSceneIntro::resetball()
{
	hit = false;

	if (actualShooter == 0) {
		actualShooter = 1;
	}
	else {
		actualShooter = 0;
	}
	bola->active = false;
	bola->inrest = true;
	App->physics->reset(bola);

	for (size_t i = 1; i < App->physics->pObjects.size(); i++)
		App->physics->pObjects.at(i)->bounceCoef = App->physics->pObjects.at(i)->initial_bounceCoef;
}

// Update: draw background
update_status ModuleSceneIntro::Update()
{
	Uint8 r = 255, g = 0, b = 0;
	App->renderer->Blit(texture, 0, 0, NULL);
	int R = 0;
	int D = 0;

	if (bola->inrest) {
		float X;
		float Y;
		if ((App->input->GetMouseX() - bola->position.x) > 0) {
			X = (App->input->GetMouseX() - bola->position.x);
			R = 0;
		}
		else {
			X = (bola->position.x - App->input->GetMouseX());
			R = 1;
		}

		if ((bola->position.y - App->input->GetMouseY()) > 0) {
			Y = (bola->position.y - App->input->GetMouseY());
			D = 0;
		}
		else {
			Y = (App->input->GetMouseY() - bola->position.y);
			D = 1;
		}

		if (R == 0 && D == 0) {
			bola->angle = atan((Y) / (X)) * 180 / M_PI;
		}
		else if (R == 1 && D == 0) {
			bola->angle = 180 - atan((Y) / (X)) * 180 / M_PI;
		}
		else if (R == 1 && D == 1) {
			bola->angle = 180 + atan((Y) / (X)) * 180 / M_PI;
		}
		else if (R == 0 && D == 1) {
			bola->angle = 360 - atan((Y) / (X)) * 180 / M_PI;
		}
		bola->velocity = PIXEL_TO_METERS(sqrt(pow(bola->position.y - App->input->GetMouseY(), 2) + pow(App->input->GetMouseX() - bola->position.x, 2)));
	}

	App->renderer->DrawLine(bola->position.x, bola->position.y, App->input->GetMouseX(), App->input->GetMouseY(), r, g, b);

	r = 0; g = 33; b = 129;
	App->renderer->DrawQuad(Canon, r, g, b);
	r = 255; g = 0; b = 129;
	App->renderer->DrawQuad(Canon2, r, g, b);

	for (size_t i = 0; i < App->physics->pObjects.size(); i++)
	{
		App->physics->pObjects.at(i)->Draw();
	}

	canonX = 10 * (App->dt / 100);

	if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) 
	{
		if (actualShooter == 0) {
			if(Canon.x > 20)
			Canon.x -= canonX;
		}
		else {
			if (Canon2.x > 1000)
			Canon2.x -= canonX;
		}
	}
	if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) 
	{
		if (actualShooter == 0) {
			if (Canon.x < 150)
			Canon.x += canonX + 1;
		}
		else {
			if (Canon2.x < 1150)
			Canon2.x += canonX + 1;
		}
	}

	if (bola->inrest) 
	{
		if (actualShooter == 0) {
			bola->position.x = Canon.x + 10;
			bola->position.y = Canon.y + 10;
		}
		else {
			bola->position.x = Canon2.x + 10;
			bola->position.y = Canon2.y + 10;
		}
	}

	if (App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN && bola->inrest)
	{
		bola->active = true;
		bola->inrest = false;
		bola->setUpVelocity();
	}

	if (App->input->GetKey(SDL_SCANCODE_R) == KEY_DOWN)
	{
		resetball();
	}

	if (actualShooter == 0) {
		if (SDL_HasIntersection(&bola->objectRect, &Canon2) && !hit) {
			hit = true;
		}
	}
	else {
		if (SDL_HasIntersection(&bola->objectRect, &Canon) && !hit) {
			hit = true;
		}
	}

	

	return UPDATE_CONTINUE;
}
