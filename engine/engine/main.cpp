#include "headers.h"
#include "rendering/render.h"
#include "soundEngine.h"
#include "timer.h"
#include "input.h"
#include "../DirectXTex/DirectXTex.h"

#include "resources/resourceManager.h"
#include "resources/texture.h"
#include "geometryLoader.h"

#include <Windows.h>

CInputManager GInputManager;
CSystemInput GSystemInput;
CTimer GTimer;

SViewObject GViewObject;
CStaticSound GSounds[SET_MAX];

int GWidth = 800;
int GHeight = 800;

//#define DUMMY_PROFILER

#ifdef DUMMY_PROFILER
#include <fstream>
long constexpr GFrameProfNum = 2000;
INT64 GFramesProf[ GFrameProfNum ];
long GFrameProfID = 0;
#endif

static void Map0()
{
	ComponentLightManager::SetDirectLightColor(Vec3(0.6f, 0.6f, 0.6f));
	ComponentLightManager::SetDirectLightDir(Vec3(0.f, 1.f, 1.f).GetNormalized());
	ComponentLightManager::SetAmbientLightColor(Vec3(0.7f, 0.7f, 0.7f));

	GEnvironmentParticleManager.InitParticles(128, 2, 10.f, GTextureResources[L"../content/textures/snow.png"].m_id );

	//floor entity
	{
		UINT const floorEntityID = GEntityManager.CreateEntity();
		CEntity* const pFloorEntity = GEntityManager.GetEntity(floorEntityID);

		SComponentHandle const transformHandle = pFloorEntity->AddComponent(EComponentType::CT_Transform);
		SComponentHandle const staticMeshHandle = pFloorEntity->AddComponent(EComponentType::CT_StaticMesh);
		SComponentHandle const rigidbodyHandle = pFloorEntity->AddComponent(EComponentType::CT_Rigidbody);

		Quaternion rotation = Quaternion::IDENTITY;
		Vec3 position(0.f, -2.f, 10.f);
		Vec3 scale(10.f, 1.f, 10.f);

		//transform
		{
			SComponentTransform& transformComponent = ComponentTransformManager::GetComponent(transformHandle);
			transformComponent.m_position = position;
			transformComponent.m_scale = scale;
			transformComponent.m_rotation = Quaternion::IDENTITY;
		}

		//static mesh
		{
			SComponentStaticMesh& staticMeshComponent = ComponentStaticMeshManager::GetComponent(staticMeshHandle);
			staticMeshComponent.m_tiling.Set(10.f, 10.f);
			staticMeshComponent.m_geometryID = GStaticGeometryResources["../content/models/box"].m_id;
			staticMeshComponent.m_layer = RL_OPAQUE;
			staticMeshComponent.m_shaderID = 0;
			staticMeshComponent.m_textureID[0] = GTextureResources[L"../content/textures/pbr_test_b.dds"].m_id;
			staticMeshComponent.m_textureID[1] = GTextureResources[L"../content/textures/pbr_test_n.dds"].m_id;
			staticMeshComponent.m_textureID[2] = GTextureResources[L"../content/textures/pbr_test_r.dds"].m_id;
			staticMeshComponent.m_textureID[3] = GTextureResources[L"../content/textures/pbr_test_m.dds"].m_id;
			staticMeshComponent.m_textureID[4] = GTextureResources[L"../content/textures/common/black.png"].m_id;
		}

		//rigidbody
		{
			PhysicsManager::PhysicsObjectDef physicsObjectDef;
			physicsObjectDef.position = position;
			physicsObjectDef.rotation = rotation;
			physicsObjectDef.bodyDef.bodyType = PhysicsManager::PBT_Box;
			physicsObjectDef.bodyDef.boxHalfExtents = scale;
			physicsObjectDef.mass = 0.f;

			SComponentRigidbody &rigidbodyComponent = ComponentRigidbodyManager::GetComponent(rigidbodyHandle);
			rigidbodyComponent.bodyPtr = PhysicsManager::AddRigidbody(physicsObjectDef);
		}

		ComponentStaticMeshManager::RegisterRenderComponents(transformHandle, staticMeshHandle);
	}

	//rigid0 entity
	{
		UINT const entityID = GEntityManager.CreateEntity();
		CEntity* const pEntity = GEntityManager.GetEntity(entityID);

		SComponentHandle const transformHandle = pEntity->AddComponent(EComponentType::CT_Transform);
		SComponentHandle const staticMeshHandle = pEntity->AddComponent(EComponentType::CT_StaticMesh);
		SComponentHandle const rigidbodyHandle = pEntity->AddComponent(EComponentType::CT_Rigidbody);

		Quaternion rotation = Quaternion::FromAxisAngle(Vec3(1.f,1.f,1.f).GetNormalized(), 45.f * MathConsts::DegToRad);
		Vec3 position(0.f, 4.f, 5.f);
		Vec3 scale(.5f, .5f, .5f);

		//transform
		{
			SComponentTransform& transformComponent = ComponentTransformManager::GetComponent(transformHandle);
			transformComponent.m_position = position;
			transformComponent.m_scale = scale;
			transformComponent.m_rotation = Quaternion::IDENTITY;
		}

		//static mesh
		{
			SComponentStaticMesh& staticMeshComponent = ComponentStaticMeshManager::GetComponent(staticMeshHandle);
			staticMeshComponent.m_tiling.Set(1.f, 1.f);
			staticMeshComponent.m_geometryID = GStaticGeometryResources["../content/models/box"].m_id;
			staticMeshComponent.m_layer = RL_OPAQUE;
			staticMeshComponent.m_shaderID = 0;
			staticMeshComponent.m_textureID[0] = GTextureResources[L"../content/textures/pbr_test_b.dds"].m_id;
			staticMeshComponent.m_textureID[1] = GTextureResources[L"../content/textures/pbr_test_n.dds"].m_id;
			staticMeshComponent.m_textureID[2] = GTextureResources[L"../content/textures/pbr_test_r.dds"].m_id;
			staticMeshComponent.m_textureID[3] = GTextureResources[L"../content/textures/pbr_test_m.dds"].m_id;
			staticMeshComponent.m_textureID[4] = GTextureResources[L"../content/textures/common/black.png"].m_id;
		}

		//rigidbody
		{
			PhysicsManager::PhysicsObjectDef physicsObjectDef;
			physicsObjectDef.position = position;
			physicsObjectDef.rotation = rotation;
			physicsObjectDef.bodyDef.bodyType = PhysicsManager::PBT_Box;
			physicsObjectDef.bodyDef.boxHalfExtents = scale;
			physicsObjectDef.mass = 1.f;

			SComponentRigidbody &rigidbodyComponent = ComponentRigidbodyManager::GetComponent(rigidbodyHandle);
			rigidbodyComponent.bodyPtr = PhysicsManager::AddRigidbody(physicsObjectDef);
		}

		ComponentStaticMeshManager::RegisterRenderComponents(transformHandle, staticMeshHandle);
		ComponentRigidbodyManager::RegisterPhysicsComponents(transformHandle, rigidbodyHandle);
	}

	//rigid1 entity
	{
		UINT const entityID = GEntityManager.CreateEntity();
		CEntity* const pEntity = GEntityManager.GetEntity(entityID);

		SComponentHandle const transformHandle = pEntity->AddComponent(EComponentType::CT_Transform);
		SComponentHandle const staticMeshHandle = pEntity->AddComponent(EComponentType::CT_StaticMesh);
		SComponentHandle const rigidbodyHandle = pEntity->AddComponent(EComponentType::CT_Rigidbody);

		Quaternion rotation = Quaternion::IDENTITY;
		Vec3 position(0.f, 5.f, 5.f);
		Vec3 scale(.5f, .5f, .5f);

		//transform
		{
			SComponentTransform& transformComponent = ComponentTransformManager::GetComponent(transformHandle);
			transformComponent.m_position = position;
			transformComponent.m_scale = scale;
			transformComponent.m_rotation = Quaternion::IDENTITY;
		}

		//static mesh
		{
			SComponentStaticMesh& staticMeshComponent = ComponentStaticMeshManager::GetComponent(staticMeshHandle);
			staticMeshComponent.m_tiling.Set(1.f, 1.f);
			staticMeshComponent.m_geometryID = GStaticGeometryResources["../content/models/sphere"].m_id;
			staticMeshComponent.m_layer = RL_OPAQUE;
			staticMeshComponent.m_shaderID = 0;
			staticMeshComponent.m_textureID[0] = GTextureResources[L"../content/textures/pbr_test_b.dds"].m_id;
			staticMeshComponent.m_textureID[1] = GTextureResources[L"../content/textures/pbr_test_n.dds"].m_id;
			staticMeshComponent.m_textureID[2] = GTextureResources[L"../content/textures/pbr_test_r.dds"].m_id;
			staticMeshComponent.m_textureID[3] = GTextureResources[L"../content/textures/pbr_test_m.dds"].m_id;
			staticMeshComponent.m_textureID[4] = GTextureResources[L"../content/textures/common/black.png"].m_id;
		}

		//rigidbody
		{
			PhysicsManager::PhysicsObjectDef physicsObjectDef;
			physicsObjectDef.position = position;
			physicsObjectDef.rotation = rotation;
			physicsObjectDef.bodyDef.bodyType = PhysicsManager::PBT_Sphere;
			physicsObjectDef.bodyDef.sphereRadius = scale.x;
			physicsObjectDef.mass = 1.f;

			SComponentRigidbody &rigidbodyComponent = ComponentRigidbodyManager::GetComponent(rigidbodyHandle);
			rigidbodyComponent.bodyPtr = PhysicsManager::AddRigidbody(physicsObjectDef);
		}

		ComponentStaticMeshManager::RegisterRenderComponents(transformHandle, staticMeshHandle);
		ComponentRigidbodyManager::RegisterPhysicsComponents(transformHandle, rigidbodyHandle);
	}

	//light 0
	{
		UINT const lightEntityID = GEntityManager.CreateEntity();
		CEntity* const pLightEntity = GEntityManager.GetEntity(lightEntityID);

		SComponentHandle const transformHandle = pLightEntity->AddComponent(EComponentType::CT_Transform);
		SComponentHandle const staticMeshHandle = pLightEntity->AddComponent(EComponentType::CT_StaticMesh);
		SComponentHandle const lightHandle = pLightEntity->AddComponent(EComponentType::CT_Light);

		//transform
		{
			SComponentTransform& transformComponent = ComponentTransformManager::GetComponent(transformHandle);
			transformComponent.m_position.Set(-1.5f, 0.5f, 10.f);
			transformComponent.m_scale.Set(1.f, 1.f, 1.f);
			transformComponent.m_rotation = Quaternion::FromAxisAngle(Vec3::RIGHT.data, -90.f * MathConsts::DegToRad);
		}

		//static mesh
		{
			SComponentStaticMesh& staticMeshComponent = ComponentStaticMeshManager::GetComponent(staticMeshHandle);
			staticMeshComponent.m_color.Set(1.f, 1.f, 1.f);
			staticMeshComponent.m_tiling.Set(1.f, 1.f);
			staticMeshComponent.m_geometryID = GStaticGeometryResources["../content/models/plane"].m_id;
			staticMeshComponent.m_layer = RL_UNLIT;
			staticMeshComponent.m_shaderID = Byte(ST_OBJECT_DRAW_SIMPLE);
			memset(staticMeshComponent.m_textureID, 0, sizeof(staticMeshComponent.m_textureID));
		}

		//light
		{
			SComponentLight& lightComponent = ComponentLightManager::GetComponent(lightHandle);
			lightComponent.m_color.Set(2.f, 2.f, 2.f);
			lightComponent.m_textureID = 0;
			lightComponent.m_lightShader = Byte(ELightFlags::LF_LTC);
		}

		ComponentStaticMeshManager::RegisterRenderComponents(transformHandle, staticMeshHandle);
		ComponentLightManager::RegisterRenderComponents(transformHandle, lightHandle);
	}

	//light 1 (texture)
	{
		UINT const lightEntityID = GEntityManager.CreateEntity();
		CEntity* const pLightEntity = GEntityManager.GetEntity(lightEntityID);

		SComponentHandle const transformHandle = pLightEntity->AddComponent(EComponentType::CT_Transform);
		SComponentHandle const staticMeshHandle = pLightEntity->AddComponent(EComponentType::CT_StaticMesh);
		SComponentHandle const lightHandle = pLightEntity->AddComponent(EComponentType::CT_Light);

		//transform
		{
			SComponentTransform& transformComponent = ComponentTransformManager::GetComponent(transformHandle);
			transformComponent.m_position.Set(5.5f, 0.5f, 10.f);
			transformComponent.m_scale.Set(1.f, 1.f, 1.f);
			transformComponent.m_rotation = Quaternion::FromAxisAngle(Vec3::RIGHT.data, -90.f * MathConsts::DegToRad);
		}

		//static mesh
		{
			SComponentStaticMesh& staticMeshComponent = ComponentStaticMeshManager::GetComponent(staticMeshHandle);
			staticMeshComponent.m_color.Set(1.f, 1.f, 1.f);
			staticMeshComponent.m_tiling.Set(1.f, 1.f);
			staticMeshComponent.m_geometryID = GStaticGeometryResources["../content/models/plane"].m_id;
			staticMeshComponent.m_layer = RL_UNLIT;
			staticMeshComponent.m_shaderID = Byte(ST_OBJECT_DRAW_SIMPLE_TEXTURE);
			memset(staticMeshComponent.m_textureID, 0, sizeof(staticMeshComponent.m_textureID));
			staticMeshComponent.m_textureID[0] = GTextureResources[L"../content/textures/stained_glass.dds"].m_id;
		}

		//light
		{
			SComponentLight& lightComponent = ComponentLightManager::GetComponent(lightHandle);
			lightComponent.m_color.Set(2.f, 2.f, 2.f);
			lightComponent.m_textureID = GTextureResources[L"../content/textures/stained_glass.ltc.dds"].m_id;
			lightComponent.m_lightShader = Byte(ELightFlags::LF_LTC_TEXTURE);
		}

		ComponentStaticMeshManager::RegisterRenderComponents(transformHandle, staticMeshHandle);
		ComponentLightManager::RegisterRenderComponents(transformHandle, lightHandle);
	}
}

