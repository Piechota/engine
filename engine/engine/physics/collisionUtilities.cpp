#include "../headers.h"
#include "physicsUtilities.h"

//Separate axis theorem
//3D needs each normal face and cross(edgeA, edgeB) as axises (parallel axes can be skipped)
static bool ConvexConvexCollision( TArray<Vec3> const &convexA, TArray<Vec3> const &convexB, TArray<Vec3> const &normals )
{
	uint const normalsCount = normals.Size();
	for (uint i = 0; i < normalsCount; ++i)
	{
		Vec3 const normal = normals[i];

		float minADist = FLT_MAX;
		float maxADist = -FLT_MAX;
		uint const convexACount = convexA.Size();
		for (uint a = 0; a < convexACount; ++a)
		{
			float const dist = Vec3::Dot(normal, convexA[a]);
			minADist = min(dist, minADist);
			maxADist = max(dist, maxADist);
		}

		float minBDist = FLT_MAX;
		float maxBDist = -FLT_MAX;
		uint const convexBCount = convexB.Size();
		for (uint b = 0; b < convexBCount; ++b)
		{
			float const dist = Vec3::Dot(normal, convexB[b]);
			minBDist = min(dist, minBDist);
			maxBDist = max(dist, maxBDist);
		}

		if (minADist > maxBDist || maxADist < minBDist)
		{
			return false;
		}
	}

	return true;
}

bool PhysicsUtilities::SphereSphereCollision( SSphere const& sphereA, SSphere const& sphereB, SHitInfo &outHitInfo)
{
	Vec3 const d = sphereA.m_position - sphereB.m_position;
	float const magSq = d.GetMagnitudeSq();
	float radiusSq = sphereA.m_radius + sphereB.m_radius;
	radiusSq *= radiusSq;

	outHitInfo.m_contactPoint = sphereB.m_position + (d / sqrt(magSq)) * sphereB.m_radius;

	return magSq <= radiusSq;
}

bool PhysicsUtilities::BoxSphereCollision(SBox const& boxA, SSphere const& sphereB, SHitInfo &outHitInfo)
{
	Matrix4x4 const boxMatrix = Matrix4x4::GetTranslateRotationSize(boxA.m_position, boxA.m_rotation, Vec3::ONE);
	Matrix4x4 invBoxMatrix;
	boxMatrix.Inverse(invBoxMatrix);

	Vec3 const spherePos = Math::MulPositionOrtho(sphereB.m_position, invBoxMatrix);

	Vec3 d = Vec3::Abs(spherePos) - boxA.m_sides;
	d.x = max(d.x, 0.f);
	d.y = max(d.y, 0.f);
	d.z = max(d.z, 0.f);

	float const distSq = d.GetMagnitudeSq();

	Vec3 contantPointBoxSpace = spherePos + (d / sqrt(distSq)) * sphereB.m_radius; //TODO: Scale!!
	outHitInfo.m_contactPoint = Math::MulPositionOrtho(contantPointBoxSpace, boxMatrix);

	return distSq <= sphereB.m_radius * sphereB.m_radius;
}


bool PhysicsUtilities::BoxBoxCollision(SBox const& boxA, SBox const& boxB, SHitInfo &outHitInfo)
{
	Matrix4x4 const boxAMatrix = Matrix4x4::GetTranslateRotationSize(boxA.m_position, boxA.m_rotation, Vec3::ONE);
	Matrix4x4 const boxBMatrix = Matrix4x4::GetTranslateRotationSize(boxB.m_position, boxB.m_rotation, Vec3::ONE);

	TArray<Vec3> verticesA(8);
	TArray<Vec3> verticesB(8);
	TArray<Vec3> normals(15);

	verticesA.Resize(8);
	verticesB.Resize(8);
	normals.Resize(15);

	verticesA[0].Set(+boxA.m_sides.x, boxA.m_sides.y, +boxA.m_sides.z);
	verticesA[1].Set(+boxA.m_sides.x, boxA.m_sides.y, -boxA.m_sides.z);
	verticesA[2].Set(-boxA.m_sides.x, boxA.m_sides.y, -boxA.m_sides.z);
	verticesA[3].Set(-boxA.m_sides.x, boxA.m_sides.y, +boxA.m_sides.z);

	verticesA[4].Set(+boxA.m_sides.x, -boxA.m_sides.y, +boxA.m_sides.z);
	verticesA[5].Set(+boxA.m_sides.x, -boxA.m_sides.y, -boxA.m_sides.z);
	verticesA[6].Set(-boxA.m_sides.x, -boxA.m_sides.y, -boxA.m_sides.z);
	verticesA[7].Set(-boxA.m_sides.x, -boxA.m_sides.y, +boxA.m_sides.z);

	verticesB[0].Set(+boxB.m_sides.x, boxB.m_sides.y, +boxB.m_sides.z);
	verticesB[1].Set(+boxB.m_sides.x, boxB.m_sides.y, -boxB.m_sides.z);
	verticesB[2].Set(-boxB.m_sides.x, boxB.m_sides.y, -boxB.m_sides.z);
	verticesB[3].Set(-boxB.m_sides.x, boxB.m_sides.y, +boxB.m_sides.z);
						 
	verticesB[4].Set(+boxB.m_sides.x, -boxB.m_sides.y, +boxB.m_sides.z);
	verticesB[5].Set(+boxB.m_sides.x, -boxB.m_sides.y, -boxB.m_sides.z);
	verticesB[6].Set(-boxB.m_sides.x, -boxB.m_sides.y, -boxB.m_sides.z);
	verticesB[7].Set(-boxB.m_sides.x, -boxB.m_sides.y, +boxB.m_sides.z);

	for (uint i = 0; i < 8; ++i)
	{
		verticesA[i] = Math::MulPositionOrtho(verticesA[i], boxAMatrix);
		verticesB[i] = Math::MulPositionOrtho(verticesB[i], boxBMatrix);
	}

	Vec3 edgesA[3];
	Vec3 edgesB[3];

	edgesA[0] = verticesA[3] - verticesA[0];
	edgesA[1] = verticesA[1] - verticesA[0];
	edgesA[2] = verticesA[4] - verticesA[0];

	edgesB[0] = verticesB[3] - verticesB[0];
	edgesB[1] = verticesB[1] - verticesB[0];
	edgesB[2] = verticesB[4] - verticesB[0];

	for (uint a = 0; a < 3; ++a)
	{
		for (uint b = 0; b < 3; ++b)
		{
			normals[a * 3 + b] = Vec3::Cross(edgesA[a], edgesB[b]);
		}
	}

	normals[9]  = Vec3::Cross(edgesA[0], edgesA[1]);
	normals[10] = Vec3::Cross(edgesA[1], edgesA[2]);
	normals[11] = Vec3::Cross(edgesA[0], edgesA[2]);
			    
	normals[12] = Vec3::Cross(edgesB[0], edgesB[1]);
	normals[13] = Vec3::Cross(edgesB[1], edgesB[2]);
	normals[14] = Vec3::Cross(edgesB[0], edgesB[2]);

	return ConvexConvexCollision(verticesA, verticesB, normals);
}

