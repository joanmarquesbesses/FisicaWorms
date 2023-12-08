#include "Globals.h"
#include "Application.h"
#include "ModulePhysics.h"
#include "math.h"

ModulePhysics::ModulePhysics(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	debug = true;
}

// Destructor
ModulePhysics::~ModulePhysics() {}

bool ModulePhysics::Start()
{
	LOG("Creating Physics 2D environment");

	return true;
}


update_status ModulePhysics::PreUpdate()
{
	time = (App->dt / 1000);
	Calculate_Gravity();
	
	for (size_t i = 0; i < pObjects.size(); i++)
	{
		if (SDL_HasIntersection(&pObjects.at(0)->objectRect, &pObjects.at(i)->objectRect) && pObjects.at(i)->etype == EntityType::WATER)
		{
			Calculate_Hydrodinamics();
		}
		else {
			Calculate_Aerodynamics();
		}
	}

if (App->input->GetKey(SDL_SCANCODE_F3) == KEY_DOWN) 
	{
		switch (collision)
		{
		case Collisions::NO:
			collision = Collisions::TELEPORT;
			break;

		case Collisions::TELEPORT:
			collision = Collisions::ITERATIVE;
			break;

		case Collisions::ITERATIVE:
			collision = Collisions::RAYCAST;
			break;

		case Collisions::RAYCAST:
			collision = Collisions::NO;
			break;

		default:
			break;
		}
	}

	if (App->input->GetKey(SDL_SCANCODE_F2) == KEY_DOWN)
	{
 		switch (integrator)
		{
		case Integrators::EULER:
			integrator = Integrators::SYMPLETIC;
			break;

		case Integrators::SYMPLETIC:
			integrator = Integrators::VERLET;
			break;

		case Integrators::VERLET:
			integrator = Integrators::EULER;
			break;

		default:
			break;
		}
	}

	if (App->input->GetKey(SDL_SCANCODE_F4) == KEY_DOWN)
	{
		enableLift = !enableLift;
	}

	if (App->input->GetKey(SDL_SCANCODE_F5) == KEY_DOWN)
	{
		enableWater = !enableWater;
	}

	switch (integrator)
	{
	case Integrators::EULER:
		Integrator_Euler();
		break;

	case Integrators::SYMPLETIC:
		Integrator_SympleticEuler();
		break;

	case Integrators::VERLET:
		Integrator_VelocityVerlet();
		break;

	default:
		break;
	}

	if (App->scene_intro->bola->active)
	{
		switch (collision)
		{
		case Collisions::NO:
			Collision_NoAdjustment();
			break;

		case Collisions::TELEPORT:
			Collision_Teleport();
			break;

		case Collisions::ITERATIVE:
			Collision_Iterative();
			break;

		case Collisions::RAYCAST:
			Collision_Raycast();
			break;

		default:
			break;
		}
	}

	return UPDATE_CONTINUE;
}


update_status ModulePhysics::PostUpdate()
{
	if(App->input->GetKey(SDL_SCANCODE_F1) == KEY_DOWN)
		App->scene_intro->debugMode = !App->scene_intro->debugMode;
	
	static char title[256];
	sprintf_s(title, 256, "Initial Vel: %.2f Angle: %.2f VelX: %.2f VelY: %.2f, airDen: %.2f, integrator: %d, collision: %d, debug: %d, dt: %.2f", 
		pObjects.at(0)->velocity, pObjects.at(0)->angle, pObjects.at(0)->velocityVec.x, pObjects.at(0)->velocityVec.y, airDensity,
		integrator, collision, App->scene_intro->debugMode ,App->dt);

	App->window->SetTitle(title);

	return UPDATE_CONTINUE;
}


// Called before quitting
bool ModulePhysics::CleanUp()
{
	LOG("Destroying physics world");

	return true;
}

void ModulePhysics::Calculate_Gravity()
{
	for (size_t i = 0; i < pObjects.size(); i++)
	{
		if (pObjects.at(i)->active == true) 
		{
			pObjects.at(i)->force.y = pObjects.at(i)->mass * App->gravity;
			pObjects.at(i)->force.x = 0;
		}
	}
}

