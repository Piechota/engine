#include "../headers.h"
#include "../timer.h"

void CComponentPhysicTestManager::PhysicTestUpdate()
{
	float constexpr testTime = 20.f;
	float const time = GTimer.GetSeconds(GTimer.TimeFromStart());

	float tmp;
	float const f = modff(time / testTime, &tmp);
	float const t = fabsf(f * 2.f - 1.f);

	UINT const componentsCount = m_testComponents.Size();
	for (UINT i = 0; i < componentsCount; ++i)
	{
		SPhysicTestComponent const &testComponent = GetComponentNoCheck(m_testComponents[i].m_physicTestID);
		SComponentTransform &transformComponent = GComponentTransformManager.GetComponentNoCheck(m_testComponents[i].m_transformID);

		SComponentPhysics const &physicsComponent = GComponentPhysicsManager.GetComponentNoCheck(m_testComponents[i].m_physicID);
		SComponentStaticMesh &staticMeshComponent = GComponentStaticMeshManager.GetComponentNoCheck(m_testComponents[i].m_staticMeshID);
		staticMeshComponent.m_color = physicsComponent.m_collided ? Vec3(1.f, 0.4f, 0.4f) : Vec3(.4f, 1.f, 0.4f);

		Vec3 const newPosition = testComponent.m_positionA * (1.f - t) + testComponent.m_positionB * t;
		transformComponent.m_position = newPosition;
	}
}