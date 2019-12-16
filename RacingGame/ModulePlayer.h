#pragma once
#include "Module.h"
#include "Globals.h"
#include "p2Point.h"
#include "Timer.h"

struct PhysVehicle3D;

#define MAX_ACCELERATION 1750.0f
#define TURN_DEGREES 17.5f * DEGTORAD
#define BRAKE_POWER 1000.0f

class ModulePlayer : public Module
{
public:
	ModulePlayer(bool start_enabled = true);
	virtual ~ModulePlayer();

	bool Start();
	update_status Update(float dt);
	bool CleanUp();

	void OnCollision(PhysBody3D* body1, PhysBody3D* body2);
	void RestartGame();
	void UpdateSensorAndBar(vec3 forward);

public:

	PhysVehicle3D* vehicle;
	Cube* sensor;
	Cube* timer_cube;
	Timer timer;
	float max_time;
	float time_left;
	float turn;
	float acceleration;
	float brake;
	int max_speed;
	vec3 position;
	vec3 initial_position;

	unsigned int motorcycle = 0;
};