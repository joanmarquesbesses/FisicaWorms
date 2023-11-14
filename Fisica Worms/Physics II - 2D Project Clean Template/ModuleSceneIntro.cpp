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
	Canon.y = 680;
	Canon.w = 20;
	Canon.h = 20;
	//porva

	//

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
	Uint8 r = 255;
	Uint8 g = 0;
	Uint8 b = 0;
	App->renderer->DrawLine(0, 700, 1024, 700, r, g, b);

	SDL_Rect water{ 1024,700,300,58 };
	
	App->renderer->DrawQuad(water, 0,0,255);
	r = 125; g = 33; b = 129;

	App->renderer->DrawQuad(Canon, r, g, b);
	App->physics->ball->Draw();
	App->physics->ballg->Draw();

	if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) 
	{
		canonX -= 10 * (App->dt / 100);
		Canon.x = canonX;
	}
	if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) 
	{
		canonX += 10 * (App->dt / 100);
		Canon.x = canonX;
	}

	if (!App->physics->launch) 
	{
		App->physics->ball->Recenter();
	}

	LOG("%d",Canon.x);
	return UPDATE_CONTINUE;
}
