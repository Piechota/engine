#pragma once

template< typename T, EComponentType componentType >
class TComponentContainer
{
protected:
	TArray<T> m_components;
	TArray<UINT32> m_freeComponents;

public:
	SComponentHandle AddComponent()
	{
		SComponentHandle handle;
		handle.m_type = componentType;
		handle.m_index = m_components.Size();
		if ( m_freeComponents.Size() == 0 )
		{
			m_components.Add();
		}
		else
		{
			handle.m_index = m_freeComponents[ m_freeComponents.Size() - 1 ];
			m_freeComponents.EraseBack();
			if ( !IsPOD< T >::value )
			{
				new ( &m_components[ handle.m_index ] ) T();
			}
		}

		return handle;
	}

	void RemoveComponent( SComponentHandle const handle )
	{
		ASSERT( handle.m_type == componentType );
		ASSERT( handle.m_index < m_components.Size() );
		m_freeComponents.Add( handle.m_index );
		if ( !IsPOD< T >::value )
		{
			m_components[ handle.m_index ].~T();
		}
	}

	FORCE_INLINE T& GetComponent( SComponentHandle const handle )
	{
		ASSERT( handle.m_type == componentType );
		return m_components[ handle.m_index ];
	}
	FORCE_INLINE T const& GetComponent( SComponentHandle const handle ) const
	{
		ASSERT( handle.m_type == componentType );
		return m_components[ handle.m_index ];
	}

	FORCE_INLINE T& GetComponentNoCheck( UINT const id )
	{
		return m_components[ id ];
	}
	FORCE_INLINE T const& GetComponentNoCheck( UINT const id ) const
	{
		return m_components[ id ];
	}
};
