#include "Globals.h"
#include "Application.h"
#include "ModulePhysics.h"
#include "math.h"

ModulePhysics::ModulePhysics(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	debug = true;
}

// Destructor
ModulePhysics::~ModulePhysics()
{
}

bool ModulePhysics::Start()
{
	LOG("Creating Physics 2D environment");
	fPoint initial_pos;
	initial_pos.x = 30.0f;
	initial_pos.y = 30.0f;

	fPoint acc;
	acc.x = 5.0f;
	acc.y = 5.0f;

	PhysicEntity* bola = new Ball(initial_pos, 5, 10, acc, 5, 45);
	App->physics->pObjects->push_back(bola);
	bola = nullptr;

	//// posar velocity x and y a la ball.h
	//ball->velocityVec.x = ball->velocity * cos(ball->angle * 3.1415 / 180);
	//ball->velocityVec.y = ball->velocity * sin(ball->angle * 3.1415 / 180);

	//ballg->velocityVec.x = ballg->velocity * cos(ballg->angle * 3.1415 / 180);
	//ballg->velocityVec.y = ballg->velocity * sin(ballg->angle * 3.1415 / 180);

	return true;
}

// 
update_status ModulePhysics::PreUpdate()
{
	time = (App->dt / 1000);
	Calculate_Gravity();
	Integrator_Euler();
	/*ballg->velocityVec.y -= App->gravity * time;
	ballg->position.y -= ballg->velocityVec.y;
	// Crear funcions per a cada modo de launch
	switch (collision) {
		case No:
			if (ball->position.y < 700) {
				if (App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN) {
					launch = true;
					ball->velocityVec.x = ball->velocity * cos(ball->angle * 3.1415 / 180);
					ball->velocityVec.y = ball->velocity * sin(ball->angle * 3.1415 / 180);
				}
				if (launch) {
					if (mode1) {
						ball->position.x += ball->velocityVec.x;
						ball->position.y -= ball->velocityVec.y;
						ball->velocityVec.y -= App->gravity * time;
					}
					else if (mode2) {
						ball->velocityVec.y -= App->gravity * time;
						ball->position.x += ball->velocityVec.x;
						ball->position.y -= ball->velocityVec.y;
					}
					else if (mode3) {
						ball->position.x += ball->velocityVec.x;
						ball->position.y -= ball->velocityVec.y;
						ball->position.y -= (pow(time, 2) * 10.0f * 0.5f);
						ball->velocityVec.y -= App->gravity * (time);
					}
				}
				else {
					launch = false;
				}
			}
			break;
		case Teleport:

			if (ball->position.y < 700) {
				if (App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN) {
					launch = true;
					ball->velocityVec.x = ball->velocity * cos(ball->angle * 3.1415 / 180);
					ball->velocityVec.y = ball->velocity * sin(ball->angle * 3.1415 / 180);
				}
				if (launch) {
					if (mode1) {
						ball->position.x += ball->velocityVec.x;
						ball->position.y -= ball->velocityVec.y;
						ball->velocityVec.y -= App->gravity * time;
					}
					else if (mode2) {
						ball->velocityVec.y -= App->gravity * time;
						ball->position.x += ball->velocityVec.x;
						ball->position.y -= ball->velocityVec.y;
					}
					else if (mode3) {
						ball->position.x += ball->velocityVec.x;
						ball->position.y -= ball->velocityVec.y;
						ball->position.y += (pow(time, 2) * 10.0f * 0.5f);
						ball->velocityVec.y -= App->gravity * (time);
					}
				}
			}
			else {
				launch = false;
				ball->position.y -= (ball->position.y - 700) + 10;
			}

			break;
		case Iteratively:

			if (App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN) {
				launch = true;
				ball->velocityVec.x = ball->velocity * cos(ball->angle * 3.1415 / 180);
				ball->velocityVec.y = ball->velocity * sin(ball->angle * 3.1415 / 180);
			}

			if (launch) {
				if (mode1) {
					ball->position.x += ball->velocityVec.x;
					ball->position.y -= ball->velocityVec.y;
					ball->velocityVec.y -= App->gravity * time;
				}
				else if (mode2) {
					ball->velocityVec.y -= App->gravity * time;
					ball->position.x += ball->velocityVec.x;
					ball->position.y -= ball->velocityVec.y;
				}
				else if (mode3) {
					ball->position.x += ball->velocityVec.x;
					ball->position.y -= ball->velocityVec.y;
					ball->position.y += (pow(time, 2) * 10.0f * 0.5f);
					ball->velocityVec.y -= App->gravity * (time);
				}
			}

			if (ball->position.y > 710) {
				launch = false;
				backtrack = true;
			}

			if (backtrack) {
				if (mode1) {
					ball->position.x -= ball->velocityVec.x;
					ball->position.y += ball->velocityVec.y;
					ball->velocityVec.y += App->gravity * time;
				}
				else if (mode2) {
					ball->velocityVec.y += App->gravity * time;
					ball->position.x -= ball->velocityVec.x;
					ball->position.y += ball->velocityVec.y;
				}
				else if (mode3) {
					ball->position.x -= ball->velocityVec.x;
					ball->position.y += ball->velocityVec.y;
					ball->position.y -= (pow(time, 2) * 10.0f * 0.5f);
					ball->velocityVec.y += App->gravity * (time);
				}
				if (ball->position.y < 695) {
					backtrack = false;
				}
			}
			break;
		case Raycast:
			int previousx = 0, previousy = 0;
			if (App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN) {
				launch = true;
				ball->velocityVec.x = ball->velocity * cos(ball->angle * 3.1415 / 180);
				ball->velocityVec.y = ball->velocity * sin(ball->angle * 3.1415 / 180);
			}

			if (launch) {
				if (mode1) {
					ball->position.x += ball->velocityVec.x;
					ball->position.y -= ball->velocityVec.y;
					ball->velocityVec.y -= App->gravity * time;
				}
				else if (mode2) {
					ball->velocityVec.y -= App->gravity * time;
					ball->position.x += ball->velocityVec.x;
					ball->position.y -= ball->velocityVec.y;
				}
				else if (mode3) {
					ball->position.x += ball->velocityVec.x;
					ball->position.y -= ball->velocityVec.y;
					ball->position.y += (pow(time, 2) * 10.0f * 0.5f);
					ball->velocityVec.y -= App->gravity * (time);
				}
				previousx = ball->position.x;
				previousy = ball->position.y;
			}

			if (ball->position.y > 690) {
				launch = false;

			}
			break;

	}*/
	return UPDATE_CONTINUE;
}

