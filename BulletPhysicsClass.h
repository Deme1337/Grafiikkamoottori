#pragma once

#ifndef BPHYSICS_H
#define BPHYSICS_H
#include "btBulletCollisionCommon.h"
#include "btBulletDynamicsCommon.h"


class BulletPhysicsClass
{
public:
	BulletPhysicsClass();

	bool initPhysics();
	void addRigidBody(btRigidBody *rbody);
	~BulletPhysicsClass();
	void physicsSimulateStep();

private:
	std::vector<btRigidBody> rigidbodies;
};

#endif

