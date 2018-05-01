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
SGeometryInfo GGeometryInfo[ G_MAX ];
CStaticSound GSounds[SET_MAX];

int GWidth = 800;
int GHeight = 800;

SComponentHandle testObjectTransformHandle;
SComponentHandle ltcLightTest;

//#define DUMMY_PROFILER

#ifdef DUMMY_PROFILER
#include <fstream>
long constexpr GFrameProfNum = 2000;
INT64 GFramesProf[ GFrameProfNum ];
long GFrameProfID = 0;
#endif


void TestTextureMipMap2D( Byte* const dstBegin, int const dstWidth, int const dstHeight, Byte* const srcBegin, int const srcWidth, int const srcHeight, int const inTexKernelRadius )
{
	int const margSize = dstWidth / 4;

	int const texCoordX0 = margSize;
	int const texCoordY0 = margSize;
	int const texCoordX1 = dstWidth - margSize;
	int const texCoordY1 = dstHeight - margSize;

	int const maxKernelDim = max( 4 * margSize + 1, 2 * inTexKernelRadius + 1 );
	int const maxKernelRadius = ( maxKernelDim - 1 ) / 2;

	float* kernel = new float[ maxKernelDim * maxKernelDim ];
	Math::GaussianKelner2D( kernel, 500.f, int( maxKernelRadius ) );

	for ( int y = 0; y < dstHeight; ++y )
	{
		int const distanceY = max( 0, max( texCoordY0 - y, y - texCoordY1 + 1 ) );
		for ( int x = 0; x < dstWidth; ++x )
		{
			int const distanceX = max( 0, max( texCoordX0 - x, x - texCoordX1 + 1 ) );
			int const distance = max( distanceX, distanceY );

			int const kernelRadius = max( inTexKernelRadius, int(float(distance * 2.f)) );
			int const kernelDim = 2 * kernelRadius + 1;
			int const kernelSize = kernelDim * kernelDim;

			int const kernelOffset = maxKernelRadius - kernelRadius;

			int const sampleX0 = ( x < kernelRadius ? 0 : ( x - kernelRadius ) );
			int const sampleY0 = ( y < kernelRadius ? 0 : ( y - kernelRadius ) );

			int const sampleX1 = ( min( dstWidth, x + kernelRadius + 1 ) );
			int const sampleY1 = ( min( dstHeight, y + kernelRadius + 1 ) );

			int const kernelX0 = kernelRadius + sampleX0 - x;
			int const kernelY0 = kernelRadius + sampleY0 - y;

			int const kernelX1 = kernelRadius + sampleX1 - x;
			int const kernelY1 = kernelRadius + sampleY1 - y;

			float sum = 0.f;

			float accR = 0.f;
			float accG = 0.f;
			float accB = 0.f;

			for ( int currentY = sampleY0; currentY < sampleY1; ++currentY )
			{
				int const srcSampleY = max( 0, min( srcWidth - 1, currentY - texCoordY0 ) );
				for ( int currentX = sampleX0; currentX < sampleX1; ++currentX )
				{
					int const srcSampleX = max( 0, min( srcWidth - 1, currentX - texCoordX0 ) );

					float const srcR = float( srcBegin[ ( srcSampleY * srcWidth + srcSampleX ) * 4 + 0 ] );
					float const srcG = float( srcBegin[ ( srcSampleY * srcWidth + srcSampleX ) * 4 + 1 ] );
					float const srcB = float( srcBegin[ ( srcSampleY * srcWidth + srcSampleX ) * 4 + 2 ] );
					float const kernelVal = kernel[ ( kernelOffset + kernelY0 + currentY - sampleY0 ) * maxKernelDim + kernelOffset + kernelX0 + currentX - sampleX0 ];
					accR += srcR * kernelVal;
					accG += srcG * kernelVal;
					accB += srcB * kernelVal;

					sum += kernelVal;
				}
			}

			dstBegin[ ( y * dstWidth + x ) * 4 + 0 ] = Byte( min( 255.f, accR / sum ) );
			dstBegin[ ( y * dstWidth + x ) * 4 + 1 ] = Byte( min( 255.f, accG / sum ) );
			dstBegin[ ( y * dstWidth + x ) * 4 + 2 ] = Byte( min( 255.f, accB / sum ) );
			dstBegin[ ( y * dstWidth + x ) * 4 + 3 ] = 0xFF;
		}
	}

	delete[] kernel;
}