// 
update_status ModulePhysics::PostUpdate()
{
	if(App->input->GetKey(SDL_SCANCODE_F1) == KEY_DOWN)
		debug = !debug;

	if(!debug)
		return UPDATE_CONTINUE;

	if (App->input->GetKey(SDL_SCANCODE_F2) == KEY_DOWN) {
		if (mode1) {
			mode2 = true;
			mode1 = false;
		} else if (mode2) {
			mode3 = true;
			mode2 = false;
		} else if (mode3) {
			mode1 = true;
			mode3 = false;
		}
	}

	if (App->input->GetKey(SDL_SCANCODE_F3) == KEY_DOWN) {
		switch (collision) {
		case No:
			collision = Teleport;
			break;
		case Teleport:
			collision = Iteratively;
			break;
		case Iteratively:
			collision = Raycast;
			break;
		case Raycast:
			collision = No;
			break;
		default:
			break;
		}
	}

	if (App->input->GetKey(SDL_SCANCODE_R) == KEY_DOWN) {
		/*ball->position.x = App->scene_intro->Canon.x + (App->scene_intro->Canon.w / 2);
		ball->position.y = App->scene_intro->Canon.y + (App->scene_intro->Canon.h / 2);
		ball->velocityVec.x = ball->velocity * cos(ball->angle * 3.1415 / 180);
		ball->velocityVec.y = ball->velocity * sin(ball->angle * 3.1415 / 180);
		ballg->position.x = 450;
		ballg->position.y = 30;
		ballg->velocityVec.x = 0;
		ballg->velocityVec.y = 0;
		launch = false;*/
	}
	/*if (launch == false) {
		if (App->input->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT) {
			ball->angle += 0.1f;
		}

		if (App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT) {
			ball->angle -= 0.1f;
		}

		if (App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT) {
			ball->velocity += 0.1f;
		}

		if (App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT) {
			if (ball->velocity > 0) {
				ball->velocity -= 0.1f;
			}
		}
	}*/

	static char title[256];
	/*sprintf_s(title, 256, "Velocity: %.2f Angle: %.2f VelocityX: %.2f VelocityY: %.2f, dt: %.2f", ball->velocity, ball->angle, ball->velocityVec.x, ball->velocityVec.y, App->dt);*/

	App->window->SetTitle(title);

	return UPDATE_CONTINUE;
}


// Called before quitting
bool ModulePhysics::CleanUp()
{
	LOG("Destroying physics world");

	return true;
}
//
//void ModulePhysics::setBallPointer(PhysicEntity* ball)
//{
//	this->ball = ball;
//}

Force ModulePhysics::Calculate_Gravity()
{
	for (size_t i = 0; i < pObjects->size(); i++)
	{
		pObjects->at(i)->force.y = pObjects->at(i)->mass * App->gravity * -1;
	}
	return Force();
}

void ModulePhysics::Integrator_Euler()
{
	for (size_t i = 0; i < pObjects->size(); i++)
	{
		pObjects->at(i)->acceleration.y = pObjects->at(i)->force.y / pObjects->at(i)->mass;
		pObjects->at(i)->velocityVec.y = pObjects->at(i)->acceleration.y * (App->dt / 1000);
		pObjects->at(i)->position.y = pObjects->at(i)->velocityVec.y * (App->dt / 1000);
	}
}
