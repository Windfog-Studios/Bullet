#pragma once
#include "Module.h"
#include "p2DynArray.h"
#include "Globals.h"

class Primitive;
class  PhysBody3D;

class ModuleSceneIntro : public Module
{
public:
	ModuleSceneIntro(bool start_enabled = true);
	~ModuleSceneIntro();

	bool Start();
	update_status Update(float dt) override;
	update_status PostUpdate(float dt) override;

	void OnCollision(PhysBody3D* body1, PhysBody3D* body2) override;

	bool CleanUp();

	//Check points
	void addCheckPoint(int pos_x, int pos_y, int pos_z);
	p2DynArray<PhysBody3D> checkpointList;
private:
	void HandleDebugInput();
	void DebugSpawnPrimitive(Primitive* p);

	p2DynArray<Primitive*> primitives;
};
