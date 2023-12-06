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

	return true;
}


update_status ModulePhysics::PreUpdate()
{
	time = (App->dt / 1000);
	Calculate_Gravity();
	Calculate_Aerodynamics();
	
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
		debug = !debug;

	if(!debug)
		return UPDATE_CONTINUE;

	static char title[256];
	sprintf_s(title, 256, "Initial Vel: %.2f Angle: %.2f VelX: %.2f VelY: %.2f, dt: %.2f, airDen: %.2f", 
		pObjects.at(0)->velocity, pObjects.at(0)->angle, pObjects.at(0)->velocityVec.x, pObjects.at(0)->velocityVec.y, App->dt, airDensity);

	App->window->SetTitle(title);

	return UPDATE_CONTINUE;
}


// Called before quitting
bool ModulePhysics::CleanUp()
{
	LOG("Destroying physics world");

	return true;
}

//void ModulePhysics::setBallPointer(PhysicEntity* ball)
//{
//	this->ball = ball;
//}

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
				float lift = (0.5 * (airDensity * METERS_TO_PIXELS(pow(pObjects.at(i)->velocityVec.y, 2)) * pObjects.at(i)->surface * 0.1)) * -1;
				if (pObjects.at(i)->velocityVec.y > 0.0f) 
				{
					pObjects.at(i)->force.y += lift;
				}
			}

			
			if (enableLift) 
			{
				// drag
				float drag = (0.5 * (airDensity *	METERS_TO_PIXELS(pow(pObjects.at(i)->velocityVec.y, 2)) * pObjects.at(i)->surface * 0.01));
				if (pObjects.at(i)->velocityVec.y < 0.0f) 
				{
					pObjects.at(i)->force.y += drag;
				}
			}
		}
	}
}

void ModulePhysics::Calculate_Hydrodinamics()
{
	//for (size_t i = 0; i < pObjects.size(); i++)
	//{
	//	if (pObjects.at(i)->active == true)
	//	{
	//		if (enableWater)
	//		{
	//			// lift
	//			float lift = (0.5 * (airDensity * METERS_TO_PIXELS(pow(pObjects.at(i)->velocityVec.y, 2)) * pObjects.at(i)->surface * 0.1)) * -1;
	//			if (pObjects.at(i)->velocityVec.y > 0.0f)
	//			{
	//				pObjects.at(i)->force.y += lift;
	//			}
	//		}


	//		if (enableWater)
	//		{
	//			// drag
	//			float drag;
	//			if (pObjects.at(i)->velocityVec.y < 0.0f)
	//			{
	//				pObjects.at(i)->force.y += drag;
	//			}
	//		}

	//		if (enableWater)
	//		{
	//			// bouyancy
	//			float bouyancy = waterDensity * App->gravity * pObjects.at(0)->volumne;
	//			if (pObjects.at(i)->velocityVec.y > 0.0f)
	//			{
	//				pObjects.at(i)->force.y += bouyancy;
	//			}
	//		}
	//	}
	//}
}

