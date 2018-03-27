#include "render.h"

struct SGlyphData
{
	Vec2 m_uv[ 4 ];
	Vec2 m_position[ 4 ];
};

#define GLYPH_UV( lbCornerX, lbCornerY, w, h ) \
{ lbCornerX / 512.f, ( ( lbCornerY - h ) / 512.f ) }, { (lbCornerX + w) / 512.f, ( ( lbCornerY - h ) / 512.f ) },	\
{ lbCornerX / 512.f, ( ( lbCornerY ) / 512.f ) }, { (lbCornerX + w) / 512.f, ( ( lbCornerY ) / 512.f ) }

#define GLYPH_POSITION_ALIGN_CENTER( w, h ) \
{ 0.5f - 0.5f * ( w / GMaxGlyphRatio.x ), ( GMaxGlyphRatio.y / GMaxGlyphRatio.x ) * ( 0.5f + 0.5f * ( h / GMaxGlyphRatio.y ) ) },	{ 0.5f + 0.5f * ( w / GMaxGlyphRatio.x ), ( GMaxGlyphRatio.y / GMaxGlyphRatio.x ) * ( 0.5f + 0.5f * ( h / GMaxGlyphRatio.y ) ) },	\
{ 0.5f - 0.5f * ( w / GMaxGlyphRatio.x ), ( GMaxGlyphRatio.y / GMaxGlyphRatio.x ) * ( 0.5f - 0.5f * ( h / GMaxGlyphRatio.y ) ) },	{ 0.5f + 0.5f * ( w / GMaxGlyphRatio.x ), ( GMaxGlyphRatio.y / GMaxGlyphRatio.x ) * ( 0.5f - 0.5f * ( h / GMaxGlyphRatio.y ) ) }

#define GLYPH_POSITION_ALIGN_TOP( w, h ) \
{ 0.5f - 0.5f * ( w / GMaxGlyphRatio.x ), GMaxGlyphRatio.y / GMaxGlyphRatio.x },	{ 0.5f + 0.5f * ( w / GMaxGlyphRatio.x ), GMaxGlyphRatio.y / GMaxGlyphRatio.x },	\
{ 0.5f - 0.5f * ( w / GMaxGlyphRatio.x ), ( GMaxGlyphRatio.y / GMaxGlyphRatio.x ) * ( 1.f - ( h / GMaxGlyphRatio.y ) ) },	{ 0.5f + 0.5f * ( w / GMaxGlyphRatio.x ), ( GMaxGlyphRatio.y / GMaxGlyphRatio.x ) * ( 1.f - ( h / GMaxGlyphRatio.y ) ) }

#define GLYPH_POSITION_ALIGN_BOTTOM( w, h ) \
{ 0.5f - 0.5f * ( w / GMaxGlyphRatio.x ), ( GMaxGlyphRatio.y / GMaxGlyphRatio.x ) * ( h / GMaxGlyphRatio.y ) },	{ 0.5f + 0.5f * ( w / GMaxGlyphRatio.x ), ( GMaxGlyphRatio.y / GMaxGlyphRatio.x ) * ( h / GMaxGlyphRatio.y ) },	\
{ 0.5f - 0.5f * ( w / GMaxGlyphRatio.x ), 0.f },	{ 0.5f + 0.5f * ( w / GMaxGlyphRatio.x ), 0.f }

#define GLYPH_CENTER( lbCornerX, lbCornerY, w, h )	\
{													\
	{												\
		GLYPH_UV( lbCornerX, lbCornerY, w, h )		\
	},												\
	{												\
		GLYPH_POSITION_ALIGN_CENTER( w, h )			\
	}												\
}													\

#define GLYPH_TOP( lbCornerX, lbCornerY, w, h )		\
{													\
	{												\
		GLYPH_UV( lbCornerX, lbCornerY, w, h )		\
	},												\
	{												\
		GLYPH_POSITION_ALIGN_TOP( w, h )			\
	}												\
}													\

#define GLYPH_BOTTOM( lbCornerX, lbCornerY, w, h )	\
{													\
	{												\
		GLYPH_UV( lbCornerX, lbCornerY, w, h )		\
	},												\
	{												\
		GLYPH_POSITION_ALIGN_BOTTOM( w, h )			\
	}												\
}													\