extern AddComponentFunc AddComponentFuncPtr[CT_Num];
extern RemoveComponentFunc RemoveComponentFuncPtr[CT_Num];

void InitGame()
{
	memset(AddComponentFuncPtr, 0, sizeof(AddComponentFuncPtr));
	memset(RemoveComponentFuncPtr, 0, sizeof(RemoveComponentFuncPtr));

	AddComponentFuncPtr[CT_Transform] = ComponentTransformManager::AddComponent;
	AddComponentFuncPtr[CT_StaticMesh] = ComponentStaticMeshManager::AddComponent;
	AddComponentFuncPtr[CT_Light] = ComponentLightManager::AddComponent;
	AddComponentFuncPtr[CT_Camera] = ComponentCameraManager::AddComponent;
	AddComponentFuncPtr[CT_Rigidbody] = ComponentRigidbodyManager::AddComponent;

	RemoveComponentFuncPtr[CT_Transform] = ComponentTransformManager::RemoveComponent;
	RemoveComponentFuncPtr[CT_StaticMesh] = ComponentStaticMeshManager::RemoveComponent;
	RemoveComponentFuncPtr[CT_Light] = ComponentLightManager::RemoveComponent;
	RemoveComponentFuncPtr[CT_Camera] = ComponentCameraManager::RemoveComponent;
	RemoveComponentFuncPtr[CT_Rigidbody] = ComponentRigidbodyManager::RemoveComponent;

	GEntityManager.Clear();

	//camera entity
	{
		UINT const cameraEntityID = GEntityManager.CreateEntity();
		CEntity* const pCameraEntity = GEntityManager.GetEntity( cameraEntityID );
		SComponentHandle const cameraHandle = pCameraEntity->AddComponent( EComponentType::CT_Camera );
		SComponentCamera& cameraComponent = ComponentCameraManager::GetComponent( cameraHandle );
		ComponentCameraManager::SetMainCamera( cameraHandle );
		ComponentCameraManager::SetMainProjection( Matrix4x4::Projection( 40.f, 1.f, 0.01f, 100000.f ) );
		cameraComponent.m_position = Vec3::ZERO;
		cameraComponent.m_rotation = Quaternion::IDENTITY;
	}

	Map0();
	//Map1();
}