void ModulePhysics::Integrator_Euler()
{
	for (size_t i = 0; i < pObjects.size(); i++)
	{
		if (pObjects.at(i)->active == true) 
		{
			// Y
			pObjects.at(i)->acceleration.y = (pObjects.at(i)->force.y / pObjects.at(i)->mass);
			pObjects.at(i)->position.y += METERS_TO_PIXELS(pObjects.at(i)->velocityVec.y) * (App->dt / 1000);
			pObjects.at(i)->velocityVec.y += pObjects.at(i)->acceleration.y * (App->dt / 1000);

			// X
			pObjects.at(i)->acceleration.x = (pObjects.at(i)->force.x / pObjects.at(i)->mass);
			pObjects.at(i)->position.x += METERS_TO_PIXELS(pObjects.at(i)->velocityVec.x) * (App->dt / 1000);
			pObjects.at(i)->velocityVec.x += pObjects.at(i)->acceleration.x * (App->dt / 1000);
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
			pObjects.at(i)->velocityVec.y += pObjects.at(i)->acceleration.y * (App->dt / 1000);
			pObjects.at(i)->position.y += METERS_TO_PIXELS(pObjects.at(i)->velocityVec.y) * (App->dt / 1000);

			// X
			pObjects.at(i)->acceleration.x = (pObjects.at(i)->force.x / pObjects.at(i)->mass);
			pObjects.at(i)->velocityVec.x += pObjects.at(i)->acceleration.x * (App->dt / 1000);
			pObjects.at(i)->position.x += METERS_TO_PIXELS(pObjects.at(i)->velocityVec.x) * (App->dt / 1000);
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
			pObjects.at(i)->position.y += METERS_TO_PIXELS(pObjects.at(i)->velocityVec.y) * (App->dt / 1000) + 0.5 * pObjects.at(i)->acceleration.y * pow(App->dt / 1000, 2);
			pObjects.at(i)->velocityVec.y += pObjects.at(i)->acceleration.y * (App->dt / 1000);

			// X
			pObjects.at(i)->acceleration.x = (pObjects.at(i)->force.x / pObjects.at(i)->mass);
			pObjects.at(i)->position.x += METERS_TO_PIXELS(pObjects.at(i)->velocityVec.x) * (App->dt / 1000) + 0.5 * pObjects.at(i)->acceleration.x * pow(App->dt / 1000, 2);
			pObjects.at(i)->velocityVec.x += pObjects.at(i)->acceleration.x * (App->dt / 1000);
		}
	}
}

void ModulePhysics::Bounce(size_t i)
{
	if (pObjects.at(i)->bounceCoef < 0.00)
	{
		pObjects.at(0)->force.y *= pObjects.at(i)->bounceCoef;
		pObjects.at(i)->bounceCoef += 0.05;

		pObjects.at(0)->acceleration.y = (pObjects.at(0)->force.y / pObjects.at(0)->mass);
		pObjects.at(0)->velocityVec.y = pObjects.at(0)->acceleration.y;
	}
	else
	{
		pObjects.at(0)->active = false;
		App->scene_intro->resetball();
	}

	pObjects.at(0)->velocityVec.x *= 0.75;
}

void ModulePhysics::Collision_NoAdjustment()
{
	for (size_t i = 1; i < pObjects.size(); i++)
	{
		if (pObjects.at(0)->position.y + 11 > pObjects.at(i)->position.y && pObjects.at(i)->etype == EntityType::GROUND)
		{
			pObjects.at(0)->active = false;
		}
	}
}

void ModulePhysics::Collision_Teleport()
{
	for (size_t i = 1; i < pObjects.size(); i++)
	{
		if (pObjects.at(0)->position.y + 11 > pObjects.at(i)->position.y && pObjects.at(i)->etype == EntityType::GROUND)
		{
			pObjects.at(0)->position.y = pObjects.at(1)->position.y - 11;

			Bounce(i);
		}
	}
}

void ModulePhysics::Collision_Iterative()
{
	for (size_t i = 1; i < pObjects.size(); i++)
	{
		if (pObjects.at(0)->position.y + 11 > pObjects.at(i)->position.y && pObjects.at(i)->etype == EntityType::GROUND)
		{
			while (pObjects.at(0)->position.y + 11 > pObjects.at(i)->position.y && pObjects.at(i)->bounceCoef < 0.00)
			{
				pObjects.at(0)->position.x -= pObjects.at(0)->velocityVec.x;
				pObjects.at(0)->position.y -= pObjects.at(0)->velocityVec.y;
			}

			Bounce(i);
		}
	}
}

void ModulePhysics::Collision_Raycast()
{
	for (size_t i = 1; i < pObjects.size(); i++)
	{
		if (pObjects.at(0)->position.y + 11 > pObjects.at(i)->position.y && pObjects.at(i)->etype == EntityType::GROUND)
		{
			Bounce(i);
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
