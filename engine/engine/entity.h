#pragma once

class CEntity
{
private:
	TArray< SComponentHandle > m_components;

public:
	void Destroy();
	SComponentHandle AddComponentTransform();
	SComponentHandle GetComponentTransform() const;

	SComponentHandle AddComponentStaticMesh();
	SComponentHandle GetComponentStaticMesh() const;

	SComponentHandle AddComponentLight();
	SComponentHandle GetComponentLight() const;

	SComponentHandle AddComponentCamera();
	SComponentHandle GetComponentCamera() const;
};
