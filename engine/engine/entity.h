#pragma once

class CEntity
{
private:
	TArray< SComponentHandle > m_components;

public:
	void Destroy();
	SComponentHandle AddComponent( EComponentType const type );
	SComponentHandle GetComponent( EComponentType const type ) const;
};
