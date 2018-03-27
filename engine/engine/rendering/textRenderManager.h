#pragma once
#include "dynamicGeometryManager.h"

struct STextRenderInfo
{
	Vec4 m_color;
	UINT16 m_verticesStart;
	UINT16 m_indicesStart;
	UINT16 m_indicesNum;
};
POD_TYPE( STextRenderInfo )

class CTextRenderManager
{
private:
	enum
	{
		CHAR_MAX_NUM = 512
	};

	TArray< STextRenderInfo > m_textRenderInfos;
	tDynGeometryID m_dynGeometryID;

public:
	void Init();
	void Print( Vec4 const color, Vec2 position, float const size, char const* msg );
	void FillRenderData();
};

extern CTextRenderManager GTextRenderManager;