void TestTextureMipMap1D( Byte* const dstBegin, int const dstWidth, int const dstHeight, Byte* const srcBegin, int const srcWidth, int const srcHeight, int const inTexKernelRadius )
{
	int const margSize = dstWidth / 4;

	int const texCoordX0 = margSize;
	int const texCoordY0 = margSize;
	int const texCoordX1 = dstWidth - margSize;
	int const texCoordY1 = dstHeight - margSize;

	int const maxKernelDim = max( 4 * margSize + 1, 2 * inTexKernelRadius + 1 );
	int const maxKernelRadius = ( maxKernelDim - 1 ) / 2;

	float* kernel = new float[ maxKernelDim ];
	Math::GaussianKelner1D( kernel, 500.f, int( maxKernelRadius ) );

	Byte* const blurHorizontal = new Byte[ dstWidth * 3 * dstHeight ];

	for ( int y = 0; y < dstHeight; ++y )
	{
		int const distanceY = max( 0, max( texCoordY0 - y, y - texCoordY1 + 1 ) );
		for ( int x = 0; x < dstWidth; ++x )
		{
			int const distanceX = max( 0, max( texCoordX0 - x, x - texCoordX1 + 1 ) );
			int const distance = max( distanceX, distanceY );

			int const kernelRadius = max( inTexKernelRadius, int(float(distance * 2.f)) );
			int const kernelDim = 2 * kernelRadius + 1;

			int const kernelOffset = maxKernelRadius - kernelRadius;

			int const sampleX0 = ( x < kernelRadius ? 0 : ( x - kernelRadius ) );
			int const sampleY0 = y;// min( texCoordY1 - 1, max( texCoordY0, y ) );

			int const sampleX1 = ( min( dstWidth, x + kernelRadius + 1 ) );

			int const kernelX0 = kernelRadius + sampleX0 - x;

			//UINT const srcSampleX0 = sampleX0 - margSize;
			//UINT const srcSampleY0 = sampleY0 - margSize;
			//
			//UINT const srcSampleX1 = sampleX1 - margSize;

			float sum = 0.f;

			float accR = 0.f;
			float accG = 0.f;
			float accB = 0.f;

			for ( int currentX = sampleX0; currentX < sampleX1; ++currentX )
			{
				int const srcSampleX = min( srcWidth - 1, max( 0, currentX - texCoordX0 ) );
				int const srcSampleY = min( srcHeight - 1, max( 0, sampleY0 - texCoordY0 ) );

				float const srcR = float( srcBegin[ ( srcSampleY * srcWidth + srcSampleX ) * 4 + 0 ] );
				float const srcG = float( srcBegin[ ( srcSampleY * srcWidth + srcSampleX ) * 4 + 1 ] );
				float const srcB = float( srcBegin[ ( srcSampleY * srcWidth + srcSampleX ) * 4 + 2 ] );
				float const kernelVal = kernel[ kernelOffset + kernelX0 + currentX - sampleX0 ];
				accR += srcR * kernelVal;
				accG += srcG * kernelVal;
				accB += srcB * kernelVal;

				sum += kernelVal;
			}

			blurHorizontal[ ( x * dstHeight + y ) * 3 + 0 ] = Byte( min( 255.f, accR / sum ) );
			blurHorizontal[ ( x * dstHeight + y ) * 3 + 1 ] = Byte( min( 255.f, accG / sum ) );
			blurHorizontal[ ( x * dstHeight + y ) * 3 + 2 ] = Byte( min( 255.f, accB / sum ) );
		}
	}

	for ( int y = 0; y < dstHeight; ++y )
	{
		int const distanceY = max( 0, max( texCoordY0 - y, y - texCoordY1 + 1 ) );
		for ( int x = 0; x < dstWidth; ++x )
		{
			int const distanceX = max( 0, max( texCoordX0 - x, x - texCoordX1 + 1 ) );
			int const distance = max( distanceX, distanceY );

			int const kernelRadius = max( inTexKernelRadius, int(float(distance * 2.f)) );
			int const kernelDim = 2 * kernelRadius + 1;

			int const kernelOffset = maxKernelRadius - kernelRadius;

			int const sampleX0 = (  x < kernelRadius ? 0 : ( x - kernelRadius ) );
			int const sampleY0 = y;

			int const sampleX1 = ( min( dstWidth, x + kernelRadius + 1 ) );

			int const kernelX0 = kernelRadius + sampleX0 - x;
			int const kernelX1 = kernelRadius + sampleX1 - x;

			float sum = 0.f;

			float accR = 0.f;
			float accG = 0.f;
			float accB = 0.f;

			for ( int currentX = sampleX0; currentX < sampleX1; ++currentX )
			{
				float const srcR = float( blurHorizontal[ ( sampleY0 * dstWidth + currentX ) * 3 + 0 ] );
				float const srcG = float( blurHorizontal[ ( sampleY0 * dstWidth + currentX ) * 3 + 1 ] );
				float const srcB = float( blurHorizontal[ ( sampleY0 * dstWidth + currentX ) * 3 + 2 ] );
				float const kernelVal = kernel[ kernelOffset + kernelX0 + currentX - sampleX0 ];
				accR += srcR * kernelVal;
				accG += srcG * kernelVal;
				accB += srcB * kernelVal;

				sum += kernelVal;
			}

			dstBegin[ ( x * dstHeight + y ) * 4 + 0 ] = Byte( min( 255.f, accR / sum ) );
			dstBegin[ ( x * dstHeight + y ) * 4 + 1 ] = Byte( min( 255.f, accG / sum ) );
			dstBegin[ ( x * dstHeight + y ) * 4 + 2 ] = Byte( min( 255.f, accB / sum ) );

			dstBegin[ ( x * dstHeight + y ) * 4 + 3 ] = 0xFF;
		}
	}

	delete[] kernel;
	delete[] blurHorizontal;
}


