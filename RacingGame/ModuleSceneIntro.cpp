#include "Globals.h"
#include "Application.h"
#include "ModuleSceneIntro.h"
#include "Primitive.h"
#include "PhysBody3D.h"

ModuleSceneIntro::ModuleSceneIntro(bool start_enabled) : Module(start_enabled)
{
	//pizza positions
	/*pizza_position[0] = { 150, 0.1f, 150 }; // top left corner
	pizza_position[1] = { -150, 0.1f, -150 }; // bottom right corner
	pizza_position[2] = {-150, 0.1f, 150}; //top right corner
	pizza_position[3] = { 150, 0.1f, -150 }; //bottom left corner
	pizza_position[4] = { 25, 0.1f, -37.5f }; //center
	pizza_position[5] = { 150, 0.1f, -40.f }; //mid-left
	pizza_position[6] = { -145, 0.1f, 0 }; //mid right
	pizza_position[7] = { 95, 0.1f, 85 }; //corner left
	pizza_position[8] = { -67.5f, 0.1f, 82.5f }; //corner right
	pizza_position[9] = { -25, 0.1f, -152.5f }; //mid bottom*/
	
	//Debug for changing pizza position
	pizza_position[0] = { 5, 1.1f, 0 }; 
	pizza_position[1] = { 10, 1.1f, 0 }; 
	pizza_position[2] = { 15, 1.1f, 0 }; 
	pizza_position[3] = { 20, 1.1f, 0 }; 
	pizza_position[4] = { 25, 1.1f, 0 }; 
	pizza_position[5] = { 30, 1.1f, 0 }; 
}

ModuleSceneIntro::~ModuleSceneIntro()
{}

