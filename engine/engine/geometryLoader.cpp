#include "headers.h"
#include "geometryLoader.h"
#include "rendering\vertexFormats.h"
#include "rendering\textRenderManager.h"
#include "utility\fbxsdk.h"
#include <fstream>

UINT64 const GStaticMeshesVersion = 1;

void CGeometryLoader::CreateMeshFromFbx( char const * file )
{
#ifndef FINAL_BUILD
	FbxNode* fbxNodeRoot = m_fbxScene->GetRootNode();
	if ( fbxNodeRoot )
	{
		int const childNum = fbxNodeRoot->GetChildCount();
		ASSERT_STR( childNum == 1, "FBX: More than 1 child not supported" );

		TArray<SSimpleObjectVertexFormat> vertices;
		TArray<UINT16> indices;

		FbxNode* fbxNodeChild = fbxNodeRoot->GetChild( 0 );
		ASSERT( fbxNodeChild->GetNodeAttribute()->GetAttributeType() == FbxNodeAttribute::eMesh );

		FbxDouble3 const& fbxScale = fbxNodeChild->LclScaling;
		Vec3 const fScale( fbxScale[ 0 ], fbxScale[ 1 ], fbxScale[ 2 ] );

		FbxMesh* fbxMesh = reinterpret_cast< FbxMesh* >( fbxNodeChild->GetNodeAttribute() );
		fbxMesh->GenerateTangentsData( 0 );

		FbxVector4 const* fbxMeshControlPoints = fbxMesh->GetControlPoints();

		FbxGeometryElementUV const* fbxUvs = nullptr;
		FbxGeometryElementNormal const* fbxNormals = nullptr;
		FbxGeometryElementTangent const* fbxTangents = nullptr;

		ASSERT( 0 < fbxMesh->GetElementUVCount() );
		ASSERT( 0 < fbxMesh->GetElementNormalCount() );
		ASSERT( 0 < fbxMesh->GetElementBinormalCount() );

		fbxUvs = fbxMesh->GetElementUV( 0 );
		fbxNormals = fbxMesh->GetElementNormal( 0 );
		fbxTangents = fbxMesh->GetElementTangent( 0 );

		UINT vertexID = 0;
		UINT const polygonNum = UINT( fbxMesh->GetPolygonCount() );
		for ( UINT polygonID = 0; polygonID < polygonNum; ++polygonID )
		{
			for ( UINT polygonVertexID = 0; polygonVertexID < 3; ++polygonVertexID )
			{
				UINT const meshVertexID = UINT( fbxMesh->GetPolygonVertex( polygonID, polygonVertexID ) );
				FbxVector4 const& fbxPosition = fbxMeshControlPoints[ meshVertexID ];

				SSimpleObjectVertexFormat vertex;
				vertex.m_position.Set( float( fbxPosition.mData[ 0 ] ), float( fbxPosition.mData[ 1 ] ), float( fbxPosition.mData[ 2 ] ) );

				UINT idUv = vertexID;
				UINT idNormal = vertexID;
				UINT idTangent = vertexID;

				if ( fbxUvs->GetMappingMode() == FbxGeometryElement::eByControlPoint )
				{
					idUv = meshVertexID;
				}

				if ( fbxUvs->GetReferenceMode() == FbxGeometryElement::eIndexToDirect )
				{
					idUv = fbxUvs->GetIndexArray().GetAt( idUv );
				}

				if ( fbxNormals->GetMappingMode() == FbxGeometryElement::eByControlPoint )
				{
					idNormal = meshVertexID;
				}

				if ( fbxNormals->GetReferenceMode() == FbxGeometryElement::eIndexToDirect )
				{
					idNormal = fbxNormals->GetIndexArray().GetAt( idNormal );
				}

				if ( fbxTangents->GetMappingMode() == FbxGeometryElement::eByControlPoint )
				{
					idTangent = meshVertexID;
				}

				if ( fbxTangents->GetReferenceMode() == FbxGeometryElement::eIndexToDirect )
				{
					idTangent = fbxTangents->GetIndexArray().GetAt( idTangent );
				}

				FbxVector2 const& fbxUV = fbxUvs->GetDirectArray().GetAt( idUv );
				FbxVector4 const& fbxNormal = fbxNormals->GetDirectArray().GetAt( idNormal );
				FbxVector4 const& fbxTangent = fbxTangents->GetDirectArray().GetAt( idTangent );

				vertex.m_normal.Set( float( fbxNormal.mData[ 0 ] ), float( fbxNormal.mData[ 1 ] ), float( fbxNormal.mData[ 2 ] ) );
				vertex.m_tangnet.Set( float( fbxTangent.mData[ 0 ] ), float( fbxTangent.mData[ 1 ] ), float( fbxTangent.mData[ 2 ] ) );
				vertex.m_uv.Set( float( fbxUV.mData[ 0 ] ), float( fbxUV.mData[ 1 ] ) );

				UINT16 newVertexID = 0;
				UINT16 const verticesNum = UINT16( vertices.Size() );
				for ( ; newVertexID < verticesNum; ++newVertexID )
				{
					if ( vertices[ newVertexID ] == vertex )
					{
						break;
					}
				}

				if ( newVertexID == verticesNum )
				{
					vertex.m_position *= fScale;
					vertices.Add( vertex );
				}

				indices.Add( newVertexID );
				++vertexID;
			}
		}

		if ( vertices.Size() )
		{
			std::fstream meshFile;
			meshFile.open( file, std::fstream::out | std::fstream::trunc | std::fstream::binary );

			UINT16 const verticesNum = UINT16( vertices.Size() );
			UINT16 const indicesNum = UINT16( indices.Size() );
			meshFile.write( reinterpret_cast< char const* >( &verticesNum ), sizeof( verticesNum ) );
			meshFile.write( reinterpret_cast< char const* >( &indicesNum ), sizeof( indicesNum ) );
			meshFile.write( reinterpret_cast< char const* >( vertices.Data() ), sizeof( SSimpleObjectVertexFormat ) * verticesNum );
			meshFile.write( reinterpret_cast< char const* >( indices.Data() ), sizeof( UINT16 ) * indicesNum );

			bool c = meshFile.fail() || meshFile.bad();

			meshFile.close();
		}
	}
#endif
}

