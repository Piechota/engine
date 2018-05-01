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

SComponentHandle CEntity::AddComponent( EComponentType const type )
{
	switch ( type )
	{
		case EComponentType::CT_Transform:
			return GComponentTransformManager.AddComponent();
		case EComponentType::CT_StaticMesh:
			return GComponentStaticMeshManager.AddComponent();
		case EComponentType::CT_Light:
			return GComponentLightManager.AddComponent();
		case EComponentType::CT_Camera:
			return GComponentCameraManager.AddComponent();
		default:
			ASSERT_STR( false, "Missing component type" );
			return SComponentHandle{ 0, 0 };
	}
}

SComponentHandle CEntity::GetComponent( EComponentType const type ) const
{
	UINT const componentsNum = m_components.Size();
	for ( UINT i = 0; i < componentsNum; ++i )
	{
		SComponentHandle const handle = m_components[ i ];
		if ( handle.m_type == type )
		{
			return handle;
		}
	}

	return { 0, EComponentType::CT_INVALID };
}