#pragma once
struct STextureInfo
{
	unsigned int m_width;
	unsigned int m_height;
	DXGI_FORMAT m_format;
	Byte m_mipLevels;
};
POD_TYPE( STextureInfo )

extern tTextureResManager GTextureResources;
