#pragma once

enum EShaderType
{
	ST_OBJECT_DRAW,
	ST_OBJECT_DRAW_NO_TEXTURES,
	ST_OBJECT_DRAW_GEOMETRY_ONLY,
	ST_OBJECT_DRAW_SIMPLE,
	ST_OBJECT_DRAW_SIMPLE_TEXTURE,
	ST_SDF_DRAW,
	ST_ENV_PARTICLE,
	ST_RECT_DRAW,
	ST_LTC_TEXTURE,

	ST_MAX
};

enum ELightType 
{
	LT_POINT,

	LT_MAX
};

enum ESoundEffectType
{
	SET_SHOOT0,
	SET_SHOOT1,
	SET_SHOOT2,
	SET_HEAL,
	SET_EXPLOSION,
	SET_BUILD,

	SET_MAX
};

enum ECollisionFlag
{
	CF_PLAYER			= 1 << 0,
	CF_PLAYER_BULLET	= 1 << 1,
	CF_ENEMY			= 1 << 2,
	CF_ENEMY_BULLET		= 1 << 3,
};

extern SViewObject GViewObject;

enum EComponentType
{
	CT_INVALID,
	CT_Transform,
	CT_StaticMesh,
	CT_Light,
	CT_Camera,
	CT_Rigidbody,

	CT_Num,
};

struct SComponentHandle
{
	UINT32 m_index : 24;
	UINT32 m_type : 8;
};
POD_TYPE(SComponentHandle)

typedef SComponentHandle(*AddComponentFunc)();
typedef void(*RemoveComponentFunc)(SComponentHandle const handle);

#include "componentsHelper.h"

#include "componentTransform.h"
#include "componentStaticMesh.h"
#include "componentLight.h"
#include "componentCamera.h"
#include "componentRigidbody.h"
