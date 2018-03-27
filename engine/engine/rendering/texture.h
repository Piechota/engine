#pragma once
struct STexture
{
	unsigned int m_width;
	unsigned int m_height;
	Byte* m_data;
	DXGI_FORMAT m_format;
	Byte m_mipLevels;
};
