#include "headers.h"
#include "rendering/render.h"
#include "soundEngine.h"
#include "timer.h"
#include "input.h"
#include "../DirectXTex/DirectXTex.h"

#include "geometryLoader.h"

#include <Windows.h>

CInputManager GInputManager;
CSystemInput GSystemInput;
CTimer GTimer;

SViewObject GViewObject;
SGeometryInfo GGeometryInfo[ G_MAX ];
CStaticSound GSounds[SET_MAX];

int GWidth = 800;
int GHeight = 800;

SComponentHandle testObjectTransformHandle;

//#define DUMMY_PROFILER

#ifdef DUMMY_PROFILER
#include <fstream>
long constexpr GFrameProfNum = 2000;
INT64 GFramesProf[ GFrameProfNum ];
long GFrameProfID = 0;
#endif

void InitGame()
{
	GEntityManager.Clear();

	UINT testEntityID = GEntityManager.CreateEntity();
	CEntity* testEntity = GEntityManager.GetEntity( testEntityID );
	SComponentHandle testCameraHandle = testEntity->AddComponentCamera();
	SComponentCamera* testCamera = &GComponentCameraManager.GetComponent( testCameraHandle );
	GComponentCameraManager.SetMainCamera( testCameraHandle.m_index );
	GComponentCameraManager.SetMainProjection( Matrix4x4::Projection( 40.f, 1.f, 0.01f, 100000.f ) );
	testCamera->m_position = Vec3::ZERO;
	testCamera->m_rotation = Quaternion::IDENTITY;

	SComponentTransform* testObjectTransform = nullptr;
	SComponentLight* testLight = nullptr;

	testEntityID = GEntityManager.CreateEntity();
	testEntity = GEntityManager.GetEntity( testEntityID );
	testObjectTransformHandle = testEntity->AddComponentTransform();
	SComponentHandle testObjectStaticMeshHandle = testEntity->AddComponentStaticMesh();

	testObjectTransform = &GComponentTransformManager.GetComponent( testObjectTransformHandle );
	SComponentStaticMesh& testObjectStaticMesh = GComponentStaticMeshManager.GetComponent( testObjectStaticMeshHandle );

	testObjectTransform->m_position.Set( 0.f, -2.f, 10.f );
	testObjectTransform->m_scale.Set( 10.f, 1.f, 10.f );
	testObjectTransform->m_rotation = Quaternion::IDENTITY;

	testObjectStaticMesh.m_tiling.Set( 10.f, 10.f );
	testObjectStaticMesh.m_geometryInfoID = G_BOX;
	testObjectStaticMesh.m_layer = RL_OPAQUE;
	testObjectStaticMesh.m_shaderID = 0;
	testObjectStaticMesh.m_textureID[ 0 ] = T_PBR_TEST_B;
	testObjectStaticMesh.m_textureID[ 1 ] = T_PBR_TEST_N;
	testObjectStaticMesh.m_textureID[ 2 ] = T_PBR_TEST_R;
	testObjectStaticMesh.m_textureID[ 3 ] = T_PBR_TEST_M;
	testObjectStaticMesh.m_textureID[ 4 ] = T_BLACK;

	GComponentStaticMeshManager.RegisterRenderComponents( testObjectTransformHandle.m_index, testObjectStaticMeshHandle.m_index );
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

	char const* meshes[] =
	{
		"../content/models/spaceship",
		"../content/models/box",
		"../content/models/scene_test",
	};
	CT_ASSERT( ARRAYSIZE( meshes ) == G_MAX );

	wchar_t const* textures[] =
	{
		L"../content/textures/sdf_font_512.png",
		L"../content/textures/common/black.png",
		L"../content/textures/common/ltc_amp.dds",
		L"../content/textures/common/ltc_mat.dds",
		L"../content/textures/spaceship_d.png",
		L"../content/textures/spaceship_n.png",
		L"../content/textures/spaceship_e.png",
		L"../content/textures/spaceship_s.png",
		L"../content/textures/Metal_Crystals_d.dds",
		L"../content/textures/Metal_Crystals_n.dds",
		L"../content/textures/Metal_Crystals_s.dds",
		L"../content/textures/pbr_test_b.dds",
		L"../content/textures/pbr_test_n.dds",
		L"../content/textures/pbr_test_r.dds",
		L"../content/textures/pbr_test_m.dds",
		L"../content/textures/rainDrop.png",
		L"../content/textures/snow.png",
	};
	CT_ASSERT( ARRAYSIZE( textures ) == T_MAX );

	GRender.Init();
	GComponentLightManager.SetDirectLightColor( Vec3( 1.f, 1.f, 1.f ) );
	GComponentLightManager.SetDirectLightDir( Vec3( 0.f, 1.f, 1.f ).GetNormalized() );
	GComponentLightManager.SetAmbientLightColor( Vec3( 0.1f, 0.1f, 0.1f ) );

	GRender.BeginLoadResources(ARRAYSIZE(textures));

	for ( UINT meshID = 0; meshID < ARRAYSIZE( meshes ); ++meshID )
	{
		SGeometryData geometryData;
		GGeometryLoader.LoadMesh( geometryData, meshes[ meshID ] );
		GGeometryInfo[ meshID ].m_geometryID = GRender.LoadResource( geometryData );
		GGeometryInfo[ meshID ].m_indicesNum = geometryData.m_indices.Size();
	}

	for (unsigned int texutreID = 0; texutreID < ARRAYSIZE(textures); ++texutreID)
	{
		STexture texture;
		DirectX::TexMetadata texMeta;
		DirectX::ScratchImage image;
		if ( DirectX::LoadFromWICFile( textures[ texutreID ], DirectX::WIC_FLAGS_NONE, &texMeta, image ) != S_OK )
		{
			CheckResult( DirectX::LoadFromDDSFile( textures[ texutreID ], DirectX::DDS_FLAGS_NONE, &texMeta, image ) );
		}

		texture.m_data = image.GetPixels();
		texture.m_width = UINT(texMeta.width);
		texture.m_height = UINT(texMeta.height);
		texture.m_format = texMeta.format;
		texture.m_mipLevels = Byte(texMeta.mipLevels);

		ASSERT( texture.m_format != DXGI_FORMAT_UNKNOWN );
		GRender.LoadResource(texture);
	}

	GRender.EndLoadResources();

	GInputManager.SetHWND(hwnd);

	GInputManager.Init();
	GInputManager.AddObserver(&GSystemInput);

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

	GRender.WaitForResourcesLoad();

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

		GComponentCameraManager.MainCameraTick();

		if ( GInputManager.KeyDownLastFrame( K_F5 ) )
		{
			GRender.ReinitShaders();
		}
		GInputManager.Tick();

		DrawDebugInfo();

		GRender.PreDrawFrame();
		GRender.DrawFrame();

		GEntityManager.Tick();
	}

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