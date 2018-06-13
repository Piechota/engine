#include "../headers.h"
#include "resourceManager.h"
#include "texture.h"
#include "../rendering/render.h"
#include "../../DirectXTex/DirectXTex.h"
#include "../geometryLoader.h"

void CTextureRes::Load( std::wstring const& path )
{
	DirectX::TexMetadata texMeta;
	DirectX::ScratchImage image;
	if ( DirectX::LoadFromWICFile( path.c_str(), DirectX::WIC_FLAGS_NONE, &texMeta, image ) != S_OK )
	{
		CheckResult( DirectX::LoadFromDDSFile( path.c_str(), DirectX::DDS_FLAGS_NONE, &texMeta, image ) );
	}

	STextureInfo texture;
	texture.m_width = UINT(texMeta.width);
	texture.m_height = UINT(texMeta.height);
	texture.m_format = texMeta.format;
	texture.m_mipLevels = Byte(texMeta.mipLevels);

	ASSERT( texture.m_format != DXGI_FORMAT_UNKNOWN );
	m_id = GRender.LoadResource( texture, image.GetPixels() );
}

void CStaticGeometryRes::Load( std::string const& path )
{
	SGeometryData geometryData;
	GGeometryLoader.LoadMesh( geometryData, path.c_str() );

	m_id = GRender.LoadResource( geometryData );
}

tTextureResManager GTextureResources;
tStaticGeometryResManager GStaticGeometryResources;