void InitGame()
{
	GEntityManager.Clear();

	//camera entity
	{
		UINT const cameraEntityID = GEntityManager.CreateEntity();
		CEntity* const pCameraEntity = GEntityManager.GetEntity( cameraEntityID );
		SComponentHandle const cameraHandle = pCameraEntity->AddComponent( EComponentType::CT_Camera );
		SComponentCamera& cameraComponent = GComponentCameraManager.GetComponent( cameraHandle );
		GComponentCameraManager.SetMainCamera( cameraHandle );
		GComponentCameraManager.SetMainProjection( Matrix4x4::Projection( 40.f, 1.f, 0.01f, 100000.f ) );
		cameraComponent.m_position = Vec3::ZERO;
		cameraComponent.m_rotation = Quaternion::IDENTITY;
	}

	//floor entity
	{
		UINT const floorEntityID = GEntityManager.CreateEntity();
		CEntity* const pFloorEntity = GEntityManager.GetEntity( floorEntityID );

		SComponentHandle const transformHandle = pFloorEntity->AddComponent( EComponentType::CT_Transform );
		SComponentHandle const staticMeshHandle = pFloorEntity->AddComponent( EComponentType::CT_StaticMesh );

		//transform
		{
			SComponentTransform& transformComponent = GComponentTransformManager.GetComponent( transformHandle );
			transformComponent.m_position.Set( 0.f, -2.f, 10.f );
			transformComponent.m_scale.Set( 10.f, 1.f, 10.f );
			transformComponent.m_rotation = Quaternion::IDENTITY;
		}

		//static mesh
		{
			SComponentStaticMesh& staticMeshComponent = GComponentStaticMeshManager.GetComponent( staticMeshHandle );
			staticMeshComponent.m_tiling.Set( 10.f, 10.f );
			staticMeshComponent.m_geometryInfoID = G_BOX;
			staticMeshComponent.m_layer = RL_OPAQUE;
			staticMeshComponent.m_shaderID = 0;
			staticMeshComponent.m_textureID[ 0 ] = GTextureResources[ L"../content/textures/pbr_test_b.dds" ];
			staticMeshComponent.m_textureID[ 1 ] = GTextureResources[ L"../content/textures/pbr_test_n.dds" ];
			staticMeshComponent.m_textureID[ 2 ] = GTextureResources[ L"../content/textures/pbr_test_r.dds" ];
			staticMeshComponent.m_textureID[ 3 ] = GTextureResources[ L"../content/textures/pbr_test_m.dds" ];
			staticMeshComponent.m_textureID[ 4 ] = GTextureResources[ L"../content/textures/common/black.png" ];
		}

		GComponentStaticMeshManager.RegisterRenderComponents( transformHandle, staticMeshHandle );
	}

	//light 0
	{
		UINT const lightEntityID = GEntityManager.CreateEntity();
		CEntity* const pLightEntity = GEntityManager.GetEntity( lightEntityID );

		SComponentHandle const transformHandle = pLightEntity->AddComponent( EComponentType::CT_Transform );
		SComponentHandle const staticMeshHandle = pLightEntity->AddComponent( EComponentType::CT_StaticMesh );
		SComponentHandle const lightHandle = pLightEntity->AddComponent( EComponentType::CT_Light );

		//transform
		{
			SComponentTransform& transformComponent = GComponentTransformManager.GetComponent( transformHandle );
			transformComponent.m_position.Set( -1.5f, 0.5f, 10.f );
			transformComponent.m_scale.Set( 1.f, 1.f, 1.f );
			transformComponent.m_rotation = Quaternion::FromAxisAngle( Vec3::RIGHT.data, -90.f * MathConsts::DegToRad );
		}

		//static mesh
		{
			SComponentStaticMesh& staticMeshComponent = GComponentStaticMeshManager.GetComponent( staticMeshHandle );
			staticMeshComponent.m_color.Set( 1.f, 1.f, 1.f );
			staticMeshComponent.m_tiling.Set( 1.f, 1.f );
			staticMeshComponent.m_geometryInfoID = G_PLANE;
			staticMeshComponent.m_layer = RL_UNLIT;
			staticMeshComponent.m_shaderID = Byte(ST_OBJECT_DRAW_SIMPLE);
			memset( staticMeshComponent.m_textureID, 0, sizeof( staticMeshComponent.m_textureID ) );
		}

		//light
		{
			SComponentLight& lightComponent = GComponentLightManager.GetComponent( lightHandle );
			lightComponent.m_color.Set( 2.f, 2.f, 2.f );
			lightComponent.m_textureID = 0;
			lightComponent.m_lightShader = Byte( ELightFlags::LF_LTC );
		}

		GComponentStaticMeshManager.RegisterRenderComponents( transformHandle, staticMeshHandle );
		GComponentLightManager.RegisterRenderComponents( transformHandle, lightHandle );
	}

	//light 1 (texture)
	{
		UINT const lightEntityID = GEntityManager.CreateEntity();
		CEntity* const pLightEntity = GEntityManager.GetEntity( lightEntityID );

		SComponentHandle const transformHandle = pLightEntity->AddComponent( EComponentType::CT_Transform );
		SComponentHandle const staticMeshHandle = pLightEntity->AddComponent( EComponentType::CT_StaticMesh );
		SComponentHandle const lightHandle = pLightEntity->AddComponent( EComponentType::CT_Light );

		//transform
		{
			SComponentTransform& transformComponent = GComponentTransformManager.GetComponent( transformHandle );
			transformComponent.m_position.Set(  5.5f, 0.5f, 10.f );
			transformComponent.m_scale.Set( 1.f, 1.f, 1.f );
			transformComponent.m_rotation = Quaternion::FromAxisAngle( Vec3::RIGHT.data, -90.f * MathConsts::DegToRad );
		}

		//static mesh
		{
			SComponentStaticMesh& staticMeshComponent = GComponentStaticMeshManager.GetComponent( staticMeshHandle );
			staticMeshComponent.m_color.Set( 1.f, 1.f, 1.f );
			staticMeshComponent.m_tiling.Set( 1.f, 1.f );
			staticMeshComponent.m_geometryInfoID = G_PLANE;
			staticMeshComponent.m_layer = RL_UNLIT;
			staticMeshComponent.m_shaderID = Byte(ST_OBJECT_DRAW_SIMPLE);
			memset( staticMeshComponent.m_textureID, 0, sizeof( staticMeshComponent.m_textureID ) );
			staticMeshComponent.m_textureID[ 0 ] = GTextureResources[ L"../content/textures/stained_glass.dds" ];
		}

		//light
		{
			SComponentLight& lightComponent = GComponentLightManager.GetComponent( lightHandle );
			lightComponent.m_color.Set( 2.f, 2.f, 2.f );
			lightComponent.m_textureID = GTextureResources[ L"../content/textures/stained_glass_test.dds" ];
			lightComponent.m_lightShader = Byte( ELightFlags::LF_LTC_TEXTURE );
		}

		GComponentStaticMeshManager.RegisterRenderComponents( transformHandle, staticMeshHandle );
		GComponentLightManager.RegisterRenderComponents( transformHandle, lightHandle );
	}
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
		"../content/models/plane",
	};
	CT_ASSERT( ARRAYSIZE( meshes ) == G_MAX );

	wchar_t const* textures[] =
	{
		L"../content/textures/sdf_font_512.png",
		L"../content/textures/common/black.png",
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
		//L"../content/textures/lena.dds",
		//L"../content/textures/lena_test.dds",
		L"../content/textures/stained_glass.dds",
		L"../content/textures/stained_glass_test.dds",
	};