void ModulePhysics::Calculate_Aerodynamics()
{
	for (size_t i = 0; i < pObjects.size(); i++)
	{
		if (pObjects.at(i)->active == true)
		{
			if (enableLift) 
			{		
				// lift
				float lift = (0.5 * (airDensity * pow(pObjects.at(i)->velocityVec.y, 2) * pObjects.at(i)->surface * 2));
				
				if (pObjects.at(i)->velocityVec.y > 0.0f) 
				{
					pObjects.at(i)->force.y -= lift;
				}
			}

			
			if (enableLift) 
			{
				// drag
				float dragy = (0.5 * (airDensity *	pow(pObjects.at(i)->velocityVec.y, 2) * pObjects.at(i)->surface * 0.2));
				float dragx = (0.5 * (airDensity * pow(pObjects.at(i)->velocityVec.x, 2) * pObjects.at(i)->surface * 0.2));
				if (pObjects.at(i)->velocityVec.y < 0.0f) 
				{
					pObjects.at(i)->force.y += dragy;
					if (pObjects.at(i)->velocityVec.x > 0.0f)
					{
						pObjects.at(i)->force.x -= dragx;
					}
					else {
						pObjects.at(i)->force.x += dragx;
					}
				}
			}
		}
	}
}

void ModulePhysics::Calculate_Hydrodinamics()
{
	for (size_t i = 0; i < pObjects.size(); i++)
	{
		if (pObjects.at(i)->active == true)
		{
			if (enableWater)
			{
				// drag
				float dragx = bCoef * pObjects.at(0)->velocityVec.x;
				float dragy = bCoef * pObjects.at(0)->velocityVec.y;

				if (pObjects.at(0)->velocityVec.y > 0.0f)
				{
					pObjects.at(i)->force.y += dragy;
				}
				else {
					pObjects.at(i)->force.y -= dragy;
				}

				pObjects.at(i)->force.x -= dragx;
			}

			if (enableWater)
			{
				// bouyancy
				float bouyancy = waterDensity * App->gravity * pObjects.at(0)->volumne;
				pObjects.at(i)->force.y -= bouyancy;
			}
		}
	}
}

void ModulePhysics::Integrator_Euler()
{
	for (size_t i = 0; i < pObjects.size(); i++)
	{
		if (pObjects.at(i)->active == true) 
		{
			// Y
			pObjects.at(i)->acceleration.y = (pObjects.at(i)->force.y / pObjects.at(i)->mass);
			pObjects.at(i)->position.y += METERS_TO_PIXELS(pObjects.at(i)->velocityVec.y) * time;
			pObjects.at(i)->velocityVec.y += pObjects.at(i)->acceleration.y * time;

			// X
			pObjects.at(i)->acceleration.x = (pObjects.at(i)->force.x / pObjects.at(i)->mass);
			pObjects.at(i)->position.x += METERS_TO_PIXELS(pObjects.at(i)->velocityVec.x) * time;
			pObjects.at(i)->velocityVec.x += pObjects.at(i)->acceleration.x * time;
		}
	}
}

void ModulePhysics::Integrator_SympleticEuler()
{
	for (size_t i = 0; i < pObjects.size(); i++)
	{
		if (pObjects.at(i)->active)
		{
			// Y
			pObjects.at(i)->acceleration.y = (pObjects.at(i)->force.y / pObjects.at(i)->mass);
			pObjects.at(i)->velocityVec.y += pObjects.at(i)->acceleration.y * time;
			pObjects.at(i)->position.y += METERS_TO_PIXELS(pObjects.at(i)->velocityVec.y) * time;

			// X
			pObjects.at(i)->acceleration.x = (pObjects.at(i)->force.x / pObjects.at(i)->mass);
			pObjects.at(i)->velocityVec.x += pObjects.at(i)->acceleration.x * time;
			pObjects.at(i)->position.x += METERS_TO_PIXELS(pObjects.at(i)->velocityVec.x) * time;
		}
	}
}

