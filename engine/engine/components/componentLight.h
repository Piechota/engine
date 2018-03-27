#pragma once

struct SComponentLight
{
	Vec3 m_color;
	float m_radius;
	float m_fade;

	Byte m_lighShader;
};

POD_TYPE( SComponentLight )

class CComponentLightManager : public TComponentContainer< SComponentLight, EComponentType::CT_Light>
{
private:
	struct SRenderComponents
	{
		UINT32 m_transformID;
		UINT32 m_lightID;
	};
	TArray< SRenderComponents > m_renderComponents;

	Vec3	m_directLightDir;
	Vec3	m_directLightColor;
	Vec3	m_ambientLightColor;

public:
	void RegisterRenderComponents( UINT32 const transformID, UINT32 const lightID )
	{
		m_renderComponents.Add( { transformID, lightID } );
	}

	void SetDirectLightDir( Vec3 const dir ) { m_directLightDir = dir; }
	void SetDirectLightColor( Vec3 const color ) { m_directLightColor = color; }
	void SetAmbientLightColor( Vec3 const color ) { m_ambientLightColor = color; }

	void FillRenderData() const;
};
