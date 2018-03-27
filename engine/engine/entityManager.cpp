#include "headers.h"

CEntityManager::~CEntityManager()
{
	UINT const entitesNum = m_entities.Size();
	for ( UINT i = 0; i < entitesNum; ++i )
	{
		delete m_entities[ i ];
	}
}

CEntity* CEntityManager::GetEntity( UINT const entityID )
{
	return m_entities[ entityID ];
}

UINT CEntityManager::CreateEntity()
{
	UINT const entityID = m_entities.Size();
	m_entities.Add( new CEntity() );
	return entityID;
}

void CEntityManager::DestroyEntity( UINT const entityID ) 
{
	m_entitiesToDestroy.Add( entityID );
}

void CEntityManager::Tick()
{
	UINT const entitiesToDestroyNum = m_entitiesToDestroy.Size();
	for ( UINT i = 0; i < entitiesToDestroyNum; ++i )
	{
		UINT const entityID = m_entitiesToDestroy[ i ];
		m_entities[ entityID ]->Destroy();
		delete m_entities[ entityID ];
		m_entities[ entityID ] = nullptr;
	}
	m_entitiesToDestroy.Clear();
}
void CEntityManager::Clear()
{
	UINT const entitesNum = m_entities.Size();
	for ( UINT i = 0; i < entitesNum; ++i )
	{
		delete m_entities[ i ];
	}

	m_entities.Clear();
}

CEntityManager GEntityManager;