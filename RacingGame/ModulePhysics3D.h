#pragma once
#include "Module.h"
#include "Globals.h"
#include "p2List.h"
#include "Primitive.h"
#include "PhysVehicle3D.h"

#include "Bullet/include/btBulletDynamicsCommon.h"

// Recommended scale is 1.0f == 1 meter, no less than 0.2 objects
#define GRAVITY btVector3(0.0f, -10.0f, 0.0f) 

class DebugDrawer;
class  PhysBody3D;

class ModulePhysics3D : public Module
{
public:
	ModulePhysics3D(bool start_enabled = true);
	~ModulePhysics3D();

	bool Init();
	bool Start();
	update_status PreUpdate(float dt) override;
	update_status Update(float dt) override;
	update_status PostUpdate(float dt) override;
	bool CleanUp();

	void AddBodyToWorld(btRigidBody* body);
	void RemoveBodyFromWorld(btRigidBody* body);
	PhysVehicle3D* AddVehicle(const VehicleInfo& info);

	PhysBody3D* RayCast(const vec3& Origin, const vec3& Direction, vec3& HitPoint = vec3());

	btPoint2PointConstraint* AddConstraintP2P(const Primitive& bodyA, const Primitive& bodyB, const btVector3& pivotInA, const btVector3& pivotInB);
	btHingeConstraint* AddConstraintHinge(const Primitive & bodyA, const Primitive & bodyB, const btVector3& pivotInA, const btVector3& pivotInB, btVector3& axisInA, btVector3& axisInB);
	btSliderConstraint* AddConstraintSlider(const Primitive& bodyA, const Primitive& bodyB, btTransform& frameinA, btTransform& frameinB);
	btGeneric6DofConstraint* AddGeneric6DofConstraint(const Primitive& bodyA, const Primitive& bodyB, const btTransform& frameInA, const btTransform& frameInB, bool useLinearReferenceFrameA = true);


private:
	btDefaultCollisionConfiguration*		collision_conf;
	btCollisionDispatcher*					dispatcher;
	btBroadphaseInterface*					broad_phase;
	btSequentialImpulseConstraintSolver*	solver;
	btDiscreteDynamicsWorld*				world;
	DebugDrawer*							debug_draw;
	btDefaultVehicleRaycaster*				vehicle_raycaster;

	p2List<btTypedConstraint*>				constraints;
	p2List<btCollisionShape*>				shapes;
	p2List<PhysBody3D*>						bodies;
	p2List<btDefaultMotionState*>			motions;
	p2List<btSliderConstraint*>				sliderConstraints;
};

class DebugDrawer : public btIDebugDraw
{
public:
	DebugDrawer() : line()
	{}

	void drawLine(const btVector3& from, const btVector3& to, const btVector3& color);
	void drawContactPoint(const btVector3& PointOnB, const btVector3& normalOnB, btScalar distance, int lifeTime, const btVector3& color);
	void reportErrorWarning(const char* warningString);
	void draw3dText(const btVector3& location, const char* textString);
	void setDebugMode(int debugMode);
	int	 getDebugMode() const;

	DebugDrawModes mode;
	Line line;
	Primitive point;
};