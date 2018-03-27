#pragma once

class CEntityManager
{
private:
	TArray< CEntity* > m_entities;
	TArray< UINT > m_entitiesToDestroy;
public:
	~CEntityManager();

	CEntity* GetEntity( UINT const entityID );
	UINT CreateEntity();
	FORCE_INLINE void DestroyEntity( UINT const entityID );
	void Clear();

	void Tick();
};

extern CEntityManager GEntityManager;
