#include "Globals.h"
#include "Application.h"
#include "ModulePlayer.h"
#include "Primitive.h"
#include "PhysVehicle3D.h"
#include "PhysBody3D.h"
#include <tgmath.h>

ModulePlayer::ModulePlayer(bool start_enabled) : Module(start_enabled), vehicle(NULL)
{
	turn = acceleration = brake = 0.0f;
	max_time = 30;
	max_time2 = 4;
	max_time3 = 2;
	max_time4 = 5;
}

ModulePlayer::~ModulePlayer()
{}

// Load assets
bool ModulePlayer::Start()
{
	LOG("Loading player");

	VehicleInfo car;

	// Car properties ----------------------------------------
	car.mass = 500.0f;
	car.suspensionStiffness = 13.88f;
	car.suspensionCompression = 0.93f;
	car.suspensionDamping = 0.88f;
	car.maxSuspensionTravelCm = 1000.0f;
	car.frictionSlip = 50.5;
	car.maxSuspensionForce = 6000.0f;

	//Chassis
	car.chassis_size.Set(1, 1, 3.5);
	car.chassis_offset.Set(0, 1.0, 0);

	car.cabin_size.Set(1.1, 1.75, 1.25);
	car.cabin_offset.Set(0, 1.25, 1);

	car.trunk_size.Set(2, 1.95, 3.25);
	car.trunk_offset.Set(0, 1.55, -1.3);

	// Wheel properties ---------------------------------------
	float connection_height = 1.2f;
	float back_wheels_displacement = -0.4;
	float wheel_radius = 0.55f;
	float wheel_width = 0.38f;
	float suspensionRestLength = 1.15f;

	// Don't change anything below this line ------------------

	float half_width = car.chassis_size.x*0.5f;
	float half_length = car.chassis_size.z*0.5f;

	vec3 direction(0,-1,0);
	vec3 axis(-1,0,0);

	car.num_wheels = 3;
	car.wheels = new Wheel[3];

	// FRONT-LEFT ------------------------
	car.wheels[0].connection.Set(0, connection_height - 0.2, half_length - wheel_radius + 0.4f);
	car.wheels[0].direction = direction;
	car.wheels[0].axis = axis;
	car.wheels[0].suspensionRestLength = suspensionRestLength;
	car.wheels[0].radius = wheel_radius;
	car.wheels[0].width = wheel_width;
	car.wheels[0].front = true;
	car.wheels[0].drive = true;
	car.wheels[0].brake = false;
	car.wheels[0].steering = true;

	// REAR-LEFT ------------------------
	car.wheels[1].connection.Set(half_width + 1.2f * wheel_width, connection_height, -half_length + wheel_radius + back_wheels_displacement);
	car.wheels[1].direction = direction;
	car.wheels[1].axis = axis;
	car.wheels[1].suspensionRestLength = suspensionRestLength;
	car.wheels[1].radius = wheel_radius;
	car.wheels[1].width = wheel_width * 1.4f;
	car.wheels[1].front = false;
	car.wheels[1].drive = false;
	car.wheels[1].brake = true;
	car.wheels[1].steering = false;

	// REAR-RIGHT ------------------------
	car.wheels[2].connection.Set(-half_width - 1.2f * wheel_width, connection_height, -half_length + wheel_radius + back_wheels_displacement);
	car.wheels[2].direction = direction;
	car.wheels[2].axis = axis;
	car.wheels[2].suspensionRestLength = suspensionRestLength;
	car.wheels[2].radius = wheel_radius;
	car.wheels[2].width = wheel_width * 1.4f;
	car.wheels[2].front = false;
	car.wheels[2].drive = false;
	car.wheels[2].brake = true;
	car.wheels[2].steering = false;

	sensor = new Cylinder(3, 4);
	sensor->body.collision_listeners.PushBack(this);
	sensor->body.SetAsSensor(true);

	arrow = new Cube(vec3(0.5, 0.05, 0.05));
	arrow->color = Green;

	arrowTopHead = new Cube(vec3(0.25, 0.05, 0.05));
	arrowTopHead->color = Green;
	arrowTopHead->transform.rotate(0, vec3(0, 1, 0));

	arrowBottomHead = new Cube(vec3(0.25, 0.05, 0.05));
	arrowBottomHead->color = Green;
	arrowBottomHead->transform.rotate(-90, vec3(0, 1, 0));

	vehicle = App->physics->AddVehicle(car);
	vehicle->SetPos(0, 0, 0);
	initial_position = vehicle->position;

	timer2.Start();
	boost_cooldown.Start();

	mamma_mia = App->audio->LoadFx("MammaMia.wav");
	Delivery = App->audio->LoadFx("Delivery.wav");

	return true;
}