void CGeometryLoader::CreateMeshMeta( char const * file, UINT64 const wrtieTime )
{
	std::fstream meshMetaFile;
	meshMetaFile.open( file, std::fstream::out | std::fstream::trunc );
	meshMetaFile.write( reinterpret_cast< char const* >( &GStaticMeshesVersion ), sizeof( GStaticMeshesVersion ) );
	meshMetaFile.write( reinterpret_cast< char const* >( &wrtieTime ), sizeof( wrtieTime ) );
	meshMetaFile.close();
}

void CGeometryLoader::Init()
{
#ifndef FINAL_BUILD
	m_fbxManager = FbxManager::Create();

	m_fbxIOSettings = FbxIOSettings::Create(m_fbxManager, IOSROOT);
	m_fbxIOSettings->SetBoolProp(IMP_FBX_MATERIAL, false);
	m_fbxIOSettings->SetBoolProp(IMP_FBX_TEXTURE, false);
	m_fbxIOSettings->SetBoolProp(IMP_FBX_LINK, false);
	m_fbxIOSettings->SetBoolProp(IMP_FBX_SHAPE, true);
	m_fbxIOSettings->SetBoolProp(IMP_FBX_GOBO, false);
	m_fbxIOSettings->SetBoolProp(IMP_FBX_ANIMATION, false);
	m_fbxIOSettings->SetBoolProp(IMP_FBX_GLOBAL_SETTINGS, false);
	m_fbxManager->SetIOSettings(m_fbxIOSettings);

	m_fbxImporter = FbxImporter::Create( m_fbxManager, "Fbx Importer" );
#endif
}

