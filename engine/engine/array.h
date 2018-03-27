#pragma once

template <typename T>
struct IsPOD
{
	enum
	{
		value = false
	};
	static void Create( T* pointer )
	{
		new ( pointer ) T();
	}
	static void Create( T* pointer, T const* oldData )
	{
		new ( pointer ) T( *oldData );
	}
	static void Create( T* pointer, T const* oldData, UINT const num )
	{
		for ( UINT i = 0; i < num; ++i )
		{
			new ( &pointer[i] ) T( oldData[i] );
		}
	}
	static void Create( T* pointer, UINT const num )
	{
		for ( UINT i = 0; i < num; ++i )
		{
			new ( &pointer[i] ) T();
		}
	}
	static void Create( T* pointer, UINT start, UINT const num )
	{
		for ( ; start < num; ++start )
		{
			new ( &pointer[start] ) T();
		}
	}
	static void Destroy(T* pointer)
	{
		pointer->~T();
	}

	static void Destroy(T* pointer, UINT const num )
	{
		for ( UINT i = 0; i < num; ++i )
		{
			pointer[i].~T();
		}
	}
	static void Destroy(T* pointer, UINT start, UINT const num)
	{
		for ( ; start < num; ++start )
		{
			pointer[start].~T();
		}
	}
};
template <typename T>
struct IsPOD<T*>
{
	enum
	{
		value = true
	};
	static void Create( T** pointer )
	{
	}
	static void Create( T** pointer, T* const* oldData )
	{
		memcpy( pointer, oldData, sizeof( T* ) );
	}
	static void Create( T** pointer, T* const* oldData, UINT const num )
	{
		memcpy( pointer, oldData, num * sizeof( T* ) );
	}
	static void Create( T** pointer, UINT start, UINT const num )
	{
	}
	static void Create( T** pointer, UINT const num )
	{
	}
	static void Destroy(T** pointer)
	{
	}
	static void Destroy(T** pointer, UINT const num )
	{
	}
	static void Destroy(T** pointer, UINT start, UINT const num)
	{
	}
};

#define POD_TYPE( T )													\
template<> struct IsPOD< T >											\
{																		\
enum { value = true };													\
static void Create( T* pointer ){}										\
static void Create( T* pointer, T const* oldData )						\
{																		\
	memcpy( pointer, oldData, sizeof( T ) );							\
}																		\
static void Create( T* pointer, T const* oldData, UINT const num )		\
{																		\
	memcpy( pointer, oldData, num * sizeof( T ) );						\
}																		\
static void Create( T* pointer, UINT const num ){}						\
static void Create( T* pointer, UINT start, UINT const num ){}			\
static void Destroy(T* pointer){}										\
static void Destroy(T* pointer, UINT const num ){}						\
static void Destroy(T* pointer, UINT start, UINT const num){}			\
};																		\

template <typename T >
class TArray
{
private:
	T* m_data;
	UINT m_size;
	UINT m_allocSize;

private:
	void Reallocate( UINT const size )
	{
		if ( size != m_allocSize )
		{
			T* oldData = m_data;
			m_data = nullptr;

			if ( 0 < size )
			{
				m_data = (T*)malloc( size * sizeof( T ) );
				UINT const copySize = size < m_size ? size : m_size;
				IsPOD< T >::Create( m_data, oldData, copySize );
			}

			IsPOD< T >::Destroy( oldData, m_size );
			free( oldData );

			m_allocSize = size;
			m_size = m_allocSize < m_size ? m_allocSize : m_size;
		}
	}

public:
	TArray()
		: m_data( nullptr )
		, m_allocSize( 0 )
		, m_size( 0 )
	{}

	TArray( UINT const allocSize )
		: m_data( nullptr )
		, m_allocSize( 0 )
		, m_size( 0 )
	{
		Reallocate( allocSize );
	}

	TArray( TArray<T> const& other )
	{
		m_allocSize = other.m_allocSize;
		m_size = other.m_size;
		m_data = (T*)malloc( m_allocSize * sizeof( T ) );

		IsPOD< T >::Create( m_data, other.m_data, m_size );
	}

	~TArray()
	{
		IsPOD< T >::Destroy( m_data, m_size );
		free( m_data );
	}

	void Resize( UINT const size )
	{
		if ( m_size < size )
		{
			if ( m_allocSize < size )
			{
				Reallocate( size );
			}

			IsPOD< T >::Create( &m_data[ m_size ], m_size, size );
		}
		else if ( size < m_size )
		{
			IsPOD< T >::Destroy( &m_data[size], size, m_size );
		}
		m_size = size;
	}

	void Reserve( UINT const size )
	{
		if ( m_allocSize < size )
		{
			Reallocate( size );
		}
	}

	void Add( T const& object )
	{
		if ( m_allocSize <= m_size )
		{
			Reallocate( ( m_allocSize + 1 ) << 1 );
		}
		
		IsPOD< T >::Create( &m_data[ m_size ], &object );
		++m_size;
	}
	void Add()
	{
		if ( m_allocSize <= m_size )
		{
			Reallocate( ( m_allocSize + 1 ) << 1 );
		}
		else
		{
			IsPOD< T >::Create( &m_data[ m_size ] );
		}
		++m_size;
	}
	void EraseBack()
	{
		ASSERT( 0 < m_size );
		IsPOD< T >::Destroy( &m_data[ m_size - 1 ] );
		--m_size;
	}

	void Clear()
	{
		IsPOD< T >::Destroy( m_data, m_size );
		m_size = 0;
	}

	void Free()
	{
		Reallocate( 0 );
	}

	FORCE_INLINE T* begin() 
	{ 
		return &m_data[0]; 
	}
	FORCE_INLINE T const* begin() const 
	{ 
		return &m_data[0]; 
	}
	FORCE_INLINE T* end() 
	{ 
		return &m_data[m_size]; 
	}
	FORCE_INLINE T const* end() const 
	{ 
		return &m_data[m_size]; 
	}

	FORCE_INLINE T* Data()
	{
		return m_data;
	}
	FORCE_INLINE T const* Data() const
	{
		return m_data;
	}

	FORCE_INLINE T& GetAt( UINT i )
	{
		ASSERT( i < m_size );
		return m_data[ i ];
	}
	FORCE_INLINE T const& GetAt( UINT i ) const
	{
		ASSERT( i < m_size );
		return m_data[ i ];
	}

	FORCE_INLINE T& operator[]( UINT i )
	{
		ASSERT( i < m_size );
		return m_data[ i ];
	}
	FORCE_INLINE T const& operator[]( UINT i ) const
	{
		ASSERT( i < m_size );
		return m_data[ i ];
	}

	FORCE_INLINE UINT Size() const
	{
		return m_size;
	}

	void operator=( TArray<T> const& other )
	{
		IsPOD< T >::Destroy( m_data, m_size );

		if ( m_allocSize != other.m_allocSize )
		{
			free( m_data );
			m_data = (T*)malloc( other.m_allocSize * sizeof( T ) );
			m_allocSize = other.m_allocSize;
		}

		m_size = other.m_size;
		IsPOD< T >::Create( m_data, other.m_data, m_size );
	}
};