// Unload assets
bool ModulePlayer::CleanUp()
{
	LOG("Unloading player");
	delete sensor;
	delete arrow;
	delete arrowTopHead;
	delete arrowBottomHead;
	return true;
}

// Update: draw background
update_status ModulePlayer::Update(float dt)
{
	turn = acceleration = brake = 0.0f;
	vec3 forward = vehicle->GetForwardVector();

	time_left2 = max_time2 - timer2.Read() * 0.001f;
	//Input
	if ((App->input->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT) && (vehicle->GetKmh() < 120))
	{
		acceleration = MAX_ACCELERATION;
	}

	if (App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT)
	{
		if (turn < TURN_DEGREES)
			turn += TURN_DEGREES;
	}

	if (App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT)
	{
		if (turn > -TURN_DEGREES)
			turn -= TURN_DEGREES;
	}

	if (App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT)
	{
		acceleration = -MAX_ACCELERATION;
	}

	if (App->input->GetKey(SDL_SCANCODE_Y) == KEY_REPEAT)
	{
		acceleration = MAX_ACCELERATION * 2;
	}
	
	if (App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_REPEAT) {
		if (vehicle->HasBody())
		{
			//vehicle->GetBody()->activate();
			//vehicle->GetBody()->applyCentralForce(btVector3((float)1, (float)1000, (float)1));
			vehicle->SetAngularVelocity(0, 0, 0);
			//vehicle->SetLinearVelocity(0, 0, 0);
			if (floor(vehicle->GetKmh()) > 2)
			{
				acceleration = -MAX_ACCELERATION * 10;
			}
			else if (floor(vehicle->GetKmh()) < -2)
			{
				acceleration = MAX_ACCELERATION * 10;
			}
			//vehicle.bo
		}
	}

	//camera control
	if (!App->scene_intro->showing_winning_map)
	{
		if (App->input->GetMouseButton(SDL_BUTTON_RIGHT) == KEY_REPEAT)
		{
			App->camera->LookAt(vec3(position.x, position.y, position.z));
		}
		else if (!App->debug)
		{
			if (!top_view)
			{
				App->camera->Position.Set(vehicle->position.x - forward.x * 10, 5.5f, vehicle->position.z - forward.z * 10);
			}
			App->camera->LookAt(vec3(position.x, position.y + 1.5f, position.z));
		}
	}

	if (App->input->GetKey(SDL_SCANCODE_T) == KEY_DOWN)
	{
		if (top_view) {
			top_view = false;
		}
		else
		{
			top_view = true;
			App->camera->Position.x = 0.0f;
			App->camera->Position.y = 350.0f;
			App->camera->Position.z = 30.0f;
		}
	}

	//update sensor and timer
	UpdateSensorAndBar(forward);

	position = vehicle->position;

	//Render
	//timer_cube->Render();
	
	arrow->Render();
	arrowTopHead->Render();
	arrowBottomHead->Render();

	vehicle->Render();
	//sensor->Render();

	if (time_left <= 0)
		RestartGame();

	if (!App->scene_intro->showing_winning_map)
	{
		char title[80];
		sprintf_s(title, "Racing Game %.1f Km/h Time left %.2f Pizzas: %i", vehicle->GetKmh(), time_left, App->scene_intro->p);
		App->window->SetTitle(title);
	}

	if (time_left2 >= 0)
	{
		acceleration = turn = 0;
		char title[30];
		sprintf_s(title, "Time to go: %i", (int)time_left2);
		App->window->SetTitle(title);
	}


	vehicle->ApplyEngineForce(acceleration);
	vehicle->Turn(turn);
	vehicle->Brake(brake);

	return UPDATE_CONTINUE;
}