// Load assets
bool ModuleSceneIntro::Start()
{
	LOG("Loading Intro assets");
	bool ret = true;
	int k = 0;
	start = App->audio->LoadFx("Start.wav");
	App->audio->PlayFx(start);
	App->audio->PlayMusic("Italian_music.ogg", 1);
	App->audio->VolumeMusic(20);
	App->camera->Move(vec3(1.0f, 1.0f, 0.0f));

	const int SnakeLength = 7;
	const float StartingSize = 0.5f;
	const float SizeIncrement = 0.2f;
	const float BallDistance = 0.3f;

	CreateDecoration();
	CreateFence(10);
	CreateBuildings();
	CreatePizza();
	CreateBollards();
	
	
	float XPos = 10.f;
	float Size = StartingSize;
	
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
	p.color = Dark_Grey;
	p.axis = true;
	p.Render();
	
	if (App->debug == true)
		HandleDebugInput();

	for (uint n = 0; n < primitives.Count(); n++)
		primitives[n]->Update();

	//pizza_pointer->base->SetPos(pizza_position->x, pizza_position->y -0.1f, pizza_position->z);
	//pizza_pointer->tape->SetPos(pizza_position->x + 1.5f, pizza_position->y + 0.8f, pizza_position->z);

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
	//body2->parentPrimitive->color = color;

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
			front_limit_building3->SetPos(-100, 50, 170);
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

			Cube* right_limit_building2 = new Cube({ 30, 40, 158 }, 10000);
			right_limit_building2->color = Grey;
			right_limit_building2->SetPos(-170, 0, 0);
			primitives.PushBack(right_limit_building2);

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
	//City buildings 1
	
		{
			Cube* city_building1 = new Cube({ 30, 40, 40 }, 10000);
			city_building1->color = Dark_Grey;
			city_building1->SetPos(120, 0, 100);
			primitives.PushBack(city_building1);

			Cube* city_building2 = new Cube({ 30, 50, 30 }, 10000);
			city_building2->color = Darker_Grey;
			city_building2->SetPos(120, 0, 70);
			primitives.PushBack(city_building2);

			Cube* city_building3 = new Cube({ 30, 30, 80 }, 10000);
			city_building3->color = Bright_Grey;
			city_building3->SetPos(120, 0, 30);
			primitives.PushBack(city_building3);

			Cube* city_building4 = new Cube({ 30, 30, 30 }, 10000);
			city_building4->color = Bright_Grey;
			city_building4->SetPos(120, 0, -70);
			primitives.PushBack(city_building4);

			Cube* city_building5 = new Cube({ 30, 70, 50 }, 10000);
			city_building5->color = Darker_Grey;
			city_building5->SetPos(100, 0, -110);
			primitives.PushBack(city_building5);
		}
	
	
	//City buildings 2
	
		{
			Cube* city_building6 = new Cube({ 40, 30, 50 }, 10000);
			city_building6->color = Dark_Grey;
			city_building6->SetPos(100, 0, -110);
			primitives.PushBack(city_building6);

			Cube* city_building7 = new Cube({ 55, 50, 50 }, 10000);
			city_building7->color = Grey;
			city_building7->SetPos(15, 0, -110);
			primitives.PushBack(city_building7);

			Cube* city_building8 = new Cube({ 90, 70, 50 }, 10000);
			city_building8->color = Darker_Grey;
			city_building8->SetPos(-80, 0, -110);
			primitives.PushBack(city_building8);
		}
		
	//City buildings 3
	
		{
			Cube* city_building9 = new Cube({ 50, 30, 70 }, 10000);
			city_building9->color = Bright_Grey;
			city_building9->SetPos(-100, 0, -50);
			primitives.PushBack(city_building9);

			Cube* city_building10 = new Cube({ 50, 30, 50 }, 10000);
			city_building10->color = Grey;
			city_building10->SetPos(-100, 0, 40);
			primitives.PushBack(city_building10);

			Cube* city_building11 = new Cube({ 50, 50, 60 }, 10000);
			city_building11->color = Darker_Grey;
			city_building11->SetPos(-100, 0, 90);
			primitives.PushBack(city_building11);
			
		}
	
	
	//City buildings 4
	
		{
			Cube* city_building12 = new Cube({ 50, 30, 30 }, 10000);
			city_building12->color = Grey;
			city_building12->SetPos(-50, 0, 110);
			primitives.PushBack(city_building12);

			Cube* city_building13 = new Cube({ 20, 40, 30 }, 10000);
			city_building13->color = Dark_Grey;
			city_building13->SetPos( -20, 0, 110);
			primitives.PushBack(city_building13);

			Cube* city_building14 = new Cube({ 50, 50, 30 }, 10000);
			city_building14->color = Darker_Grey;
			city_building14->SetPos( 50, 0, 110);
			primitives.PushBack(city_building14);
		}
	
	
	//City buildings 5
		{
			Cube* city_building15 = new Cube({ 20, 30, 40 }, 10000);
			city_building15->color = Bright_Grey;
			city_building15->SetPos(-40, 0, -40);
			primitives.PushBack(city_building15);

			Cube* city_building16 = new Cube({ 20, 50, 60 }, 10000);
			city_building16->color = Darker_Grey;
			city_building16->SetPos(-35, 0, 40);
			primitives.PushBack(city_building16);
		}
	
	//City buildings 6
	
		{
			Cube* city_building17 = new Cube({ 20, 10, 20 }, 10000);
			city_building17->color = Grey;
			city_building17->SetPos(-20, 0, 60);
			primitives.PushBack(city_building17);

			Cube* city_building18 = new Cube({ 20, 20, 20 }, 10000);
			city_building18->color = Dark_Grey;
			city_building18->SetPos( 0, 0, 60);
			primitives.PushBack(city_building18);

			Cube* city_building19 = new Cube({ 20, 30, 20 }, 10000);
			city_building19->color = Bright_Grey;
			city_building19->SetPos(40, 0, 60);
			primitives.PushBack(city_building19);

			Cube* city_building20 = new Cube({ 25, 40, 20 }, 10000);
			city_building20->color = Darker_Grey;
			city_building20->SetPos( 60, 0, 60);
			primitives.PushBack(city_building20);
		}
		
	
	//City buildings 7
		{
			Cube* city_building21 = new Cube({ 20, 20, 20 }, 10000);
			city_building21->color = Grey;
			city_building21->SetPos(100, 0, 20);
			primitives.PushBack(city_building21);

			Cube* city_building22 = new Cube({ 20, 40, 20 }, 10000);
			city_building22->color = Darker_Grey;
			city_building22->SetPos(80, 0, 20);
			primitives.PushBack(city_building22);

			Cube* city_building23 = new Cube({ 20, 30, 20 }, 10000);
			city_building23->color = Dark_Grey;
			city_building23->SetPos(60, 0, 20);
			primitives.PushBack(city_building23);

			Cube* city_building24 = new Cube({ 25, 40, 20 }, 10000);
			city_building24->color = Bright_Grey;
			city_building24->SetPos(40, 0, 20);
			primitives.PushBack(city_building24);

			Cube* city_building25 = new Cube({ 20, 40, 20 }, 10000);
			city_building25->color = Grey;
			city_building25->SetPos(20, 0, 20);
			primitives.PushBack(city_building25);

			Cube* city_building26 = new Cube({ 20, 20, 20 }, 10000);
			city_building26->color = Darker_Grey;
			city_building26->SetPos(-20, 0, 20);
			primitives.PushBack(city_building26);
		}
		
	//City buildings 8
		{
			Cube* city_building27 = new Cube({ 20, 20, 20 }, 10000);
			city_building27->color = Grey;
			city_building27->SetPos( 0, 0, -20);
			primitives.PushBack(city_building27);

			Cube* city_building28 = new Cube({ 20, 20, 10 }, 10000);
			city_building28->color = Bright_Grey;
			city_building28->SetPos(0, 0, -50);
			primitives.PushBack(city_building28);

			Cube* city_building29 = new Cube({ 20, 20, 20 }, 10000);
			city_building29->color = Grey;
			city_building29->SetPos(50, 0, -20);
			primitives.PushBack(city_building29);

			Cube* city_building30 = new Cube({ 20, 20, 10 }, 10000);
			city_building30->color = Darker_Grey;
			city_building30->SetPos(50, 0, -50);
			primitives.PushBack(city_building30);

		}
		
}

