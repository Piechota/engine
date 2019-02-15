#include "../headers.h"
#include "../physics/physicsUtilities.h"

void CComponentPhysicsManager::PhysicTick()
{
	UINT const componentsNum = m_physicsComponents.Size();

	for (UINT i = 0; i < componentsNum; ++i)
	{
		GetComponentNoCheck(m_physicsComponents[i].m_physicsID).m_collided = false;
	}

	for (UINT i = 0; i < componentsNum; ++i)
	{
		SComponentPhysics &componentPhysicA = GetComponentNoCheck(m_physicsComponents[i].m_physicsID);
		SComponentTransform const &componentTransformA = GComponentTransformManager.GetComponentNoCheck(m_physicsComponents[i].m_transformID);
		SHitInfo hitInfo;

		if (componentPhysicA.m_colliderType == CT_Sphere)
		{
			SSphere const sphereA = { componentTransformA.m_position, componentPhysicA.m_radius };

			for (UINT j = i + 1; j < componentsNum; ++j)
			{
				SComponentPhysics &componentPhysicB = GetComponentNoCheck(m_physicsComponents[j].m_physicsID);
				SComponentTransform const &componentTransformB = GComponentTransformManager.GetComponentNoCheck(m_physicsComponents[j].m_transformID);

				bool collided = false;

				if (componentPhysicB.m_colliderType == CT_Sphere)
				{
					SSphere const sphereB = { componentTransformB.m_position, componentPhysicB.m_radius };
					collided = PhysicsUtilities::SphereSphereCollision(sphereA, sphereB, hitInfo);
				}
				else if (componentPhysicB.m_colliderType == CT_Box)
				{
					SBox const boxB = { componentTransformB.m_rotation, componentTransformB.m_position, componentPhysicB.m_boxSides };
					collided = PhysicsUtilities::BoxSphereCollision(boxB, sphereA, hitInfo);
				}

				componentPhysicA.m_collided |= collided;
				componentPhysicB.m_collided |= collided;
			}
		}
		else if (componentPhysicA.m_colliderType == CT_Box)
		{
			SBox const boxA = { componentTransformA.m_rotation, componentTransformA.m_position, componentPhysicA.m_boxSides };

			for (UINT j = i + 1; j < componentsNum; ++j)
			{
				SComponentPhysics &componentPhysicB = GetComponentNoCheck(m_physicsComponents[j].m_physicsID);
				SComponentTransform const &componentTransformB = GComponentTransformManager.GetComponentNoCheck(m_physicsComponents[j].m_transformID);

				bool collided = false;

				if (componentPhysicB.m_colliderType == CT_Sphere)
				{
					SSphere const sphereB = { componentTransformB.m_position, componentPhysicB.m_radius };
					collided = PhysicsUtilities::BoxSphereCollision(boxA, sphereB, hitInfo);
				}
				else if (componentPhysicB.m_colliderType == CT_Box)
				{
					SBox const boxB = { componentTransformB.m_rotation, componentTransformB.m_position, componentPhysicB.m_boxSides };
					collided = PhysicsUtilities::BoxBoxCollision(boxA, boxB, hitInfo);
				}

				componentPhysicA.m_collided |= collided;
				componentPhysicB.m_collided |= collided;
			}
		}
	}
}