void ModulePhysics::Integrator_VelocityVerlet()
{
	for (size_t i = 0; i < pObjects.size(); i++)
	{
		if (pObjects.at(i)->active)
		{
			// Y
			pObjects.at(i)->acceleration.y = (pObjects.at(i)->force.y / pObjects.at(i)->mass);
			pObjects.at(i)->position.y += METERS_TO_PIXELS(pObjects.at(i)->velocityVec.y) * time + 0.5 * pObjects.at(i)->acceleration.y * pow(time, 2);
			pObjects.at(i)->velocityVec.y += pObjects.at(i)->acceleration.y * time;

			// X
			pObjects.at(i)->acceleration.x = (pObjects.at(i)->force.x / pObjects.at(i)->mass);
			pObjects.at(i)->position.x += METERS_TO_PIXELS(pObjects.at(i)->velocityVec.x) * time + 0.5 * pObjects.at(i)->acceleration.x * pow(time, 2);
			pObjects.at(i)->velocityVec.x += pObjects.at(i)->acceleration.x * time;
		}
	}
}

void ModulePhysics::BounceGround(size_t i)
{
	if (pObjects.at(i)->bounceCoef > 0.00)
	{
		pObjects.at(0)->force.y -= ((pObjects.at(0)->velocityVec.y) / time) * pObjects.at(0)->mass * pObjects.at(i)->bounceCoef;
		pObjects.at(0)->velocityVec.x *= pObjects.at(i)->initial_bounceCoef;

		pObjects.at(0)->acceleration.y = (pObjects.at(0)->force.y / pObjects.at(0)->mass);
		pObjects.at(0)->velocityVec.y = pObjects.at(0)->acceleration.y * time;
		pObjects.at(i)->bounceCoef -= 0.15;
	}
	else
	{
		pObjects.at(0)->active = false;
		App->scene_intro->resetball();
	}
}

void ModulePhysics::BounceRoof()
{
	pObjects.at(0)->force.y += ((pObjects.at(0)->velocityVec.y * -1) / time) * pObjects.at(0)->mass;
	pObjects.at(0)->acceleration.y = (pObjects.at(0)->force.y / pObjects.at(0)->mass);
	pObjects.at(0)->velocityVec.y = pObjects.at(0)->acceleration.y * time;
}

void ModulePhysics::BounceWall()
{
	pObjects.at(0)->force.x -= ((pObjects.at(0)->velocityVec.x) / time) * pObjects.at(0)->mass;

	pObjects.at(0)->acceleration.x = (pObjects.at(0)->force.x / pObjects.at(0)->mass);
	pObjects.at(0)->velocityVec.x = pObjects.at(0)->acceleration.x * time;
}

void ModulePhysics::Collision_NoAdjustment()
{
	for (size_t i = 1; i < pObjects.size(); i++)
	{
		if (SDL_HasIntersection(&pObjects.at(0)->objectRect, &pObjects.at(i)->objectRect) && pObjects.at(i)->etype == EntityType::GROUND)
		{
			pObjects.at(0)->active = false;
		}
	}
}

void ModulePhysics::Collision_Teleport()
{
	for (size_t i = 1; i < pObjects.size(); i++)
	{
		if (SDL_HasIntersection(&pObjects.at(0)->objectRect, &pObjects.at(i)->objectRect) && pObjects.at(i)->etype == EntityType::GROUND)
		{
			if ((pObjects.at(0)->position.y + pObjects.at(0)->objectRect.h) <= (pObjects.at(i)->position.y + pObjects.at(i)->objectRect.h)) {
				if ((pObjects.at(0)->position.x + pObjects.at(0)->objectRect.w) <= (pObjects.at(i)->position.x + pObjects.at(i)->objectRect.w)) {
					if (pObjects.at(0)->position.x >= pObjects.at(i)->position.x) {
						if (pObjects.at(0)->position.y <= (pObjects.at(i)->position.y + pObjects.at(i)->objectRect.h)) {
							pObjects.at(0)->position.y = pObjects.at(i)->position.y - pObjects.at(0)->objectRect.h/2 - 1;
							BounceGround(i);
							break;
						}
					}
				}
			}

			if ((pObjects.at(0)->position.y + pObjects.at(0)->objectRect.h) >= pObjects.at(i)->objectRect.y &&
				pObjects.at(0)->position.y <= (pObjects.at(i)->position.y + pObjects.at(i)->objectRect.h) && 
				(pObjects.at(0)->position.x + pObjects.at(0)->objectRect.w) >= (pObjects.at(i)->position.x + pObjects.at(i)->objectRect.w) ||
				(pObjects.at(0)->position.x <= pObjects.at(i)->position.x))
			{
				pObjects.at(0)->position.x = pObjects.at(i)->position.x + pObjects.at(i)->objectRect.w + 1 + (pObjects.at(0)->objectRect.w/2);
				BounceWall();
			}
			else {
				pObjects.at(0)->position.y = pObjects.at(i)->objectRect.y + pObjects.at(i)->objectRect.h + 1 + pObjects.at(0)->objectRect.h;
				BounceRoof();
			}
		}
	}
}

