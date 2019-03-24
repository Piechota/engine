#pragma once

struct SComponentLight
{
	Vec3 m_color;
	float m_radius;
	float m_fade;

	UINT16 m_textureID;
	Byte m_lightShader;
};
POD_TYPE( SComponentLight )

namespace ComponentLightManager
{
	DECLARE_COMPONENT(SComponentLight)

	extern void RegisterRenderComponents(SComponentHandle const transformHandle, SComponentHandle const lighHandle);
	extern void SetDirectLightDir(Vec3 const dir);
	extern void SetDirectLightColor(Vec3 const color);
	extern void SetAmbientLightColor(Vec3 const color);
	extern void FillRenderData();
}