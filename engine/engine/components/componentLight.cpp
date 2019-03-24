#include "../headers.h"
#include "../rendering/render.h"

#define COMP_TYPE SComponentLight
#define COMP_ID EComponentType::CT_Light

namespace ComponentLightManager
{
#include "componentsImpl.h"

	struct SRenderComponents
	{
		UINT32 m_transformID;
		UINT32 m_lightID;
	};
	TArray< SRenderComponents > m_renderComponents;

	Vec3	m_directLightDir;
	Vec3	m_directLightColor;
	Vec3	m_ambientLightColor;

	void RegisterRenderComponents(SComponentHandle const transformHandle, SComponentHandle const lighHandle)
	{
		ASSERT(transformHandle.m_type == EComponentType::CT_Transform);
		ASSERT(lighHandle.m_type == EComponentType::CT_Light);

		m_renderComponents.Add({ transformHandle.m_index, lighHandle.m_index });
	}

	void SetDirectLightDir(Vec3 const dir) { m_directLightDir = dir; }
	void SetDirectLightColor(Vec3 const color) { m_directLightColor = color; }
	void SetAmbientLightColor(Vec3 const color) { m_ambientLightColor = color; }

	void FillRenderData()
	{
		float constexpr planeVertices[] =
		{
			-1.f, 0.f, 1.f,
			1.f, 0.f, 1.f,
			1.f, 0.f, -1.f,
			-1.f, 0.f, -1.f
		};

		Matrix4x4 const worldToView = GViewObject.m_camera.m_worldToView;

		{
			Byte dirLightFlags = Byte(LF_AMBIENT);
			if (m_directLightColor != Vec3(0.f, 0.f, 0.f))
			{
				dirLightFlags |= Byte(LF_DIRECT);
			}

			SLightRenderData lightRenderData;
			lightRenderData.m_texturesNum = 0;
			lightRenderData.m_lightShader = dirLightFlags;

			Vec3 const directionLightVS = Math::MulVectorOrtho(m_directLightDir, worldToView);

			CConstBufferCtx const cbCtx = GRender.GetLightConstBufferCtx(lightRenderData.m_cbOffset, dirLightFlags);
			if (dirLightFlags & Byte(LF_DIRECT))
			{
				cbCtx.SetParam(&directionLightVS, sizeof(directionLightVS), Hash("LightDirVS"));
				cbCtx.SetParam(&m_directLightColor, sizeof(m_directLightColor), Hash("Color"));
			}
			cbCtx.SetParam(&m_ambientLightColor, sizeof(m_ambientLightColor), Hash("AmbientColor"));

			GRender.AddLightRenderData(lightRenderData);
		}

		SLightRenderData lightRenderData;
		UINT const renderComponentsNum = m_renderComponents.Size();
		GRender.LightRenderDataReserveNext(renderComponentsNum);

		Vec4 areaVertices[4];
		for (UINT i = 0; i < renderComponentsNum; ++i)
		{
			SComponentTransform const transform = ComponentTransformManager::GetComponentNoCheck(m_renderComponents[i].m_transformID);
			SComponentLight const light = GetComponentNoCheck(m_renderComponents[i].m_lightID);

			lightRenderData.m_lightShader = light.m_lightShader;
			lightRenderData.m_texturesNum = 0;

			if (light.m_lightShader & LF_TEXTURE)
			{
				lightRenderData.m_texturesNum = 1;
				lightRenderData.m_texturesOffset = GRender.GetTexturesOffset();
				GRender.AddTextureID(light.m_textureID);
			}

			Matrix4x4 const objectToWorld = Matrix4x4::GetTranslateRotationSize(transform.m_position, transform.m_rotation, transform.m_scale);
			Matrix4x4 const objectToView = Math::Mul(objectToWorld, worldToView);

			if (light.m_lightShader & LF_LTC)
			{
				for (UINT i = 0; i < 4; ++i)
				{
					areaVertices[i] = Math::MulPositionOrtho(&planeVertices[i * 3], objectToView);
				}
			}
			Vec3 const positionVS = Math::MulPositionOrtho(transform.m_position, worldToView);
			Vec2 const attenuation(1.f / light.m_radius, light.m_fade);

			CConstBufferCtx const cbCtx = GRender.GetLightConstBufferCtx(lightRenderData.m_cbOffset, light.m_lightShader);
			cbCtx.SetParam(&positionVS, sizeof(positionVS), Hash("LightPos"));
			cbCtx.SetParam(&attenuation, sizeof(attenuation), Hash("Attenuation"));
			cbCtx.SetParam(&light.m_color, sizeof(light.m_color), Hash("Color"));
			cbCtx.SetParam(areaVertices, sizeof(areaVertices), Hash("Vertices"));

			GRender.AddLightRenderData(lightRenderData);
		}
	}
}
	
#undef COMP_TYPE 
#undef COMP_ID 