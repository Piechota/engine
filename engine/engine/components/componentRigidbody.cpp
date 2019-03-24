#include "../headers.h"
#include "componentRigidbody.h"

#define COMP_TYPE SComponentRigidbody
#define COMP_ID EComponentType::CT_Rigidbody

namespace ComponentRigidbodyManager
{
	struct SPhysicsComponents
	{
		UINT32 m_transformID;
		UINT32 m_rigidbodyID;
	};
	TArray< SPhysicsComponents > m_physicsComponents;
#include "componentsImpl.h"

	void RegisterPhysicsComponents(SComponentHandle const transformHandle, SComponentHandle const rigidbodyHandle)
	{
		ASSERT(transformHandle.m_type == EComponentType::CT_Transform);
		ASSERT(rigidbodyHandle.m_type == EComponentType::CT_Rigidbody);
		m_physicsComponents.Add({ transformHandle.m_index, rigidbodyHandle.m_index });
	}

	void SyncTransformsWithPhysics()
	{
		uint const physicsComponentsCount = m_physicsComponents.Size();
		for (uint i = 0; i < physicsComponentsCount; ++i)
		{
			SComponentTransform &transform = ComponentTransformManager::GetComponentNoCheck(m_physicsComponents[i].m_transformID);
			SComponentRigidbody const rigidbodyMesh = GetComponentNoCheck(m_physicsComponents[i].m_rigidbodyID);

			PhysicsManager::GetRigidbodyTransform(rigidbodyMesh.bodyPtr, transform.m_position, transform.m_rotation);
		}
	}
}

#undef COMP_TYPE 
#undef COMP_ID 