#pragma once
template <typename T, UINT SIZE >
class TStaticArray
{
private:
	T m_data[SIZE];
	UINT m_size;

public:
	TStaticArray()
		: m_size( 0 )
	{}

	TStaticArray( TArray<T> const& other )
	{
		m_size = other.m_size;
		IsPOD< T >::Create( m_data, other.m_data, m_size );
	}

	~TStaticArray()
	{
		IsPOD< T >::Destroy( m_data, m_size );
	}

	void Resize( UINT const size )
	{
		ASSERT( size <= SIZE );
		if ( m_size < size )
		{
			IsPOD< T >::Create( &m_data[ m_size ], m_size, size );
		}
		else if ( size < m_size )
		{
			IsPOD< T >::Destroy( &m_data[size], size, m_size );
		}
		m_size = size;
	}

	void Add( T const& object )
	{
		ASSERT( m_size < SIZE );
		m_data[ m_size ] = object;
		++m_size;
	}
	void Add()
	{
		ASSERT( m_size < SIZE );
		IsPOD< T >::Create( &m_data[ m_size ] );
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

	void operator=( TStaticArray<T, SIZE> const& other )
	{
		IsPOD< T >::Destroy( m_data, m_size );
		m_size = other.m_size;
		IsPOD< T >::Create( m_data, other.m_data, m_size );
	}
};