void DrawDebugInfo()
{
	char fpsText[ 15 ];
	sprintf_s( fpsText, 15, "FPS:%.2f", 1.f / GTimer.Delta() );
	GTextRenderManager.Print( Vec4( 1.f, 0.5f, 0.f, 1.f ), Vec2( 0.f, 1.f - 0.025f ), 0.05f, fpsText );
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pCmdLine, INT nCmdShow)
{
	srand(time(NULL));
	WNDCLASS windowClass = { 0 };

	windowClass.style = CS_HREDRAW | CS_VREDRAW;
	windowClass.lpfnWndProc = CInputManager::WindowProc;
	windowClass.hInstance = hInstance;
	windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	windowClass.lpszClassName = L"WindowClass";
	windowClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);

	if (!RegisterClass(&windowClass))
	{
		MessageBox(0, L"RegisterClass FAILED", 0, 0);
	}

	HWND hwnd = CreateWindow(
		L"WindowClass",
		L"SandBox",
		WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU,
		CW_USEDEFAULT, CW_USEDEFAULT,
		GWidth, GHeight,
		NULL, NULL,
		hInstance,
		NULL);

	if (hwnd == 0)
	{
		MessageBox(0, L"CreateWindow FAILED", 0, 0);
	}
	
	ShowWindow(hwnd, nCmdShow);

	RECT clientRect;
	GetClientRect(hwnd, &clientRect);
	GWidth = clientRect.right - clientRect.left;
	GHeight = clientRect.bottom - clientRect.top;

	GSoundEngine.Init();
	GGeometryLoader.Init();

	GRender.SetWindowWidth(GWidth);
	GRender.SetWindowHeight(GHeight);
	GRender.SetHWND(hwnd);

	GRender.Init();

	GInputManager.SetHWND(hwnd);

	GInputManager.Init();
	GInputManager.AddObserver(&GSystemInput);

	PhysicsManager::Init();

	InitGame();

	char const* sounds[] =
	{
		"../content/sounds/shoot0.wav",
		"../content/sounds/shoot1.wav",
		"../content/sounds/shoot2.wav",
		"../content/sounds/heal.wav",
		"../content/sounds/explosion.wav",
		"../content/sounds/build.wav",
	};

	for (unsigned int soundID = 0; soundID < SET_MAX; ++soundID)
	{
		GSounds[soundID] = GSoundEngine.CreateStaticSound(sounds[soundID]);
	}

