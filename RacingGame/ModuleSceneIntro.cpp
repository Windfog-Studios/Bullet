#include "Globals.h"
#include "Application.h"
#include "ModuleSceneIntro.h"
#include "Primitive.h"
#include "PhysBody3D.h"

ModuleSceneIntro::ModuleSceneIntro(bool start_enabled) : Module(start_enabled)
{
}

ModuleSceneIntro::~ModuleSceneIntro()
{}

// Load assets
bool ModuleSceneIntro::Start()
{
	LOG("Loading Intro assets");
	bool ret = true;
	int k = 0;

	App->camera->Move(vec3(1.0f, 1.0f, 0.0f));
	App->camera->LookAt(vec3(App->player->position.x, App->player->position.y, App->player->position.z));

	const int SnakeLength = 7;
	const float StartingSize = 0.5f;
	const float SizeIncrement = 0.2f;
	const float BallDistance = 0.3f;

	CreateBuildings();
	CreatePizza();

	float XPos = 0.f;
	float Size = StartingSize;
	/*
	for (int n = 0; n < SnakeLength; n++)
	{
		Sphere* s = new Sphere(Size);
		primitives.PushBack(s);
		s->SetPos(XPos, 10.f, -2.5f);
		//TODO 2: Link all the spheres with your P2P constraints
		if (n > 0)
		{
			App->physics->AddConstraintP2P(*s, **primitives.At(n - 1),btVector3(s->GetRadius(),0,0), btVector3(-s->GetRadius(), 0, 0));
		}
		k = n;
	}
	*/

	XPos = 10.f;
	Size = StartingSize;
	/*
	for (int n = 0; n < SnakeLength; n++)
	{
		Sphere* s = new Sphere(Size);
		primitives.PushBack(s);
		s->SetPos(XPos, 10.f, 2.5f);
		if (n > 0)
		{
			App->physics->AddConstraintHinge(*s, **primitives.At(k),
				btVector3(s->GetRadius(), 0, 0), btVector3(-s->GetRadius(),0, 0),
				btVector3(0,0,1),btVector3(0,0,1));
		}
		k++;
	}
	*/
	return ret;
}

// Load assets
bool ModuleSceneIntro::CleanUp()
{
	LOG("Unloading Intro scene");

	return true;
}

void ModuleSceneIntro::HandleDebugInput()
{
	if (App->input->GetKey(SDL_SCANCODE_1) == KEY_DOWN)
		DebugSpawnPrimitive(new Sphere());
	if (App->input->GetKey(SDL_SCANCODE_2) == KEY_DOWN)
		DebugSpawnPrimitive(new Cube());
	if (App->input->GetKey(SDL_SCANCODE_3) == KEY_DOWN)
		DebugSpawnPrimitive(new Cylinder());
	if (App->input->GetKey(SDL_SCANCODE_4) == KEY_DOWN)
		for (uint n = 0; n < primitives.Count(); n++)
			primitives[n]->SetPos((float)(std::rand() % 40 - 20), 10.f, (float)(std::rand() % 40 - 20));
	if (App->input->GetKey(SDL_SCANCODE_5) == KEY_DOWN)
		for (uint n = 0; n < primitives.Count(); n++)
			primitives[n]->body.Push(vec3((float)(std::rand() % 500) - 250, 500, (float)(std::rand() % 500) - 250));

	if (App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_DOWN)
	{
		//TODO: NEW CODE
		//A snippet of new code that may be useful for you. Nothing to do here really


		//Get a vector indicating the direction from the camera viewpoint to the "mouse"
		const vec2 mousePos(((float)App->input->GetMouseX() / (float)App->window->Width()) * 2.f - 1.f,
			-((float)App->input->GetMouseY() / (float)App->window->Height()) * 2.f + 1.f);
		const vec4 rayEye = inverse(App->renderer3D->ProjectionMatrix) * vec4(mousePos.x, mousePos.y, -1.f, 1.f);
		const vec4 rayWorld(inverse(App->camera->GetViewMatrix()) * vec4(rayEye.x, rayEye.y, -1.f, 0.f));

		vec3 Dir(rayWorld.x, rayWorld.y, rayWorld.z);
		//Cast a ray from the camera, in the "mouse" direction
		PhysBody3D* body = App->physics->RayCast(App->camera->Position, Dir);
		if (body)
		{
			//Change the color of the clicked primitive
			body->parentPrimitive->color = Color((float)(std::rand() % 255) / 255.f, (float)(std::rand() % 255) / 255.f, (float)(std::rand() % 255) / 255.f);
		}
	}
}

