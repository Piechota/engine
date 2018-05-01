#pragma once

struct SComponentLight
{
	Vec3 m_color;
	float m_radius;
	float m_fade;

	Byte m_textureID;
	Byte m_lightShader;
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
	void RegisterRenderComponents( SComponentHandle const transformHandle, SComponentHandle const lighHandle )
	{
		ASSERT( transformHandle.m_type == EComponentType::CT_Transform );
		ASSERT( lighHandle.m_type == EComponentType::CT_Light );

		m_renderComponents.Add( { transformHandle.m_index, lighHandle.m_index } );
	}

	void SetDirectLightDir( Vec3 const dir ) { m_directLightDir = dir; }
	void SetDirectLightColor( Vec3 const color ) { m_directLightColor = color; }
	void SetAmbientLightColor( Vec3 const color ) { m_ambientLightColor = color; }

	void FillRenderData() const;
};