#if defined( CREATE_LTC_TEX )
	{
		DirectX::TexMetadata texMeta;
		DirectX::ScratchImage image;
		//CheckResult( DirectX::LoadFromDDSFile( L"../content/textures/lena.dds", DirectX::DDS_FLAGS_NONE, &texMeta, image ) );
		CheckResult( DirectX::LoadFromDDSFile( L"../content/textures/stained_glass.dds", DirectX::DDS_FLAGS_NONE, &texMeta, image ) );

		UINT const mipLevels = texMeta.mipLevels;
		DirectX::ScratchImage dstImage;
		dstImage.Initialize2D( texMeta.format, texMeta.width << 1, texMeta.height << 1, 1, mipLevels );

		for ( UINT i = 0; i < mipLevels; ++i )
		{
			UINT const size = image.GetImage( i, 0, 0 )->width;
			TestTextureMipMap2D( dstImage.GetImage( i, 0, 0 )->pixels, size << 1, size << 1, image.GetImage( i, 0, 0 )->pixels, size, size, i );
		}

		//DirectX::SaveToDDSFile( dstImage.GetImages(), dstImage.GetImageCount(), dstImage.GetMetadata(), 0, L"../content/textures/lena_test.dds" );
		DirectX::SaveToDDSFile( dstImage.GetImages(), dstImage.GetImageCount(), dstImage.GetMetadata(), 0, L"../content/textures/stained_glass_test.dds" );
	}