#if defined( CREATE_LTC_TEX )
	{
		UINT16 const textureID = GTextureResources[ L"../content/textures/stained_glass.dds" ].m_id;
		STextureInfo const textureInfo = GRender.GetTextureInfo( textureID );

		DirectX::ScratchImage ltcTex;
		ltcTex.Initialize2D( textureInfo.m_format, textureInfo.m_width, textureInfo.m_height, 1, textureInfo.m_mipLevels );

		GRender.CreateLTCTexture( textureID, ltcTex.GetPixels() );

		DirectX::SaveToDDSFile( ltcTex.GetImages(), ltcTex.GetImageCount(), ltcTex.GetMetadata(), 0, L"../content/textures/stained_glass.ltc.dds" );
	}
#endif

	MSG msg = { 0 };
	bool run = true;
	while (run)
	{
		GTimer.Tick();
#ifdef DUMMY_PROFILER
		GFramesProf[ GFrameProfID % GFrameProfNum] = GTimer.LastDelta();
		++GFrameProfID;
#endif

		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
			if (msg.message == WM_QUIT)
			{
				run = false;
				break;
			}
		}

		if (GInputManager.KeyDownLastFrame(K_F5))
		{
			GRender.ReinitShaders();
		}

		PhysicsManager::Tick();
		ComponentRigidbodyManager::SyncTransformsWithPhysics();
		ComponentCameraManager::MainCameraTick();
		
		GInputManager.Tick();

		//DrawDebugInfo();

		GRender.PreDrawFrame();
		GRender.DrawFrame();

		GEntityManager.Tick();
	}

	PhysicsManager::Release();
	GRender.Release();
	GSoundEngine.Release();
	GGeometryLoader.Release();

