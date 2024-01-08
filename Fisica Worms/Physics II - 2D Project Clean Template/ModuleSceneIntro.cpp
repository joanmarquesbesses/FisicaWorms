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
	textureBackground = App->textures->Load("../Assets/des_bg.png");
	textureC1 = App->textures->Load("../Assets/Ewok animatioons.png");
	textureC2 = App->textures->Load("../Assets/animaciones_soldado.png");

	win1 = App->textures->Load("../Assets/win-lose-joan_ewok.png");
	win2 = App->textures->Load("../Assets/win-lose-joan_soldado.png");
	App->renderer->camera.x = App->renderer->camera.y = 0;

	Canon.x = 200;
	Canon.y = 700;
	Canon.w = 90;
	Canon.h = 190;

	Canon2.x = 1300;
	Canon2.y = 700;
	Canon2.w = 90;
	Canon2.h = 190;


	// Initialize Idle Animation CANON 1
	C1idleAnimation.PushBack({ 48,380,92,193 });
	C1idleAnimation.PushBack({ 186,376,92,196 });
	C1idleAnimation.speed = 0.1f;

	// Initialize Walk Animation CANON 1
	C1walkAnimation.PushBack({ 48,115,79,185 });
	C1walkAnimation.PushBack({ 217,118,86,182 });
	C1walkAnimation.PushBack({ 392,105,79,195 });
	C1walkAnimation.PushBack({ 562,114,79,186 });
	C1walkAnimation.PushBack({ 731,116,84,183 });
	C1walkAnimation.PushBack({ 904,104,80,194 });
	C1walkAnimation.speed = 0.1f;

	// Initialize Attack Animation CANON 1
	C1attackAnimation.PushBack({ 48,637,82,191 });
	C1attackAnimation.PushBack({ 145,616,90,212 });
	C1attackAnimation.PushBack({ 249,638,106,190 });
	C1attackAnimation.PushBack({ 370,617,124,210 });
	C1attackAnimation.speed = 0.1f;
	C1attackAnimation.loop = false;

	C1currentAnimation = &C1idleAnimation;

	// Initialize Idle Animation CANON 2
	C2idleAnimation.PushBack({ 54,383,67,188 });
	C2idleAnimation.PushBack({ 130,389,67,184 });
	C2idleAnimation.speed = 0.1f;

	// Initialize Walk Animation CANON 2
	C2walkAnimation.PushBack({ 45,123,98,179 });
	C2walkAnimation.PushBack({ 153,134,97,169 });
	C2walkAnimation.PushBack({ 259,122,50,182 });
	C2walkAnimation.PushBack({ 333,115,79,179 });
	C2walkAnimation.PushBack({ 422,117,97,179 });
	C2walkAnimation.PushBack({ 524,134,101,170 });
	C2walkAnimation.PushBack({ 642,122,47,482 });
	C2walkAnimation.PushBack({ 704,118,80,182 });
	C2walkAnimation.speed = 0.1f;

	// Initialize Attack Animation CANON 2
	C2attackAnimation.PushBack({ 54,639,68,188 });
	C2attackAnimation.PushBack({ 125,633,92,196 });
	C2attackAnimation.PushBack({ 217,638,69,190 });
	C2attackAnimation.PushBack({ 285,636,98,191 });
	C2attackAnimation.PushBack({ 383,640,67,188 });
	C2attackAnimation.speed = 0.1f;
	C2attackAnimation.loop = false;

	C2currentAnimation = &C2idleAnimation;

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

	if (actualShooter == 0) 
	{
		actualShooter = 1;
		C1attackAnimation.Reset();
	}
	else 
	{
		actualShooter = 0;
		C2attackAnimation.Reset();
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
	App->renderer->Blit(textureBackground, 0, 0, NULL);

	if(c1flip){
		App->renderer->Blit1(textureC1, Canon.x, Canon.y, &C1currentAnimation->GetCurrentFrame());
	}else{
		App->renderer->Blit(textureC1, Canon.x, Canon.y, &C1currentAnimation->GetCurrentFrame());
	}

	if (c2flip)
	{
		App->renderer->Blit1(textureC2, Canon2.x, Canon2.y, &C2currentAnimation->GetCurrentFrame());
	}
	else
	{
		App->renderer->Blit(textureC2, Canon2.x, Canon2.y, &C2currentAnimation->GetCurrentFrame());
	}

	if (bola->active == true) {
		if (bola->objectRect.x < 0 || bola->objectRect.x > SCREEN_WIDTH) {
			resetball();
		}
		if (bola->objectRect.y > SCREEN_HEIGHT) {
			resetball();
		}
	}

	if (actualShooter == 0) {
		if (SDL_HasIntersection(&bola->objectRect, &Canon2) && !hit) {
			hit = true;
			C2Lives--;
		}
	}
	else {
		if (SDL_HasIntersection(&bola->objectRect, &Canon) && !hit) {
			hit = true;
			C1Lives--;

		}
	}

	if (App->input->GetKey(SDL_SCANCODE_R) == KEY_DOWN)
	{
		resetball();
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
	if(debugMode)
	App->renderer->DrawQuad(Canon, r, g, b);
	r = 255; g = 0; b = 129;
	if (debugMode)
	App->renderer->DrawQuad(Canon2, r, g, b);

	for (size_t i = 0; i < App->physics->pObjects.size(); i++)
	{
		App->physics->pObjects.at(i)->Draw();
	}

	canonX = 10 * (App->dt / 100);

	if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) 
	{
		if (actualShooter == 0) {
			if(Canon.x > 100)
			Canon.x -= canonX;
			C1currentAnimation = &C1walkAnimation;
		}
		else {
			if (Canon2.x > 1000)
			Canon2.x -= canonX;
			C2currentAnimation = &C2walkAnimation;
		}
		
	}
	if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) 
	{
		if (actualShooter == 0) {
			if (Canon.x < 400)
			Canon.x += canonX + 1;
			C1currentAnimation = &C1walkAnimation;
		}
		else {
			if (Canon2.x < 1400)
			Canon2.x += canonX + 1;
			C2currentAnimation = &C2walkAnimation;
		}
	}

	

	if (App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN && bola->inrest)
	{
		bola->active = true;
		bola->inrest = false;
		bola->setUpVelocity();

		if (actualShooter == 0)
		{
			C1currentAnimation = &C1attackAnimation;
		}
		else 
		{
			C2currentAnimation = &C2attackAnimation;
		}
	}

	if (C2Lives <= 0)
	{
		//Player 1 Wins
		App->renderer->Blit(win1, 1920 / 2, 1035 / 2);
	}

	if (C1Lives <= 0)
	{
		//Player 2 Wins
		App->renderer->Blit(win2, 1920 / 2, 1035 / 2);
	}

	if (actualShooter == 0)
	{
		C2currentAnimation = &C2idleAnimation;
	}
	else
	{
		C1currentAnimation = &C1idleAnimation;
	}

	C1currentAnimation->Update();
	C2currentAnimation->Update();

	

	return UPDATE_CONTINUE;
}