void ModuleSceneIntro::CreatePizza()
{
	float tape_angle = 60.f;

	pizza.base = new Cube({ 2, 0.2f, 2 }, 0);
	primitives.PushBack(pizza.base);
	pizza.base->SetPos(5, 1, 0);
	pizza.base->color = Beige;
	pizza.base->body.SetAsSensor(true);
	pizza.base->body.collision_listeners.PushBack(this);

	pizza.tape = new Cube({ 2, 0.2f, 2 }, 0);
	pizza.tape->transform.rotate(tape_angle, vec3(0, 0, 1));
	primitives.PushBack(pizza.tape);
	pizza.tape->SetPos(6.5f, 1.9f, 0);
	pizza.tape->color = Beige;
	pizza.tape->body.SetAsSensor(true);
	pizza.tape->body.collision_listeners.PushBack(this);
	
	pizza.pizza = new Cylinder(0.8, 0.1f, 0);
	pizza.pizza->transform.rotate(90.f, vec3(0, 0, 1));
	primitives.PushBack(pizza.pizza);
	pizza.pizza->SetPos(pizza_position[p].x, pizza_position[p].y, pizza_position[p].z);
	pizza.pizza->color = Red;
	pizza.pizza->body.SetAsSensor(true);
	pizza.pizza->body.collision_listeners.PushBack(this);
	pizza.pizza->body.isPizza = true;

	pizza_pointer = &pizza;

}

