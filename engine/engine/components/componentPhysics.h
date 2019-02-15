#pragma once
enum EColliderType
{
	CT_Sphere,
	CT_Box,
};

struct SComponentPhysics
{
	union
	{
		struct
		{
			float m_radius;
		};
		struct
		{
			Vec3 m_boxSides;
		};
	};

	EColliderType m_colliderType;
	bool m_collided;
};
POD_TYPE( SComponentPhysics )

class CComponentPhysicsManager : public TComponentContainer< SComponentPhysics, EComponentType::CT_Physics>
{
private:
	struct SPhysicsComponents
	{
		UINT32 m_transformID;
		UINT32 m_physicsID;
	};
	TArray< SPhysicsComponents > m_physicsComponents;

public:
	void RegisterPhysicsComponents(SComponentHandle const transformHandle, SComponentHandle const physicHandle)
	{
		ASSERT(transformHandle.m_type == EComponentType::CT_Transform);
		ASSERT(physicHandle.m_type == EComponentType::CT_Physics);
		m_physicsComponents.Add({ transformHandle.m_index, physicHandle.m_index });
	}

	void PhysicTick();
};
