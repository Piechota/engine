#pragma once
struct SPhysicTestComponent
{
	Vec3 m_positionA;
	Vec3 m_positionB;
};
POD_TYPE(SPhysicTestComponent)

class CComponentPhysicTestManager : public TComponentContainer< SPhysicTestComponent, EComponentType::CT_PhysicsTest >
{
private:
	struct STestComponents
	{
		UINT32 m_transformID;
		UINT32 m_physicID;
		UINT32 m_staticMeshID;
		UINT32 m_physicTestID;
	};
	TArray< STestComponents > m_testComponents;

public:
	void RegisterPhysicTestComponents(SComponentHandle const transformHandle, SComponentHandle const physicHandle, SComponentHandle const staticMeshHandle, SComponentHandle const physicTestHandle)
	{
		ASSERT(transformHandle.m_type == EComponentType::CT_Transform);
		ASSERT(physicHandle.m_type == EComponentType::CT_Physics);
		ASSERT(staticMeshHandle.m_type == EComponentType::CT_StaticMesh);
		ASSERT(physicTestHandle.m_type == EComponentType::CT_PhysicsTest);
		m_testComponents.Add({ transformHandle.m_index, physicHandle.m_index, staticMeshHandle.m_index, physicTestHandle.m_index });
	}

	void PhysicTestUpdate();
};