void ModulePlayer::RestartGame() {
	App->scene_intro->Load();
	vehicle->SetPos(initial_position.x, initial_position.y, initial_position.z);
	vehicle->GetBody()->setLinearVelocity(btVector3(0, 0, 0));
	time_left = max_time;
	timer.Start();
	App->scene_intro->p = -1;
	App->scene_intro->changePizzaPosition(-1);
}

void ModulePlayer::OnCollision(PhysBody3D* body1, PhysBody3D* body2) {

	if (body2->isPizza)
	{
		App->audio->PlayFx(Delivery);
		App->scene_intro->changePizzaPosition(App->scene_intro->p);
		time_left = max_time;
		timer.Start();
	}
}

void ModulePlayer::UpdateSensorAndBar(vec3 forward) {
	vec3 target;	
	double angle;
	sensor->Update();
	sensor->body.GetBody()->applyForce(btVector3(0, -GRAVITY.y(), 0), btVector3(0, 0, 0));
	sensor->SetPos(vehicle->position.x, 2, vehicle->position.z - 0.5);

	time_left = max_time - timer.Read() * 0.001f;
	
	if (time_left <= 0)	
		App->audio->PlayFx(mamma_mia);
	
	//arrow->SetSize(vec3(0.75 * (time_left / max_time), 0.05, 0.05));
	arrow->body.GetBody()->applyForce(btVector3(0, -GRAVITY.y(), 0), btVector3(0, 0, 0));
	arrow->SetPos(App->camera->Position.x + forward.x, App->camera->Position.y + 0.175, App->camera->Position.z + forward.z);

	//Red
	arrowTopHead->SetPos(App->camera->Position.x + forward.x + 0.05, App->camera->Position.y + 0.175, App->camera->Position.z + forward.z + 0.15);

	//Blue
	arrowBottomHead->SetPos(App->camera->Position.x + forward.x + 0.15, App->camera->Position.y + 0.175, App->camera->Position.z + forward.z + 0.05);

	target = App->scene_intro->pizza_position[App->scene_intro->p];
	angle = atan((target.z - arrow->GetPos().z) / (target.x - arrow->GetPos().x));
	
	//Rotation to target
	arrow->transform.rotate(angle * RADTODEG + 90, vec3(0, 1, 0));
	arrowTopHead->transform.rotate(angle * RADTODEG + 120, vec3(0, 1, 0));
	arrowBottomHead->transform.rotate(angle * RADTODEG + 60, vec3(0, 1, 0));

	float number = 0.01f;

	/*LOG("pizza x: %f", target.x);
	LOG("pizza z: %f", target.z);
	LOG("arrow x: %f", arrow->GetPos().x);
	LOG("arrow z: %f", arrow->GetPos().z);
	LOG("----------------");*/
	LOG("Rotation z: %f", arrow_rotation);
	
	/*if(arrow->GetPos().x > target.x)
	{
		//Rotate right
		LOG("Rotate Right");
		arrow->transform.rotate((arrow_rotation + number) * RADTODEG, vec3(0, 1, 0));
		//arrowTopHead->transform.rotate(number, vec3(0, 1, 0));
		//arrowBottomHead->transform.rotate(number, vec3(0, 1, 0));
		arrow_rotation += number;
	}else if (arrow->GetPos().x < target.x)
	{
		LOG("Rotate Left");
		//Rotate left
		arrow->transform.rotate((arrow_rotation - number) * RADTODEG, vec3(0, 1, 0));
		//arrowTopHead->transform.rotate(-number, vec3(0, 1, 0));
		//arrowBottomHead->transform.rotate(-number, vec3(0, 1, 0));
		arrow_rotation -= number;
	}*/
}