void ModuleSceneIntro::CreateFence(int first_cylinder_position)
{	
	/*
	int k = 0;
	int SnakeLength = 24;
	float BallDistance = 0.2f;
	first_cylinder_position = 2.5;
	int x = 10;
	int y = 3;
	int z = 8.25;
	float XPos = first_cylinder_position;
	float Size = 0.25;

	Sphere* last_sphere;

	//first fence
	Cylinder* c = new Cylinder(0.5, 4, 0);
	//c->transform.rotate(90.f, vec3(0, 0, 1));
	//c->body.SetRotation()
	//primitives.PushBack(c);
	c->SetPos(3, y, z);
	c->color = Yellow;
	p2DynArray<Sphere*> spheres;

	for (int n = 0; n < SnakeLength; n++)
	{
		Sphere* s = new Sphere(Size);
		primitives.PushBack(s);
		spheres.PushBack(s);
		s->SetPos(XPos, y, 4.5f);
		s->color = Yellow;
		XPos += Size;
		if (n > 0)
		{
			App->physics->AddConstraintP2P(*last_sphere, *s, btVector3(last_sphere->GetRadius(), 0, 0), btVector3(-s->GetRadius(), 0, 0));
		}
		last_sphere = s;
		k = n;
	}
	Sphere* first_sphere = *spheres.At(0);
	App->physics->AddConstraintP2P(*c, *first_sphere, btVector3(-c->GetHeight() * 0.25f , 0 , 0), btVector3(first_sphere->GetRadius(), 0, 0));

	Cylinder* c2 = new Cylinder(0.5, 4, 0);
	//c2->transform.rotate(90.f, vec3(0, 0, 1));
	//primitives.PushBack(c2);
	c2->SetPos(-2 + - XPos, y, z);
	c2->color = Yellow;
	App->physics->AddConstraintP2P(*c2, *last_sphere, btVector3( c2->GetHeight() * 0.25f - Size,0, 0), btVector3(-last_sphere->GetRadius(), 0, 0));

	//second fence
	x = -52;
	z = 16;
	SnakeLength = 48;
	c = new Cylinder(0.5f, 4, 0);
	c->SetPos(x, y, z);
	c->color = Yellow;
	XPos = x;

	for (int n = 0; n < SnakeLength; n++)
	{
		Sphere* s = new Sphere(Size);
		primitives.PushBack(s);
		spheres.PushBack(s);
		s->SetPos(XPos, y, z);
		s->color = Yellow;
		XPos += Size;
		if (n > 0)
		{
			App->physics->AddConstraintP2P(*last_sphere, *s, btVector3(last_sphere->GetRadius(), 0, 0), btVector3(-s->GetRadius(), 0, 0));
		}
		last_sphere = s;
		k++;
	}
	first_sphere = *spheres.At(k-1);
	App->physics->AddConstraintP2P(*c, *first_sphere, btVector3(-c->GetHeight() * 0.25f + Size, 0, 0), btVector3(first_sphere->GetRadius(), 0, 0));

	c2 = new Cylinder(0.5, 4, 0);
	c2->SetPos(x - 30, y, z);
	c2->color = Yellow;
	App->physics->AddConstraintP2P(*c2, *last_sphere, btVector3(c2->GetHeight() * 0.25f - Size, 0, 0), btVector3(-last_sphere->GetRadius(), 0, 0));



	//third fence
	x = 105;
	z = 98;
	SnakeLength = 8;
	c = new Cylinder(0.5, 4, 0);
	c->SetPos(x, y, z);
	c->color = Yellow;
	XPos = x;

	for (int n = 0; n < SnakeLength; n++)
	{
		Sphere* s = new Sphere(Size);
		primitives.PushBack(s);
		spheres.PushBack(s);
		s->SetPos(XPos, y, z);
		s->color = Yellow;
		XPos += Size;
		if (n > 0)
		{
			App->physics->AddConstraintP2P(*last_sphere, *s, btVector3(last_sphere->GetRadius(), 0, 0), btVector3(-s->GetRadius(), 0, 0));
		}
		last_sphere = s;
		k++;
	}
	first_sphere = *spheres.At(k - 1);
	//App->physics->AddConstraintP2P(*c, *first_sphere, btVector3(c->GetHeight() * 0.25f + Size, 0, 0), btVector3(first_sphere->GetRadius(), 0, 0));

	c2 = new Cylinder(0.5, 4, 0);
	c2->SetPos(XPos - 32, y, z);
	c2->color = Yellow;

	//fourth fence
	x = 70;
	z = -100;
	SnakeLength = 8;
	c = new Cylinder(0.5, 4, 0);
	c->SetPos(x, y, z);
	c->color = Yellow;
	XPos = x;

	for (int n = 0; n < SnakeLength; n++)
	{
		Sphere* s = new Sphere(Size);
		primitives.PushBack(s);
		spheres.PushBack(s);
		s->SetPos(XPos, y, z);
		s->color = Yellow;
		XPos += Size;
		if (n > 0)
		{
			App->physics->AddConstraintP2P(*last_sphere, *s, btVector3(last_sphere->GetRadius(), 0, 0), btVector3(-s->GetRadius(), 0, 0));
		}
		last_sphere = s;
		k++;
	}
	first_sphere = *spheres.At(k - 1);
	//App->physics->AddConstraintP2P(*c, *first_sphere, btVector3(c->GetHeight() * 0.25f + Size, 0, 0), btVector3(first_sphere->GetRadius(), 0, 0));

	c2 = new Cylinder(0.5, 4, 0);
	c2->SetPos(XPos - 40, y, z);
	c2->color = Yellow;
	*/
}