Vec2 const GMaxGlyphRatio( 30.f, 33.f );
SGlyphData const GGlyphData[] =
{
	GLYPH_BOTTOM(	2.f,		28.f,	12.f, 27.f ),			// ! //
	GLYPH_TOP(		16.f,		17.f,	16.f, 16.f ),			// " //
	GLYPH_CENTER(	34.f,		27.f,	23.f, 26.f ),			// # //
	GLYPH_BOTTOM(	59.f,		30.f,	20.f, 29.f ),			// $ //
	GLYPH_BOTTOM(	81.f,		28.f,	26.f, 27.f ),			// % //
	GLYPH_BOTTOM(	109.f,		28.f,	25.f, 27.f ),			// & //
	GLYPH_TOP(		136.f,		17.f,	11.f, 16.f ),			// ' //
	GLYPH_BOTTOM(	149.f,		31.f,	15.f, 30.f ),			// ( //
	GLYPH_BOTTOM(	166.f,		31.f,	15.f, 30.f ),			// ) //
	GLYPH_CENTER(	183.f,		21.f,	20.f, 20.f ),			// * //
	GLYPH_CENTER(	205.f,		22.f,	20.f, 21.f ),			// + //
	GLYPH_BOTTOM(	227.f,		16.f,	13.f, 15.f ),			// , //
	GLYPH_CENTER(	242.f,		12.f,	15.f, 11.f ),			// - //
	GLYPH_BOTTOM(	259.f,		13.f,	12.f, 12.f ),			// . //
	GLYPH_BOTTOM(	273.f,		27.f,	17.f, 26.f ),			// / //
	GLYPH_BOTTOM(	292.f,		28.f,	20.f, 27.f ),			// 0 //
	GLYPH_BOTTOM(	314.f,		27.f,	15.f, 26.f ),			// 1 //
	GLYPH_BOTTOM(	331.f,		27.f,	20.f, 26.f ),			// 2 //
	GLYPH_BOTTOM(	353.f,		28.f,	20.f, 27.f ),			// 3 //
	GLYPH_BOTTOM(	375.f,		27.f,	22.f, 26.f ),			// 4 //
	GLYPH_BOTTOM(	399.f,		28.f,	20.f, 27.f ),			// 5 //
	GLYPH_BOTTOM(	421.f,		28.f,	20.f, 27.f ),			// 6 //
	GLYPH_BOTTOM(	443.f,		27.f,	20.f, 26.f ),			// 7 //
	GLYPH_BOTTOM(	465.f,		28.f,	20.f, 27.f ),			// 8 //
	GLYPH_BOTTOM(	487.f,		28.f,	20.f, 27.f ),			// 9 //
	GLYPH_BOTTOM(	242.f,		37.f,	12.f, 23.f ),			// : //
	GLYPH_BOTTOM(	256.f,		41.f,	13.f, 26.f ),			// ; //
	GLYPH_CENTER(	183.f,		44.f,	20.f, 21.f ),			// < //
	GLYPH_CENTER(	205.f,		39.f,	20.f, 15.f ),			// = //
	GLYPH_CENTER(	16.f,		50.f,	20.f, 21.f ),			// > //
	GLYPH_BOTTOM(	271.f,		56.f,	18.f, 27.f ),			// ? //
	GLYPH_BOTTOM(	314.f,		58.f,	28.f, 29.f ),			// @ //
	GLYPH_BOTTOM(	344.f,		56.f,	24.f, 26.f ),			// A //
	GLYPH_BOTTOM(	375.f,		55.f,	21.f, 26.f ),			// B //
	GLYPH_BOTTOM(	398.f,		57.f,	22.f, 27.f ),			// C //
	GLYPH_BOTTOM(	422.f,		56.f,	23.f, 26.f ),			// D //
	GLYPH_BOTTOM(	38.f,		55.f,	18.f, 26.f ),			// E //
	GLYPH_BOTTOM(	291.f,		56.f,	18.f, 26.f ),			// F //
	GLYPH_BOTTOM(	447.f,		57.f,	23.f, 27.f ),			// G //
	GLYPH_BOTTOM(	472.f,		56.f,	22.f, 26.f ),			// H //
	GLYPH_BOTTOM(	227.f,		44.f,	11.f, 26.f ),			// I //
	GLYPH_BOTTOM(	81.f,		61.f,	15.f, 31.f ),			// J //
	GLYPH_BOTTOM(	98.f,		56.f,	21.f, 26.f ),			// K //
	GLYPH_BOTTOM(	121.f,		56.f,	18.f, 26.f ),			// L //
	GLYPH_BOTTOM(	141.f,		59.f,	26.f, 26.f ),			// M //
	GLYPH_BOTTOM(	240.f,		69.f,	22.f, 26.f ),			// N //
	GLYPH_BOTTOM(	169.f,		73.f,	25.f, 27.f ),			// O //
	GLYPH_BOTTOM(	58.f,		58.f,	20.f, 26.f ),			// P //
	GLYPH_BOTTOM(	196.f,		77.f,	25.f, 31.f ),			// Q //
	GLYPH_BOTTOM(	2.f,		78.f,	21.f, 26.f ),			// R //
	GLYPH_BOTTOM(	370.f,		84.f,	20.f, 27.f ),			// S //
	GLYPH_BOTTOM(	25.f,		83.f,	22.f, 26.f ),			// T //
	GLYPH_BOTTOM(	264.f,		85.f,	22.f, 27.f ),			// U //
	GLYPH_BOTTOM(	288.f,		84.f,	23.f, 26.f ),			// V //
	GLYPH_BOTTOM(	472.f,		84.f,	30.f, 26.f ),			// W //
	GLYPH_BOTTOM(	422.f,		84.f,	22.f, 26.f ),			// X //
	GLYPH_BOTTOM(	344.f,		84.f,	22.f, 26.f ),			// Y //
	GLYPH_BOTTOM(	98.f,		84.f,	21.f, 26.f ),			// Z //
	GLYPH_BOTTOM(	223.f,		76.f,	15.f, 30.f ),			// [ //
	GLYPH_BOTTOM(	121.f,		84.f,	17.f, 26.f ),			// \ //
	GLYPH_BOTTOM(	446.f,		89.f,	14.f, 30.f ),			// ] //
	GLYPH_TOP(		392.f,		79.f,	21.f, 20.f ),			// ^ //
	GLYPH_BOTTOM(	313.f,		70.f,	20.f, 10.f ),			// _ //
	GLYPH_TOP(		49.f,		73.f,	14.f, 13.f ),			// ` //
	GLYPH_BOTTOM(	140.f,		84.f,	19.f, 23.f ),			// a //
	GLYPH_BOTTOM(	65.f,		91.f,	20.f, 28.f ),			// b //
	GLYPH_BOTTOM(	240.f,		94.f,	18.f, 23.f ),			// c //
	GLYPH_BOTTOM(	313.f,		100.f,	20.f, 28.f ),			// d //
	GLYPH_BOTTOM(	161.f,		98.f,	20.f, 23.f ),			// e //
	GLYPH_BOTTOM(	183.f,		106.f,	18.f, 27.f ),			// f //
	GLYPH_BOTTOM(	203.f,		107.f,	21.f, 28.f ),			// g //
	GLYPH_BOTTOM(	2.f,		107.f,	19.f, 27.f ),			// h //
	GLYPH_BOTTOM(	49.f,		101.f,	12.f, 26.f ),			// i //
	GLYPH_BOTTOM(	392.f,		113.f,	15.f, 32.f ),			// j //
	GLYPH_BOTTOM(	23.f,		112.f,	19.f, 27.f ),			// k //
	GLYPH_BOTTOM(	227.f,		44.f,	11.f, 26.f ),			// l //
	GLYPH_BOTTOM(	409.f,		108.f,	27.f, 22.f ),			// m //
	GLYPH_BOTTOM(	369.f,		108.f,	19.f, 22.f ),			// n //
	GLYPH_BOTTOM(	335.f,		109.f,	21.f, 23.f ),			// o //
	GLYPH_BOTTOM(	462.f,		114.f,	20.f, 28.f ),			// p //
	GLYPH_BOTTOM(	87.f,		114.f,	20.f, 28.f ),			// q //
	GLYPH_BOTTOM(	484.f,		108.f,	16.f, 22.f ),			// r //
	GLYPH_BOTTOM(	288.f,		109.f,	18.f, 23.f ),			// s //
	GLYPH_BOTTOM(	109.f,		111.f,	16.f, 25.f ),			// t //
	GLYPH_BOTTOM(	127.f,		108.f,	20.f, 22.f ),			// u //
	GLYPH_BOTTOM(	260.f,		108.f,	21.f, 21.f ),			// v //
	GLYPH_BOTTOM(	149.f,		121.f,	27.f, 21.f ),			// w //
	GLYPH_BOTTOM(	438.f,		112.f,	21.f, 21.f ),			// x //
	GLYPH_BOTTOM(	63.f,		120.f,	21.f, 27.f ),			// y //
	GLYPH_BOTTOM(	239.f,		117.f,	19.f, 21.f ),			// z //
	GLYPH_BOTTOM(	308.f,		132.f,	17.f, 30.f ),			// { //
	GLYPH_CENTER(	44.f,		136.f,	11.f, 33.f ),			// | //
	GLYPH_BOTTOM(	178.f,		138.f,	17.f, 30.f ),			// } //
	GLYPH_TOP(		197.f,		121.f,	20.f, 12.f ),			// ~ //
};