#ifdef DUMMY_PROFILER
	time_t rawTime;
	tm timeInfo;
	char profileFileName[ 256 ];
	time( &rawTime );
	localtime_s( &timeInfo, &rawTime );
	strftime( profileFileName, 256, "profiling/dummyProfiler %d-%m-%Y %Ih%Mm%Ss.csv", &timeInfo );

	std::fstream profileFile;
	profileFile.open( profileFileName, std::fstream::out | std::fstream::trunc );
	if ( profileFile.is_open() )
	{
		profileFile << "frameID,time,fps\n";
		char profText[ 256 ];
		int const start = max( 0, GFrameProfID - GFrameProfNum );
		INT64 iMin = INT64_MAX, iMax = 0, iSum = 0;
		float fMin = 100000.f, fMax = 0.f, fSum = 0.f;
		for ( int i = start; i < GFrameProfID; ++i )
		{
			INT64 const time = GFramesProf[ i % GFrameProfNum ];
			float const fps = 1.f / GTimer.GetSeconds( time );
			iMin = min( time, iMin );
			iMax = max( time, iMax );
			iSum += time;

			fMin = min( fps, fMin );
			fMax = max( fps, fMax );
			fSum += fps;

			sprintf_s( profText, 256, "%i,%lli,%.4f\n", i - start, time, fps );
			profileFile << profText;
		}

		sprintf_s( profText, 256, "\nmin,max,avg\n" );
		profileFile << profText;
		sprintf_s( profText, 256, "%lli,%lli,%.4f\n", iMin, iMax, static_cast<float>(iSum ) / static_cast<float>( GFrameProfID - start ) );
		profileFile << profText;
		sprintf_s( profText, 256, "%.4f,%.4f,%.4f\n", fMin, fMax, static_cast<float>(fSum ) / static_cast<float>( GFrameProfID - start ) );
		profileFile << profText;

		profileFile.close();
	}
#endif

	return 0;
}