void CGeometryLoader::LoadMesh( SGeometryData& geometryData, char const * file )
{ 
	size_t const meshFileNameSize = strlen( file ) + 2 + 1;
	char* meshFileName = new char[ meshFileNameSize ];
	sprintf_s( meshFileName, meshFileNameSize, "%s%s", file, ".m" );

#ifndef FINAL_BUILD
	size_t const fbxFileNameSize = strlen( file ) + 4 + 1;
	size_t const meshMetaFileNameSize = strlen( file ) + 7 + 1;
	char* fbxFileName = new char[ fbxFileNameSize ];
	char* meshMetaFileName = new char[ meshMetaFileNameSize ];
	sprintf_s( fbxFileName, fbxFileNameSize, "%s%s", file, ".fbx" );
	sprintf_s( meshMetaFileName, meshMetaFileNameSize, "%s%s", file, ".m.meta" );

	HANDLE fbxFileHandle;
	fbxFileHandle = CreateFileA( fbxFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL );

	if ( fbxFileHandle != INVALID_HANDLE_VALUE )
	{
		FILETIME fbxLastWriteTime;
		GetFileTime( fbxFileHandle, NULL, NULL, &fbxLastWriteTime );
		CloseHandle( fbxFileHandle );
		UINT64 const fbxU64LastWriteTime = UINT64( fbxLastWriteTime.dwHighDateTime ) << UINT64(4 * 8) | UINT64(fbxLastWriteTime.dwLowDateTime);

		bool needCreate = true;
		HANDLE metaFileHandle;
		metaFileHandle = CreateFileA( meshMetaFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL );
		if ( metaFileHandle != INVALID_HANDLE_VALUE )
		{
			UINT64 meshVersion;
			UINT64 metaData;
			ReadFile( metaFileHandle, &meshVersion, sizeof( meshVersion ), NULL, NULL );
			ReadFile( metaFileHandle, &metaData, sizeof( metaData ), NULL, NULL );
			CloseHandle( metaFileHandle );

			if ( metaData == fbxU64LastWriteTime && meshVersion == GStaticMeshesVersion )
			{
				needCreate = false;
			}
		}

		if ( needCreate )
		{
			LoadFbx( fbxFileName, meshFileName, meshMetaFileName, fbxU64LastWriteTime );
		}
	}

	delete[] fbxFileName;
	delete[] meshMetaFileName;
#endif

	std::fstream meshFile;
	meshFile.open( meshFileName, std::fstream::in | std::fstream::binary );
	ASSERT( meshFile.is_open() );

	UINT16 verticesNum;
	UINT16 indicesNum;
	meshFile.read( reinterpret_cast< char* >( &verticesNum ), sizeof( verticesNum ) ); 
	meshFile.read( reinterpret_cast< char* >( &indicesNum ), sizeof( indicesNum ) ); 

	geometryData.m_vertices.Resize( verticesNum );
	geometryData.m_indices.Resize( indicesNum );

	meshFile.read( reinterpret_cast< char* >( geometryData.m_vertices.Data() ), sizeof( SSimpleObjectVertexFormat ) * verticesNum ); 
	bool c = meshFile.eof();
	meshFile.read( reinterpret_cast< char* >( geometryData.m_indices.Data() ), sizeof( UINT16 ) * indicesNum ); 
	std::streamsize x = meshFile.gcount();
	meshFile.close();


	delete[] meshFileName;
}

void CGeometryLoader::LoadFbx( char const* fbxFile, char const* meshFile, char const* meshMetaFile, UINT64 const wrtieTime )
{
#ifndef FINAL_BUILD
	if ( !m_fbxImporter->Initialize( fbxFile, -1, m_fbxIOSettings ) )
	{
		FbxString const error = m_fbxImporter->GetStatus().GetErrorString();
		GTextRenderManager.Print( Vec4( 1.f, 0.f, 0.f, 1.f ), Vec2( 0.f, 1.f - 0.05f ), 0.05f, error.Buffer() );
		return;
	}

	m_fbxScene = FbxScene::Create( m_fbxManager, "Fbx Scene" );
	if ( !m_fbxImporter->Import( m_fbxScene ) )
	{
		FbxString const error = m_fbxImporter->GetStatus().GetErrorString();
		GTextRenderManager.Print( Vec4( 1.f, 0.f, 0.f, 1.f ), Vec2( 0.f, 1.f - 0.05f ), 0.05f, error.Buffer() );
		return;
	}

	FbxGeometryConverter fbxConverter( m_fbxManager );
	fbxConverter.Triangulate( m_fbxScene, true );

	CreateMeshFromFbx( meshFile );
	CreateMeshMeta( meshMetaFile, wrtieTime );
	m_fbxScene->Destroy();
#endif
}

void CGeometryLoader::Release()
{
#ifndef FINAL_BUILD
	m_fbxManager->Destroy();
#endif
}

CGeometryLoader GGeometryLoader;