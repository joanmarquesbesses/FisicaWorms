#pragma once
#include "Module.h"
#include "Globals.h"
#include "Timer.h"
#include <vector>

#include "Ball.h"

#define PIXELS_PER_METER 50.0f // if touched change METER_PER_PIXEL too
#define METER_PER_PIXEL 0.02f // this is 1 / PIXELS_PER_METER !

#define METERS_TO_PIXELS(m) ((int) floor(PIXELS_PER_METER * m))
#define PIXEL_TO_METERS(p)  ((float) METER_PER_PIXEL * p)

enum Collisions {
	No,
	Teleport,
	Iteratively,
	Raycast
};


class ModulePhysics : public Module
{
public:
	ModulePhysics(Application* app, bool start_enabled = true);
	~ModulePhysics();

	bool Start();
	update_status PreUpdate();
	update_status PostUpdate();
	bool CleanUp();


	void Calculate_Gravity();
	Force Calculate_Aerodynamics();
	Force Calculate_Hydrodinamics();

	void Integrator_Euler();
	void Integrator_SympleticEuler();
	void Integrator_VelocityVerlet();

	void setUpVelocity();

	std::vector <PhysicEntity*> pObjects;
public:
	bool time1 = true;
	bool time2 = false;
	float time;
	bool mode1 = true;
	bool mode2, mode3 = false;
	bool launch, backtrack = false;
	Timer flytime;
	bool debug;
	float velocityx, velocityy;
	Collisions collision = No;
};