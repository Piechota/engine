#pragma once

struct SPosVertexFormat
{
	static D3D12_INPUT_ELEMENT_DESC const desc[];
	static UINT const descNum;

	Vec2 m_position;
};
POD_TYPE(SPosVertexFormat)

struct SPosUvVertexFormat
{
	static D3D12_INPUT_ELEMENT_DESC const desc[];
	static UINT const descNum;

	Vec2 m_position;
	Vec2 m_uv;
};
POD_TYPE(SPosUvVertexFormat)

struct SSimpleObjectVertexFormat
{
	static D3D12_INPUT_ELEMENT_DESC const desc[];
	static UINT const descNum;

	Vec3 m_position;
	Vec3 m_normal;
	Vec3 m_tangnet;
	Vec2 m_uv;

	bool operator==( SSimpleObjectVertexFormat const& other ) const
	{
		return m_position == other.m_position && m_normal == other.m_normal && m_tangnet == other.m_tangnet && m_uv == other.m_uv;
	}
};
POD_TYPE(SSimpleObjectVertexFormat)
