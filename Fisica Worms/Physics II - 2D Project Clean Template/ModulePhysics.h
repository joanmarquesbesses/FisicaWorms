#pragma once
#include "Module.h"
#include "Globals.h"
#include "Timer.h"
#include <vector>

#include "Ball.h"
#include "Terrain.h"

#define PIXELS_PER_METER 50.0f // if touched change METER_PER_PIXEL too
#define METER_PER_PIXEL 0.02f // this is 1 / PIXELS_PER_METER !

#define METERS_TO_PIXELS(m) ((int) floor(PIXELS_PER_METER * m))
#define PIXEL_TO_METERS(p)  ((float) METER_PER_PIXEL * p)

enum class Collisions {
	NO,
	TELEPORT,
	ITERATIVE,
	RAYCAST
};

enum class Integrators {
	EULER,
	SYMPLETIC,
	VERLET
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

	//	
	void Calculate_Gravity();
	void Calculate_Aerodynamics();
	void Calculate_Hydrodinamics();

	// Integrators
	void Integrator_Euler();
	void Integrator_SympleticEuler();
	void Integrator_VelocityVerlet();

	// Bouncing the Ball
	void BounceGround(size_t index);
	void BounceWall();
	void BounceRoof();

	// Collisions
	void Collision_NoAdjustment();
	void Collision_Teleport();
	void Collision_Iterative();
	void Collision_Raycast();

	void setUpVelocity();
	void reset(PhysicEntity* pObjects);

	std::vector <PhysicEntity*> pObjects;
	
public:
	bool time1 = true;
	bool time2 = false;
	float time;

	bool launch, backtrack = false;
	Timer flytime;
	bool debug;
	float velocityx, velocityy;

	bool enableLift = true;
	bool enableWater = true;

	float airDensity = 1.3f;
	float waterDensity = 1.0f;
	float bCoef = 20.0f;

	Collisions collision = Collisions::TELEPORT;
	Integrators integrator = Integrators::EULER;
};