void ModulePhysics::Collision_Iterative()
{
	for (size_t i = 1; i < pObjects.size(); i++)
	{
		if (SDL_HasIntersection(&pObjects.at(0)->objectRect, &pObjects.at(i)->objectRect) && pObjects.at(i)->etype == EntityType::GROUND)
		{
			if ((pObjects.at(0)->position.y + pObjects.at(0)->objectRect.h) <= (pObjects.at(i)->position.y + pObjects.at(i)->objectRect.h)) {
				if ((pObjects.at(0)->position.x + pObjects.at(0)->objectRect.w) <= (pObjects.at(i)->position.x + pObjects.at(i)->objectRect.w)) {
					if (pObjects.at(0)->position.x >= pObjects.at(i)->position.x) {
						if (pObjects.at(0)->position.y <= (pObjects.at(i)->position.y + pObjects.at(i)->objectRect.h)) {
							while (((pObjects.at(0)->position.y + pObjects.at(0)->objectRect.h / 2 + 1) > pObjects.at(i)->position.y) && pObjects.at(i)->bounceCoef > 0.00)
							{
								if (pObjects.at(0)->velocityVec.x > 0) {
									pObjects.at(0)->position.x++;
								}
								else {
									pObjects.at(0)->position.x--;
								}
								pObjects.at(0)->position.y--;
							}

							BounceGround(i);
							break;
						}
					}
				}
			}

			if ((pObjects.at(0)->position.y + pObjects.at(0)->objectRect.h) >= pObjects.at(i)->objectRect.y &&
				pObjects.at(0)->position.y <= (pObjects.at(i)->position.y + pObjects.at(i)->objectRect.h) &&
				(pObjects.at(0)->position.x + pObjects.at(0)->objectRect.w) >= (pObjects.at(i)->position.x + pObjects.at(i)->objectRect.w) ||
				(pObjects.at(0)->position.x <= pObjects.at(i)->position.x))
			{
				while ((pObjects.at(0)->position.x - 12) < (pObjects.at(i)->position.x + pObjects.at(i)->objectRect.w))
				{
					pObjects.at(0)->position.x++;
				}

				BounceWall();
				break;
			}
			else {
				while ((pObjects.at(0)->position.y - 12) < (pObjects.at(i)->position.y + pObjects.at(i)->objectRect.h))
				{
					if (pObjects.at(0)->velocityVec.x > 0) {
						pObjects.at(0)->position.x++;
					}
					else {
						pObjects.at(0)->position.x--;
					}
					pObjects.at(0)->position.y++;
				}

				BounceRoof();
				break;
			}
		}
	}
}

void ModulePhysics::Collision_Raycast()
{
	for (size_t i = 1; i < pObjects.size(); i++)
	{
		if (SDL_HasIntersection(&pObjects.at(0)->objectRect, &pObjects.at(i)->objectRect) && pObjects.at(i)->etype == EntityType::GROUND)
		{
			BounceGround(i);
		}
	}
}

void ModulePhysics::setUpVelocity()
{
	for (size_t i = 0; i < pObjects.size(); i++)
	{
		if (pObjects.at(i)->active == true)
		{
			pObjects.at(i)->setUpVelocity();
		}
	}
}

void ModulePhysics::reset(PhysicEntity* pObjects)
{
	pObjects->force.y = 0;
	pObjects->force.x = 0;
	pObjects->acceleration.y = 0;
	pObjects->velocityVec.y = 0;
	pObjects->acceleration.x = 0;
	pObjects->velocityVec.x = 0;
}
