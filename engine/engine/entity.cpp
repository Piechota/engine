#include "headers.h"

void CEntity::Destroy()
{
	UINT const componentsNum = m_components.Size();
	for ( UINT i = 0; i < componentsNum; ++i )
	{
		SComponentHandle const handle = m_components[ i ];
		switch ( handle.m_type )
		{
			case EComponentType::CT_Transform:
				GComponentTransformManager.RemoveComponent( handle );
				break;
			case EComponentType::CT_StaticMesh:
				GComponentStaticMeshManager.RemoveComponent( handle );
				break;
			case EComponentType::CT_Light:
				GComponentLightManager.RemoveComponent( handle );
				break;
			case EComponentType::CT_Camera:
				GComponentCameraManager.RemoveComponent( handle );
				break;
			default:
				ASSERT_STR( false, "Missing component type" );
				break;
		}
	}
}

SComponentHandle CEntity::AddComponentTransform()
{
	SComponentHandle const handle = GComponentTransformManager.AddComponent();
	m_components.Add( handle );
	return handle;
}

SComponentHandle CEntity::GetComponentTransform() const
{
	UINT const componentsNum = m_components.Size();
	for ( UINT i = 0; i < componentsNum; ++i )
	{
		SComponentHandle const handle = m_components[ i ];
		if ( handle.m_type == EComponentType::CT_Transform )
		{
			return handle;
		}
	}

	return { 0, EComponentType::CT_INVALID };
}

SComponentHandle CEntity::AddComponentStaticMesh()
{
	SComponentHandle const handle = GComponentStaticMeshManager.AddComponent();
	m_components.Add( handle );
	return handle;
}

SComponentHandle CEntity::GetComponentStaticMesh() const
{
	UINT const componentsNum = m_components.Size();
	for ( UINT i = 0; i < componentsNum; ++i )
	{
		SComponentHandle const handle = m_components[ i ];
		if ( handle.m_type == EComponentType::CT_StaticMesh )
		{
			return handle;
		}
	}

	return { 0, EComponentType::CT_INVALID };
}

SComponentHandle CEntity::AddComponentLight()
{
	SComponentHandle const handle = GComponentLightManager.AddComponent();
	m_components.Add( handle );
	return handle;
}

SComponentHandle CEntity::GetComponentLight() const
{
	UINT const componentsNum = m_components.Size();
	for ( UINT i = 0; i < componentsNum; ++i )
	{
		SComponentHandle const handle = m_components[ i ];
		if ( handle.m_type == EComponentType::CT_Light )
		{
			return handle;
		}
	}

	return { 0, EComponentType::CT_INVALID };
}

SComponentHandle CEntity::AddComponentCamera()
{
	SComponentHandle const handle = GComponentCameraManager.AddComponent();
	m_components.Add( handle );
	return handle;
}

SComponentHandle CEntity::GetComponentCamera() const
{
	UINT const componentsNum = m_components.Size();
	for ( UINT i = 0; i < componentsNum; ++i )
	{
		SComponentHandle const handle = m_components[ i ];
		if ( handle.m_type == EComponentType::CT_Camera )
		{
			return handle;
		}
	}

	return { 0, EComponentType::CT_INVALID };
}