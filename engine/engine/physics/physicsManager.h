#pragma once
#ifdef _DEBUG
#pragma comment(lib, "../lib/bullet/BulletCollision_Debug.lib")
#pragma comment(lib, "../lib/bullet/BulletDynamics_Debug.lib")
#pragma comment(lib, "../lib/bullet/LinearMath_Debug.lib")
#else
#pragma comment(lib, "../lib/bullet/BulletCollision_MinsizeRel.lib")
#pragma comment(lib, "../lib/bullet/BulletDynamics_MinsizeRel.lib")
#pragma comment(lib, "../lib/bullet/LinearMath_MinsizeRel.lib")
#endif

namespace PhysicsManager
{
	enum EPhysicsBodyType
	{
		PBT_Box,
		PBT_Sphere,
	};

	struct PhysicsBodyDef
	{
		PhysicsBodyDef() {}

		union
		{
			Vec3 boxHalfExtents;
			float sphereRadius;
		};
		EPhysicsBodyType bodyType;
	};
	
	struct PhysicsObjectDef
	{
		PhysicsObjectDef() {}

		Quaternion rotation;
		Vec3 position;
		float mass;

		PhysicsBodyDef bodyDef;
	};

	extern void Init();
	extern void Tick();
	extern void Release();

	extern void *AddRigidbody(PhysicsObjectDef const& objectDef );
	extern void GetRigidbodyTransform( void *bodyPtr, Vec3 &outPosition, Quaternion &outRotation);
}