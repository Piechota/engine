#include "..\core.h"
#include "physicsManager.h"
#include "..\bullet\btBulletDynamicsCommon.h"
#include "physicsHelpers.h"
namespace PhysicsManager
{
	///collision configuration contains default setup for memory, collision setup. Advanced users can create their own configuration.
	btDefaultCollisionConfiguration *collisionConfiguration;

	///use the default collision dispatcher. For parallel processing you can use a diffent dispatcher (see Extras/BulletMultiThreaded)
	btCollisionDispatcher *dispatcher;

	///btDbvtBroadphase is a good general purpose broadphase. You can also try out btAxis3Sweep.
	btBroadphaseInterface *overlappingPairCache;

	///the default constraint solver. For parallel processing you can use a different solver (see Extras/BulletMultiThreaded)
	btSequentialImpulseConstraintSolver *solver;

	btDiscreteDynamicsWorld *dynamicsWorld;

	TArray< btCollisionShape* > collisionShapes;

	void Init()
	{
		collisionConfiguration = new btDefaultCollisionConfiguration();
		dispatcher = new btCollisionDispatcher(collisionConfiguration);
		overlappingPairCache = new btDbvtBroadphase();
		solver = new btSequentialImpulseConstraintSolver;
		dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, overlappingPairCache, solver, collisionConfiguration);
		dynamicsWorld->setGravity(btVector3(0, -10, 0));
	}
	void Tick()
	{
		dynamicsWorld->stepSimulation(1.f / 60.f, 10);
	}
	void Release()
	{
		uint const collisionShapesCount = collisionShapes.Size();
		for (uint i = 0; i < collisionShapesCount; ++i)
		{
			delete collisionShapes[i];
		}

		delete dynamicsWorld;
		delete solver;
		delete overlappingPairCache;
		delete dispatcher;
		delete collisionConfiguration;
	}

	void *AddRigidbody(PhysicsObjectDef const& objectDef)
	{
		btCollisionShape* shape = nullptr;
		
		switch (objectDef.bodyDef.bodyType)
		{
			case EPhysicsBodyType::PBT_Box:
				shape = new btBoxShape(Vec3TobtVector3(objectDef.bodyDef.boxHalfExtents));
				break;
			case EPhysicsBodyType::PBT_Sphere:
				shape = new btSphereShape(objectDef.bodyDef.sphereRadius);
				break;
			default:
				ASSERT_STR(false, "Uknown body type");
				return nullptr;
		}

		collisionShapes.Add(shape);

		btTransform objectTransform;
		objectTransform.setIdentity();
		objectTransform.setOrigin(Vec3TobtVector3(objectDef.position));
		objectTransform.setRotation(QuaternionTobtQuaternion(objectDef.rotation));

		//rigidbody is dynamic if and only if mass is non zero, otherwise static
		bool isDynamic = (objectDef.mass != 0.f);

		btVector3 localInertia(0, 0, 0);
		if (isDynamic)
			shape->calculateLocalInertia(objectDef.mass, localInertia);

		//using motionstate is optional, it provides interpolation capabilities, and only synchronizes 'active' objects
		btDefaultMotionState* myMotionState = new btDefaultMotionState(objectTransform);
		btRigidBody::btRigidBodyConstructionInfo rbInfo(objectDef.mass, myMotionState, shape, localInertia);
		btRigidBody* body = new btRigidBody(rbInfo);

		//add the body to the dynamics world
		dynamicsWorld->addRigidBody(body);

		return body;
	}

	void GetRigidbodyTransform(void *bodyPtr, Vec3 &outPosition, Quaternion &outRotation)
	{
		btRigidBody* body = reinterpret_cast< btRigidBody*>(bodyPtr);
		btTransform trans;
		if (body && body->getMotionState())
		{
			body->getMotionState()->getWorldTransform(trans);
		}
		else
		{
			ASSERT(false);
		}
		outPosition = btVector3ToVec3(trans.getOrigin());
		outRotation = btQuaternionToQuaternion(trans.getRotation());
	}
}