void ModuleSceneIntro::CreateDecoration()
{
	{
		Cube* sidewalk1 = new Cube({ 38, 1, 160 }, 0);
		sidewalk1->color = Bright_Grey;
		sidewalk1->SetPos(120, 0, 55);
		primitives.PushBack(sidewalk1);
		
		Cube* sidewalk2 = new Cube({ 41, 1, 34 }, 0);
		sidewalk2->color = Bright_Grey;
		sidewalk2->SetPos(120, 0, -66);
		primitives.PushBack(sidewalk2);
	}
	{
		Cube* sidewalk3 = new Cube({ 77, 1, 55 }, 0);
		sidewalk3->color = Bright_Grey;
		sidewalk3->SetPos(102, 0, -110);
		primitives.PushBack(sidewalk3);
		
		Cube* sidewalk4 = new Cube({ 60, 1, 55 }, 0);
		sidewalk4->color = Grey;
		sidewalk4->SetPos(15, 0, -110);
		primitives.PushBack(sidewalk4);
		
		Cube* sidewalk5 = new Cube({ 95, 1, 55 }, 0);
		sidewalk5->color = Bright_Grey;
		sidewalk5->SetPos(-80, 0, -110);
		primitives.PushBack(sidewalk5);
	}
	{
		Cube* sidewalk6 = new Cube({ 55, 1, 73 }, 0);
		sidewalk6->color = Bright_Grey;
		sidewalk6->SetPos(-100, 0, -46);
		primitives.PushBack(sidewalk6);
		
		Cube* sidewalk7 = new Cube({ 55, 1, 117 }, 0);
		sidewalk7->color = Bright_Grey;
		sidewalk7->SetPos(-100, 0, 68);
		primitives.PushBack(sidewalk7);
	}
	{
		Cube* sidewalk8 = new Cube({ 72, 1, 35 }, 0);
		sidewalk8->color = Bright_Grey;
		sidewalk8->SetPos(-36.5f, 0, 110);
		primitives.PushBack(sidewalk8);

		Cube* sidewalk9 = new Cube({ 55, 1,35 }, 0);
		sidewalk9->color = Bright_Grey;
		sidewalk9->SetPos(50, 0, 110);
		primitives.PushBack(sidewalk9);
	}
	{
		Cube* sidewalk10 = new Cube({ 25, 1, 45 }, 0);
		sidewalk10->color = Bright_Grey;
		sidewalk10->SetPos(-40, 0, -40);
		primitives.PushBack(sidewalk10);
		
		Cube* sidewalk11 = new Cube({ 25, 1, 65 }, 0);
		sidewalk11->color = Bright_Grey;
		sidewalk11->SetPos(-45, 0, 40);
		primitives.PushBack(sidewalk11);		
	}
	{
		Cube* sidewalk12 = new Cube({ 45, 1, 25 }, 0);
		sidewalk12->color = Bright_Grey;
		sidewalk12->SetPos(-10, 0, 60);
		primitives.PushBack(sidewalk12);

		Cube* sidewalk13 = new Cube({ 50, 1, 25 }, 0);
		sidewalk13->color = Bright_Grey;
		sidewalk13->SetPos(52, 0, 60);
		primitives.PushBack(sidewalk13);
	}
	{
		Cube* sidewalk14 = new Cube({ 102, 1, 25 }, 0);
		sidewalk14->color = Bright_Grey;
		sidewalk14->SetPos(50, 0, 20);
		primitives.PushBack(sidewalk14);

		Cube* sidewalk15 = new Cube({ 25, 1, 25 }, 0);
		sidewalk15->color = Bright_Grey;
		sidewalk15->SetPos(-20, 0, 20);
		primitives.PushBack(sidewalk15);
	}
	{
		Cube* sidewalk16 = new Cube({ 25, 1, 25 }, 0);
		sidewalk16->color = Bright_Grey;
		sidewalk16->SetPos(0, 0, -20);
		primitives.PushBack(sidewalk16);

		Cube* sidewalk17 = new Cube({ 25, 1, 15 }, 0);
		sidewalk17->color = Bright_Grey;
		sidewalk17->SetPos(0, 0, -50);
		primitives.PushBack(sidewalk17);

		Cube* sidewalk18 = new Cube({ 25, 1, 25 }, 0);
		sidewalk18->color = Bright_Grey;
		sidewalk18->SetPos(50, 0, -20);
		primitives.PushBack(sidewalk18);

		Cube* sidewalk19 = new Cube({ 25, 1, 15 }, 0);
		sidewalk19->color = Bright_Grey;
		sidewalk19->SetPos(50, 0, -50);
		primitives.PushBack(sidewalk19);
	}
}

