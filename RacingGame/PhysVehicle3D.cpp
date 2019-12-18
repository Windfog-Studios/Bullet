#include "PhysVehicle3D.h"
#include "Primitive.h"
#include "Bullet/include/btBulletDynamicsCommon.h"
#include "p2Log.h"
#include "Application.h"

// ----------------------------------------------------------------------------
VehicleInfo::~VehicleInfo()
{
	//if(wheels != NULL)
		//delete wheels;
}

// ----------------------------------------------------------------------------
PhysVehicle3D::PhysVehicle3D(btRigidBody* body, btRaycastVehicle* vehicle, const VehicleInfo& info) : PhysBody3D(body), vehicle(vehicle), info(info)
{
}

// ----------------------------------------------------------------------------
PhysVehicle3D::~PhysVehicle3D()
{
	delete vehicle;
}

// ----------------------------------------------------------------------------
void PhysVehicle3D::Render()
{
	Cylinder wheel;

	wheel.color = Dark_Green;

	for(int i = 0; i < vehicle->getNumWheels(); ++i)
	{
		wheel.SetRadius(info.wheels[i].radius);
		wheel.SetHeight(info.wheels[i].width);

		vehicle->updateWheelTransform(i);
		vehicle->getWheelInfo(i).m_worldTransform.getOpenGLMatrix(&wheel.transform);

		wheel.Render();
	}

	Cube chassis(info.chassis_size);
	vehicle->getChassisWorldTransform().getOpenGLMatrix(&chassis.transform);
	btQuaternion q = vehicle->getChassisWorldTransform().getRotation();
	btVector3 offset(info.chassis_offset.x, info.chassis_offset.y, info.chassis_offset.z);
	offset = offset.rotate(q.getAxis(), q.getAngle());
	position.x = chassis.transform.M[12] += offset.getX();
	position.y = chassis.transform.M[13] += offset.getY();
	position.z = chassis.transform.M[14] += offset.getZ();
	chassis.color = Black;

	Cube cabin(info.cabin_size);
	vehicle->getChassisWorldTransform().getOpenGLMatrix(&cabin.transform);
	btQuaternion qcb = vehicle->getChassisWorldTransform().getRotation();
	btVector3 cb_offset(info.cabin_offset.x, info.cabin_offset.y, info.cabin_offset.z);
	cb_offset = cb_offset.rotate(qcb.getAxis(), qcb.getAngle());
	cabin.transform.M[12] += cb_offset.getX();
	cabin.transform.M[13] += cb_offset.getY();
	cabin.transform.M[14] += cb_offset.getZ();
	cabin.color = Beige;

	Cube trunk(info.trunk_size);
	vehicle->getChassisWorldTransform().getOpenGLMatrix(&trunk.transform);
	btQuaternion qtr = vehicle->getChassisWorldTransform().getRotation();
	btVector3 tr_offset(info.trunk_offset.x, info.trunk_offset.y, info.trunk_offset.z);
	tr_offset = tr_offset.rotate(qtr.getAxis(), qtr.getAngle());
	trunk.transform.M[12] += tr_offset.getX();
	trunk.transform.M[13] += tr_offset.getY();
	trunk.transform.M[14] += tr_offset.getZ();
	trunk.color = Dark_Red;

	//chassis.Render();
	trunk.Render();
	cabin.Render();
}

// ----------------------------------------------------------------------------
void PhysVehicle3D::ApplyEngineForce(float force)
{
	for(int i = 0; i < vehicle->getNumWheels(); ++i)
	{
		if(info.wheels[i].drive == true)
		{
			vehicle->applyEngineForce(force, i);
		}
	}
}

// ----------------------------------------------------------------------------
void PhysVehicle3D::Brake(float force)
{
	for(int i = 0; i < vehicle->getNumWheels(); ++i)
	{
		if(info.wheels[i].brake == true)
		{
			vehicle->setBrake(force, i);
		}
	}
}

// ----------------------------------------------------------------------------
void PhysVehicle3D::Turn(float degrees)
{
	for(int i = 0; i < vehicle->getNumWheels(); ++i)
	{
		if(info.wheels[i].steering == true)
		{
			vehicle->setSteeringValue(degrees, i);
		}
	}
}

// ----------------------------------------------------------------------------
float PhysVehicle3D::GetKmh() const
{
	return vehicle->getCurrentSpeedKmHour();
}

vec3 PhysVehicle3D::GetForwardVector() const {
	return vec3(vehicle->getForwardVector().x(), vehicle->getForwardVector().y(), vehicle->getForwardVector().z());
}