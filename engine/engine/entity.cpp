#include "headers.h"

AddComponentFunc AddComponentFuncPtr[CT_Num];
RemoveComponentFunc RemoveComponentFuncPtr[CT_Num];

void CEntity::Destroy()
{
	UINT const componentsNum = m_components.Size();
	for ( UINT i = 0; i < componentsNum; ++i )
	{
		SComponentHandle const handle = m_components[ i ];

		if (handle.m_type >= CT_Num || handle.m_type == CT_INVALID)
		{
			ASSERT_STR(false, "Missing component type");
			break;
		}

		ASSERT_STR(RemoveComponentFuncPtr[handle.m_type], "RemoveComponent not set");
		RemoveComponentFuncPtr[handle.m_type](handle);
	}
}

SComponentHandle CEntity::AddComponent( EComponentType const type )
{
	if (type >= CT_Num || type == CT_INVALID)
	{
		ASSERT_STR(false, "Missing component type");
		return SComponentHandle{ 0, 0 };
	}

	ASSERT_STR(AddComponentFuncPtr[type], "AddComponent not set");
	return AddComponentFuncPtr[type]();
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