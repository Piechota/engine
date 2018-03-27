#pragma once
#include "rendering/geometry.h"

namespace fbxsdk
{
	class FbxManager;
	class FbxImporter;
	class FbxIOSettings;
	class FbxScene;
	class FbxMesh;
}

class CGeometryLoader
{
private:
#ifndef FINAL_BUILD
	fbxsdk::FbxManager*			m_fbxManager;
	fbxsdk::FbxImporter*		m_fbxImporter;
	fbxsdk::FbxIOSettings*		m_fbxIOSettings;
	fbxsdk::FbxScene*			m_fbxScene;
#endif

private:
	void CreateMeshFromFbx( char const* file );
	void CreateMeshMeta( char const* file, UINT64 const wrtieTime );
	void LoadFbx( char const* fbxFile, char const* meshFile, char const* meshMetaFile, UINT64 const wrtieTime );

public:
	void Init();
	void LoadMesh( SGeometryData& geometryData, char const* file );
	void Release();
};

extern CGeometryLoader GGeometryLoader;