void ModuleSceneIntro::DebugSpawnPrimitive(Primitive * p)
{
	primitives.PushBack(p);
	p->SetPos(App->camera->Position.x, App->camera->Position.y, App->camera->Position.z);
	p->body.collision_listeners.PushBack(this);
	p->body.Push(-App->camera->Z * 1000.f);
}

// Update
update_status ModuleSceneIntro::Update(float dt)
{
	Plane p(vec3(0, 1, 0));
	p.axis = true;
	p.Render();
	
	if (App->debug == true)
		HandleDebugInput();

	for (uint n = 0; n < primitives.Count(); n++)
		primitives[n]->Update();

	return UPDATE_CONTINUE;
}

update_status ModuleSceneIntro::PostUpdate(float dt)
{
	for (uint n = 0; n < primitives.Count(); n++)
	{
		primitives[n]->Render();
	}

	return UPDATE_CONTINUE;
}

void ModuleSceneIntro::OnCollision(PhysBody3D * body1, PhysBody3D * body2)
{
	Color color = Color((float)(std::rand() % 255) / 255.f, (float)(std::rand() % 255) / 255.f, (float)(std::rand() % 255) / 255.f);

	body1->parentPrimitive->color = color;
	body2->parentPrimitive->color = color;

}

void ModuleSceneIntro::addCheckPoint(vec3 pos)
{
	Cube checkpoint;
	checkpoint.SetPos(pos.x, pos.y, pos.z);
	checkpoint.color = Green;

	//checkpointList.PushBack(checkpoint);
}

