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
	void UpdateSensor();

public:
	PhysVehicle3D* vehicle;
	Cylinder* sensor;
	Timer timer;
	Timer timer2;
	Timer boost_duration;
	Timer boost_cooldown;
	
	float	max_time;
	float	movement_blocking_max_time;
	float	time_left;
	float	left_blocking_time;
	float	turn;
	float	acceleration;
	float	brake;
	int		max_speed;

	vec3	position;
	vec3	initial_position;
	vec3	forward;

	bool	top_view = false;

	unsigned int mamma_mia = 0;
	unsigned int Delivery = 0;
};