#endif

	GRender.Init();

	GComponentLightManager.SetDirectLightColor( Vec3::ZERO );
	GComponentLightManager.SetDirectLightDir( Vec3( 0.f, 1.f, 1.f ).GetNormalized() );
	GComponentLightManager.SetAmbientLightColor( Vec3( 0.2f, 0.2f, 0.2f ) );

	GRender.BeginLoadResources(ARRAYSIZE(textures));

	for ( UINT meshID = 0; meshID < ARRAYSIZE( meshes ); ++meshID )
	{
		SGeometryData geometryData;
		GGeometryLoader.LoadMesh( geometryData, meshes[ meshID ] );
		GGeometryInfo[ meshID ].m_geometryID = GRender.LoadResource( geometryData );
		GGeometryInfo[ meshID ].m_indicesNum = geometryData.m_indices.Size();
	}

	for (unsigned int textureID = 0; textureID < ARRAYSIZE(textures); ++textureID)
	{
		DirectX::TexMetadata texMeta;
		DirectX::ScratchImage image;
		if ( DirectX::LoadFromWICFile( textures[ textureID ], DirectX::WIC_FLAGS_NONE, &texMeta, image ) != S_OK )
		{
			CheckResult( DirectX::LoadFromDDSFile( textures[ textureID ], DirectX::DDS_FLAGS_NONE, &texMeta, image ) );
		}

		STextureInfo texture;
		texture.m_width = UINT(texMeta.width);
		texture.m_height = UINT(texMeta.height);
		texture.m_format = texMeta.format;
		texture.m_mipLevels = Byte(texMeta.mipLevels);

		ASSERT( texture.m_format != DXGI_FORMAT_UNKNOWN );
		GTextureResources[ textures[ textureID ] ] = GRender.LoadResource( texture, image.GetPixels() );
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

	//STexture testTexture;
	//testTexture.m_width = 512;
	//testTexture.m_height = 512;
	//testTexture.m_mipLevels = 10;
	//GRender.Test( T_LENA, L"../content/textures/stained_glass_test_2.dds", testTexture );

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