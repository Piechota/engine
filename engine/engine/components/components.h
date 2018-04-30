#pragma once

enum EShaderType
{
	ST_OBJECT_DRAW,
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

enum EGeometry
{
	G_SPACESHIP,
	G_BOX,
	G_SCENE_TEST,
	G_PLANE,

	G_MAX
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

struct SGeometryInfo
{
	UINT m_indicesNum;
	Byte m_geometryID;
};
POD_TYPE( SGeometryInfo )

extern SViewObject GViewObject;
extern SGeometryInfo GGeometryInfo[ G_MAX ];

enum EComponentType
{
	CT_INVALID,
	CT_Transform,
	CT_StaticMesh,
	CT_Light,
	CT_Physics,
	CT_Camera,

	CT_Num,
};

struct SComponentHandle
{
	UINT32 m_index : 24;
	UINT32 m_type : 8;
};
POD_TYPE( SComponentHandle )

#include "componentContainer.h"

#include "componentTransform.h"
#include "componentStaticMesh.h"
#include "componentLight.h"
#include "componentPhysics.h"
#include "componentCamera.h"

#include "componentsManagers.h"