#pragma once
#include "Module.h"
#include "p2DynArray.h"
#include "Globals.h"
#include "Primitive.h"

class Primitive;
class PhysBody3D;

#define MAX_PIZZA_POSITIONS 10

struct Pizza {
	Cube* tape;	
	Cylinder* pizza;
	Cube* base;
};

class ModuleSceneIntro : public Module
{
public:
	ModuleSceneIntro(bool start_enabled = true);
	~ModuleSceneIntro();

	bool Start();
	update_status Update(float dt) override;
	update_status PostUpdate(float dt) override;
	bool CleanUp();

	void OnCollision(PhysBody3D* body1, PhysBody3D* body2) override;

	void CreateBuildings();
	void CreatePizza();
	void CreateFence(int first_cylinder_position);
	void CreateDecoration();
	void CreateBollards();
	void CreateSingleBollard(float x, float z);

	void changePizzaPosition(int x, int y, int z);

	p2DynArray<Primitive*> GetPrimitivesList() const { return primitives; };
	p2List<Cylinder*> bollards;
	//Check points
	void addCheckPoint(vec3 pos);
	//p2DynArray<Cube> checkpointList; //TODO: this crashes
private:
	void HandleDebugInput();
	void DebugSpawnPrimitive(Primitive* p);
	p2DynArray<Primitive*> primitives;
public:
	int p = 0;
	vec3 pizza_position[MAX_PIZZA_POSITIONS];
	Pizza pizza;
	unsigned int start = 0;

};