void ModuleSceneIntro::changePizzaPosition(int x, int y, int z)
{
	pizza_pointer->base->SetPos(x, y-0.2f, z);
	pizza_pointer->pizza->SetPos(x, y, z);
	pizza_pointer->tape->SetPos(x+1.5f, y+0.7f, z);
	p++;	
}

void ModuleSceneIntro::CreateBollards() {
	btSliderConstraint* constraint;
	Cylinder* bollard;
	bollard = new Cylinder(0.4, 4, 0);
	bollard->transform.rotate(90.f, vec3(0, 0, 1));
	primitives.PushBack(bollard);
	bollard->SetPos(0, 1, 4);
	bollard->color = Yellow;

	Cylinder* bollardBase;
	bollardBase = new Cylinder(0.8, 0.2, 0);
	bollardBase->transform.rotate(90.f, vec3(0, 0, 1));
	primitives.PushBack(bollardBase);
	bollardBase->SetPos(0, 1, 4);
	bollardBase->color = Grey;

	btVector3 sliderWorldPos(0, 1, 4);
	btVector3 sliderAxis(0, 0, 1);

	btTransform frameInA, frameInB;

	frameInA.setIdentity();
	frameInB.setIdentity();
	frameInA.setOrigin(btVector3(0, 1, 4));
	frameInB.setOrigin(btVector3(0, 1, 4));

	constraint = App->physics->AddConstraintSlider(*bollard, *bollardBase, frameInA, frameInB);
	constraint->setLowerLinLimit(3);
	constraint->setUpperLinLimit(6);
	constraint->setLowerAngLimit(3);
	constraint->setUpperAngLimit(3);
	bollard->body.Push(vec3(0, 0, 100));
}

