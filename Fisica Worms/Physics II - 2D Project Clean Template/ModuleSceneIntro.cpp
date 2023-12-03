#include "Globals.h"
#include "Application.h"
#include "ModuleSceneIntro.h"


ModuleSceneIntro::ModuleSceneIntro(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	graphics = NULL;
	texture = App->textures->Load("../Assets/canon.png");
}

ModuleSceneIntro::~ModuleSceneIntro()
{

}

// Load assets
bool ModuleSceneIntro::Start()
{
	LOG("Loading Intro assets");
	bool ret = true;

	App->renderer->camera.x = App->renderer->camera.y = 0;

	Canon.x = 20;
	Canon.y = 670;
	Canon.w = 20;
	Canon.h = 20;

	fPoint initial_pos;
	initial_pos.x = 30.0f;
	initial_pos.y = 680.0f;

	bola = new Ball(initial_pos, 10, 10, 20, 70);

	App->physics->pObjects.push_back(bola);
	App->physics->setUpVelocity();
	bola->active = false;

	SDL_Rect ground;
	ground.x = 0;
	ground.y = 700;
	ground.w = 1024;
	ground.h = 58;
	App->physics->pObjects.push_back(new Ground(ground));

	return ret;
}

// Load assets
bool ModuleSceneIntro::CleanUp()
{
	LOG("Unloading Intro scene");

	return true;
}

// Update: draw background
update_status ModuleSceneIntro::Update()
{
	Uint8 r = 255, g = 0, b = 0;

	if (bola->inrest) {
		float X;
		float Y;
		if ((App->input->GetMouseX() - bola->position.x) > 0) {
			X = (App->input->GetMouseX() - bola->position.x);
		}
		else {
			X = (bola->position.x - App->input->GetMouseX());
		}

		if ((bola->position.y - App->input->GetMouseY()) > 0) {
			Y = (bola->position.y - App->input->GetMouseY());
		}
		else {
			Y = (App->input->GetMouseY() - bola->position.y);
		}

		bola->angle = atan((Y) / (X)) * 180 / M_PI;
		bola->velocity = PIXEL_TO_METERS(sqrt(pow(bola->position.y - App->input->GetMouseY(), 2) + pow(App->input->GetMouseX() - bola->position.x, 2)));
	}

	App->renderer->DrawLine(bola->position.x, bola->position.y, App->input->GetMouseX(), App->input->GetMouseY(), r, g, b);

	LOG("%f", PIXEL_TO_METERS(sqrt(pow(bola->position.y - App->input->GetMouseY(),2) + pow(App->input->GetMouseX() - bola->position.x,2))));

	SDL_Rect water{ 1024,700,300,58 };
	App->renderer->DrawQuad(water, 0,0,255);

	r = 125; g = 33; b = 129;
	App->renderer->DrawQuad(Canon, r, g, b);

	for (size_t i = 0; i < App->physics->pObjects.size(); i++)
	{
		App->physics->pObjects.at(i)->Draw();
	}

	canonX = 10 * (App->dt / 100);

	if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) 
	{
		Canon.x -= canonX;
	}
	if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) 
	{
		Canon.x += canonX + 1;
	}

	if (bola->inrest) 
	{
		bola->position.x = Canon.x + 10;
		bola->position.y = Canon.y + 10;
	}

	if (App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN && bola->inrest)
	{
		bola->active = true;
		bola->inrest = false;
		bola->setUpVelocity();
	}

	if (App->input->GetKey(SDL_SCANCODE_R) == KEY_DOWN)
	{
		bola->active = false;
		bola->inrest = true;
		App->physics->reset(bola);

		for (size_t i = 1; i < App->physics->pObjects.size(); i++)
			App->physics->pObjects.at(i)->bounceCoef = App->physics->pObjects.at(i)->initial_bounceCoef;
		
	}

	/*if (!App->physics->launch) 
	{
		App->physics->ball->Recenter();
	}*/

	return UPDATE_CONTINUE;
}