void ModuleSceneIntro::CreateBuildings()
{
	//Collider limit buildings	
	{
		//Map limits at (x,y,z) = (175, y, 180)		
		{//Map limits front (x,y,180)
			Cube* front_limit_building0 = new Cube({ 100, 40, 30 }, 10000);
			front_limit_building0->color = Grey;
			front_limit_building0->SetPos(140, 0, 180);
			primitives.PushBack(front_limit_building0);

			Cube* front_limit_building1 = new Cube({ 70, 70, 30 }, 10000);
			front_limit_building1->color = Grey;
			front_limit_building1->SetPos(60, 0, 180);
			primitives.PushBack(front_limit_building1);

			Cube* front_limit_building2 = new Cube({ 100, 50, 30 }, 10000);
			front_limit_building2->color = Grey;
			front_limit_building2->SetPos(-20, 0, 180);
			primitives.PushBack(front_limit_building2);

			Cube* front_limit_building3 = new Cube({ 50, 100, 30 }, 10000);
			front_limit_building3->color = Grey;
			front_limit_building3->SetPos(-100, 50, 180);
			primitives.PushBack(front_limit_building3);

			Cube* front_limit_building4 = new Cube({ 50, 70, 30 }, 10000);
			front_limit_building4->color = Grey;
			front_limit_building4->SetPos(-130, 0, 180);
			primitives.PushBack(front_limit_building4);
		}
		{//Map limits left (175,y,z)
			Cube* left_limit_building0 = new Cube({ 30, 70, 30 }, 10000);
			left_limit_building0->color = Grey;
			left_limit_building0->SetPos(175, 0, 150);
			primitives.PushBack(left_limit_building0);

			Cube* left_limit_building1 = new Cube({ 30, 50, 60 }, 10000);
			left_limit_building1->color = Grey;
			left_limit_building1->SetPos(175, 0, 105);
			primitives.PushBack(left_limit_building1);

			Cube* left_limit_building2 = new Cube({ 30, 40, 100 }, 10000);
			left_limit_building2->color = Grey;
			left_limit_building2->SetPos(175, 0, 25);
			primitives.PushBack(left_limit_building2);

			Cube* left_limit_building3 = new Cube({ 30, 70, 30 }, 10000);
			left_limit_building3->color = Grey;
			left_limit_building3->SetPos(175, 0, -40);
			primitives.PushBack(left_limit_building3);

			Cube* left_limit_building4 = new Cube({ 30, 60, 120 }, 10000);
			left_limit_building4->color = Grey;
			left_limit_building4->SetPos(175, 0, -95);
			primitives.PushBack(left_limit_building4);
		}
		{//Map limits right
			Cube* right_limit_building0 = new Cube({ 30, 70, 30 }, 10000);
			right_limit_building0->color = Grey;
			right_limit_building0->SetPos(-175, 0, 150);
			primitives.PushBack(right_limit_building0);

			Cube* right_limit_building1 = new Cube({ 30, 50, 60 }, 10000);
			right_limit_building1->color = Grey;
			right_limit_building1->SetPos(-175, 0, 105);
			primitives.PushBack(right_limit_building1);

			Cube* right_limit_building2 = new Cube({ 30, 40, 100 }, 10000);
			right_limit_building2->color = Grey;
			right_limit_building2->SetPos(-175, 0, 25);
			primitives.PushBack(right_limit_building2);

			Cube* right_limit_building3 = new Cube({ 30, 120, 80 }, 10000);
			right_limit_building3->color = Grey;
			right_limit_building3->SetPos(-150, 0, -40);
			primitives.PushBack(right_limit_building3);

			Cube* right_limit_building4 = new Cube({ 30, 60, 90 }, 10000);
			right_limit_building4->color = Grey;
			right_limit_building4->SetPos(-175, 0, -125);
			primitives.PushBack(right_limit_building4);
		}
		{//Map limits back
			Cube* back_limit_building0 = new Cube({ 100, 40, 30 }, 10000);
			back_limit_building0->color = Grey;
			back_limit_building0->SetPos(140, 0, -180);
			primitives.PushBack(back_limit_building0);

			Cube* back_limit_building1 = new Cube({ 70, 70, 30 }, 10000);
			back_limit_building1->color = Grey;
			back_limit_building1->SetPos(60, 0, -180);
			primitives.PushBack(back_limit_building1);

			Cube* back_limit_building2 = new Cube({ 100, 50, 30 }, 10000);
			back_limit_building2->color = Grey;
			back_limit_building2->SetPos(-20, 0, -180);
			primitives.PushBack(back_limit_building2);

			Cube* back_limit_building3 = new Cube({ 50, 100, 30 }, 10000);
			back_limit_building3->color = Grey;
			back_limit_building3->SetPos(-100, 50, -180);
			primitives.PushBack(back_limit_building3);

			Cube* back_limit_building4 = new Cube({ 50, 70, 30 }, 10000);
			back_limit_building4->color = Grey;
			back_limit_building4->SetPos(-130, 0, -180);
			primitives.PushBack(back_limit_building4);
		}
	}
	//City buildings
	{

	}
}

void ModuleSceneIntro::CreatePizza()
{
	// pizza 1
	{
		Cube* base1 = new Cube({ 2, 0.2f, 2 }, 0);
		primitives.PushBack(base1);
		base1->SetPos(5, 1, 0);
		base1->color = Beige;

		Cube* tape1 = new Cube({ 2, 0.2f, 2 }, 0);
		tape1->transform.rotate(60.f, vec3(0, 0, 1));
		primitives.PushBack(tape1);
		tape1->SetPos(6.5f, 1.9f, 0);
		tape1->color = Beige;

		Cylinder* pizza1 = new Cylinder(0.8, 0.1f, 0);
		pizza1->transform.rotate(90.f, vec3(0, 0, 1));
		primitives.PushBack(pizza1);
		pizza1->SetPos(5, 1.1f, 0);
		pizza1->color = Red;

	}
}

void ModuleSceneIntro::CreateFence()
{
	int k = 0;

	const int SnakeLength = 7;
	const float StartingSize = 0.5f;
	const float SizeIncrement = 0.2f;
	const float BallDistance = 0.3f;

	float XPos = 0.f;
	float Size = StartingSize;

	for (int n = 0; n < SnakeLength; n++)
	{
		Cube* fence = new Cube({ 2, 0.2f, 2 }, 10000);
		primitives.PushBack(fence);
		fence->SetPos(XPos, 10.f, -2.5f);
		//TODO 2: Link all the spheres with your P2P constraints
		if (n > 0)
		{
			//App->physics->AddConstraintP2P(*fence, **primitives.At(n - 1), btVector3(fence->axis, btVector3(-fence->GetRadius(), 0, 0));
		}
		k = n;
	}

}