void CTextRenderManager::Init()
{
	m_dynGeometryID = GDynamicGeometryManager.AllocateGeometry( CHAR_MAX_NUM * 4 * sizeof( SPosUvVertexFormat ), sizeof( SPosUvVertexFormat ), CHAR_MAX_NUM * 6 * ( GDXGIFormatsBitsSize[ DXGI_FORMAT_R16_UINT ] / 8 ), DXGI_FORMAT_R16_UINT );
}

void CTextRenderManager::Print( Vec4 const color, Vec2 position, float const size, char const* msg )
{
	position = 2.f * position - 1.f;

	STextRenderInfo renderInfo;

	SPosUvVertexFormat* vertices = nullptr;
	UINT16* indices = nullptr;

	UINT const charNum = UINT(strlen( msg ));

	GDynamicGeometryManager.GetVerticesForWrite( 4 * charNum, m_dynGeometryID, reinterpret_cast< void*& >( vertices ), renderInfo.m_verticesStart );
	ASSERT( vertices );
	GDynamicGeometryManager.GetIndicesForWrite( 6 * charNum, m_dynGeometryID, reinterpret_cast< void*& >( indices ), renderInfo.m_indicesStart );
	ASSERT( indices );

	UINT indicesToDraw = 0;
	for ( UINT iChar = 0; iChar < charNum; ++iChar )
	{
		char const currentChar = msg[ iChar ];
		if ( currentChar != ' ' )
		{
			ASSERT( currentChar >= '!' && '~' >= currentChar );
			UINT const filledVertices = 4 * indicesToDraw / 6;
			indices[ indicesToDraw + 0 ] = filledVertices + 0;
			indices[ indicesToDraw + 1 ] = filledVertices + 1;
			indices[ indicesToDraw + 2 ] = filledVertices + 2;

			indices[ indicesToDraw + 3 ] = filledVertices + 2;
			indices[ indicesToDraw + 4 ] = filledVertices + 1;
			indices[ indicesToDraw + 5 ] = filledVertices + 3;

			indicesToDraw += 6;

			SGlyphData const glyph = GGlyphData[ currentChar - 33 ];
			vertices[ filledVertices + 0 ].m_position.Set( position.x + glyph.m_position[0].x * size, position.y + glyph.m_position[0].y * size );
			vertices[ filledVertices + 0 ].m_uv = glyph.m_uv[ 0 ];

			vertices[ filledVertices + 1 ].m_position.Set( position.x + glyph.m_position[1].x * size, position.y + glyph.m_position[1].y * size );
			vertices[ filledVertices + 1 ].m_uv = glyph.m_uv[ 1 ];

			vertices[ filledVertices + 2 ].m_position.Set( position.x + glyph.m_position[2].x * size, position.y + glyph.m_position[2].y * size );
			vertices[ filledVertices + 2 ].m_uv = glyph.m_uv[ 2 ];

			vertices[ filledVertices + 3 ].m_position.Set( position.x + glyph.m_position[3].x * size, position.y + glyph.m_position[3].y * size );
			vertices[ filledVertices + 3 ].m_uv = glyph.m_uv[ 3 ];
		}

		position.x += 0.5f * size;
	}

	renderInfo.m_indicesNum = indicesToDraw;
	renderInfo.m_color = color;

	m_textRenderInfos.Add( renderInfo );
}
void CTextRenderManager::FillRenderData()
{
	SCommonRenderData renderData;
	renderData.m_drawType = EDrawType::DrawIndexedInstanced;
	renderData.m_shaderID = EShaderType::ST_SDF_DRAW;
	renderData.m_texturesOffset = GRender.GetTexturesOffset();
	renderData.m_texturesNum = 1;
	GRender.AddTextureID( ETextures::T_SDF_FONT_512 );
	renderData.m_topology = D3D_PRIMITIVE_TOPOLOGY::D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	renderData.m_instancesNum = 1;
	renderData.m_geometryID = GDynamicGeometryManager.GetGeometryID( m_dynGeometryID );
	Vec2 const cutOff( 0.7f - 0.035f, 0.7f + 0.035f );

	UINT const textRenderInfoNum = m_textRenderInfos.Size();
	for ( UINT i = 0; i < textRenderInfoNum; ++i )
	{
		STextRenderInfo const& renderInfo = m_textRenderInfos[ i ];
		renderData.m_verticesStart = renderInfo.m_verticesStart;
		renderData.m_indicesStart = renderInfo.m_indicesStart;
		renderData.m_indicesNum = renderInfo.m_indicesNum;

		CConstBufferCtx const cbCtx = GRender.GetConstBufferCtx( renderData.m_cbOffset, EShaderType::ST_SDF_DRAW );
		cbCtx.SetParam( &renderInfo.m_color,	sizeof( renderInfo.m_color ),	EShaderParameters::Color );
		cbCtx.SetParam( &cutOff,				sizeof( cutOff ),				EShaderParameters::Cutoff );

		GRender.AddCommonRenderData( renderData, RL_OVERLAY );
	}

	m_textRenderInfos.Clear();
}


CTextRenderManager GTextRenderManager;