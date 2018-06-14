#pragma once

class CTextureRes 
{
public:
	CTextureRes()
		: m_id( 0 )
	{}

	uint16_t m_id;

	void Load( std::wstring const& path );
};

class CStaticGeometryRes 
{
public:
	CStaticGeometryRes()
		: m_id( 0 )
	{}

	uint16_t m_id;

	void Load( std::string const& path );
};

template< class K, class V >
class NOVTABLE IResourceManager : public std::map< K, V >
{
public:
	V& operator[]( K const& key )
	{
		V* value = nullptr;

		std::map< K, V >::iterator itr = std::map< K, V >::find( key );
		if ( itr == std::map< K, V >::end() )
		{
			value = &std::map< K, V >::operator[]( key );
			value->Load( key );
		}
		else
		{
			value = &itr->second;
		}

		return *value;
	}
};

typedef IResourceManager< std::wstring, CTextureRes > tTextureResManager;
typedef IResourceManager< std::string, CStaticGeometryRes > tStaticGeometryResManager;

extern tTextureResManager GTextureResources;
extern tStaticGeometryResManager GStaticGeometryResources;

