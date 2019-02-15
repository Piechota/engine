#pragma once

#include "physicsObjects.h"

namespace PhysicsUtilities
{
	void GenerateConvexFromVertices( TArray<Vec3> const& vertices );

	bool SphereSphereCollision(SSphere const& sphereA, SSphere const& sphereB, SHitInfo &outHitInfo);
	bool BoxSphereCollision(SBox const& boxA, SSphere const& sphereB, SHitInfo &outHitInfo);
	bool BoxBoxCollision( SBox const& boxA, SBox const& boxB, SHitInfo &outHitInfo );
};
