#pragma once
#include "Module.h"
#include "p2DynArray.h"
#include "Globals.h"
#include "Primitive.h"

class Primitive;
class PhysBody3D;

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
	void CreateFence();

	p2DynArray<Primitive*> GetPrimitivesList() const { return primitives; };
	//Check points
	void addCheckPoint(vec3 pos);
	//p2DynArray<Cube> checkpointList; //TODO: this crashes
private:
	void HandleDebugInput();
	void DebugSpawnPrimitive(Primitive* p);
	p2DynArray<Primitive*> primitives;
};
