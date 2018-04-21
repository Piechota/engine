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
SComponentHandle ltcLightTest;

//#define DUMMY_PROFILER

#ifdef DUMMY_PROFILER
#include <fstream>
long constexpr GFrameProfNum = 2000;
INT64 GFramesProf[ GFrameProfNum ];
long GFrameProfID = 0;
#endif

void GaussianKelner1D( float* const dst, float const weight, int const radius )
{
	int const dim = 2 * radius + 1;

	float const d0 = 1.f / ( weight * sqrt( 2.f * M_PI ) );
	float const d1 = 1.f / ( 2.f * weight * weight );

	for ( int x = -radius; x <= radius; ++x )
	{
		float const dist = float( x ) * float( x );
		dst[ x + radius ] = exp( -dist * d1 ) * d0;
	}
}

void GaussianKelner2D( float* const dst, float const weight, int const radius )
{
	int const dim = 2 * radius + 1;

	float const d0 = 1.f / ( weight * sqrt( 2.f * M_PI ) );
	float const d1 = 1.f / ( 2.f * weight * weight );

	for ( int y = -radius; y <= radius; ++y )
	{
		for ( int x = -radius; x <= radius; ++x )
		{
			float const dist = float( x ) * float( x ) + float( y ) * float( y );
			dst[ (y + radius) * dim + x + radius ] = exp( -dist * d1 ) * d0;
		}
	}
}

void TestTextureMipMap2D( Byte* const dstBegin, UINT const dstWidth, UINT const dstHeight, Byte* const srcBegin, UINT const srcWidth, UINT const srcHeight, UINT const inTexKernelRadius )
{
	UINT const margSize = dstWidth / 4;

	int const texCoordX0 = margSize;
	int const texCoordY0 = margSize;
	int const texCoordX1 = dstWidth - margSize;
	int const texCoordY1 = dstHeight - margSize;

	UINT const maxKernelDim = 2 * (4 * margSize + 1);
	UINT const maxKernelRadius = ( maxKernelDim - 1 ) / 2;

	float* kernel = new float[ maxKernelDim * maxKernelDim ];
	GaussianKelner2D( kernel, 9000000000.f, int( maxKernelRadius ) );

	for ( int y = 0; y < dstHeight; ++y )
	{
		UINT const distanceY = max( 0, max( texCoordY0 - y, y - texCoordY1 + 1 ) );
		for ( int x = 0; x < dstWidth; ++x )
		{
			UINT const distanceX = max( 0, max( texCoordX0 - x, x - texCoordX1 + 1 ) );
			UINT const distance = max( distanceX, distanceY );

			UINT const kernelRadius = max( inTexKernelRadius, int(float(distance * 3.f)) );
			UINT const kernelDim = 2 * kernelRadius + 1;
			UINT const kernelSize = kernelDim * kernelDim;

			UINT const kernelOffset = maxKernelRadius - kernelRadius;

			UINT const sampleX0 = max( texCoordX0, x < kernelRadius ? 0 : ( x - kernelRadius ) );
			UINT const sampleY0 = max( texCoordY0, y < kernelRadius ? 0 : ( y - kernelRadius ) );

			UINT const sampleX1 = min( texCoordX1, min( dstWidth, x + kernelRadius + 1 ) );
			UINT const sampleY1 = min( texCoordY1, min( dstHeight, y + kernelRadius + 1 ) );

			UINT const kernelX0 = kernelRadius + sampleX0 - x;
			UINT const kernelY0 = kernelRadius + sampleY0 - y;

			UINT const kernelX1 = kernelRadius + sampleX1 - x;
			UINT const kernelY1 = kernelRadius + sampleY1 - y;

			UINT const srcSampleX0 = sampleX0 - margSize;
			UINT const srcSampleY0 = sampleY0 - margSize;

			UINT const srcSampleX1 = sampleX1 - margSize;
			UINT const srcSampleY1 = sampleY1 - margSize;

			float sum = 0.f;

			float accR = 0.f;
			float accG = 0.f;
			float accB = 0.f;

			for ( UINT currentY = srcSampleY0; currentY < srcSampleY1; ++currentY )
			{
				for ( UINT currentX = srcSampleX0; currentX < srcSampleX1; ++currentX )
				{
					int const kernelCoordX = ( int( kernelOffset + kernelX0 + currentX - srcSampleX0 ) - int( maxKernelRadius ) );
					int const kernelCoordY = ( int( kernelOffset + kernelY0 + currentY - srcSampleY0 ) - int( maxKernelRadius ) );

					//if ( kernelCoordX * kernelCoordX + kernelCoordY * kernelCoordY <= kernelRadius * kernelRadius )
					{
						float const srcR = float( srcBegin[ ( currentY * srcWidth + currentX ) * 4 + 0 ] );
						float const srcG = float( srcBegin[ ( currentY * srcWidth + currentX ) * 4 + 1 ] );
						float const srcB = float( srcBegin[ ( currentY * srcWidth + currentX ) * 4 + 2 ] );
						float const kernelVal = kernel[ ( kernelOffset + kernelY0 + currentY - srcSampleY0 ) * maxKernelDim + kernelOffset + kernelX0 + currentX - srcSampleX0 ];
						accR += srcR * kernelVal;
						accG += srcG * kernelVal;
						accB += srcB * kernelVal;

						sum += kernelVal;
					}
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

void TestTextureMipMap1D( Byte* const dstBegin, UINT const dstWidth, UINT const dstHeight, Byte* const srcBegin, UINT const srcWidth, UINT const srcHeight, UINT const inTexKernelRadius )
{
	UINT const margSize = dstWidth / 4;

	int const texCoordX0 = margSize;
	int const texCoordY0 = margSize;
	int const texCoordX1 = dstWidth - margSize;
	int const texCoordY1 = dstHeight - margSize;

	UINT const maxKernelDim = max( 4 * margSize + 1, 2 * inTexKernelRadius + 1 );
	UINT const maxKernelRadius = ( maxKernelDim - 1 ) / 2;

	float* kernel = new float[ maxKernelDim ];
	GaussianKelner1D( kernel, 500.f, int( maxKernelRadius ) );

	Byte* const blurHorizontal = new Byte[ dstWidth * 3 * dstHeight ];

	for ( int y = 0; y < dstHeight; ++y )
	{
		UINT const distanceY = max( 0, max( texCoordY0 - y, y - texCoordY1 + 1 ) );
		for ( int x = 0; x < dstWidth; ++x )
		{
			UINT const distanceX = max( 0, max( texCoordX0 - x, x - texCoordX1 + 1 ) );
			UINT const distance = max( distanceX, distanceY );

			UINT const kernelRadius = max( inTexKernelRadius, int(float(distance * 2.f)) );
			UINT const kernelDim = 2 * kernelRadius + 1;

			UINT const kernelOffset = maxKernelRadius - kernelRadius;

			UINT const sampleX0 = max( texCoordX0, x < kernelRadius ? 0 : ( x - kernelRadius ) );
			UINT const sampleY0 = min( texCoordY1 - 1, max( texCoordY0, y ) );

			UINT const sampleX1 = min( texCoordX1, min( dstWidth, x + kernelRadius + 1 ) );

			UINT const kernelX0 = kernelRadius + sampleX0 - x;

			UINT const srcSampleX0 = sampleX0 - margSize;
			UINT const srcSampleY0 = sampleY0 - margSize;

			UINT const srcSampleX1 = sampleX1 - margSize;

			float sum = 0.f;

			float accR = 0.f;
			float accG = 0.f;
			float accB = 0.f;

			for ( UINT currentX = srcSampleX0; currentX < srcSampleX1; ++currentX )
			{
					float const srcR = float( srcBegin[ ( srcSampleY0 * srcWidth + currentX ) * 4 + 0 ] );
					float const srcG = float( srcBegin[ ( srcSampleY0 * srcWidth + currentX ) * 4 + 1 ] );
					float const srcB = float( srcBegin[ ( srcSampleY0 * srcWidth + currentX ) * 4 + 2 ] );
					float const kernelVal = kernel[ kernelOffset + kernelX0 + currentX - srcSampleX0 ];
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
		UINT const distanceY = max( 0, max( texCoordY0 - y, y - texCoordY1 + 1 ) );
		for ( int x = 0; x < dstWidth; ++x )
		{
			UINT const distanceX = max( 0, max( texCoordX0 - x, x - texCoordX1 + 1 ) );
			UINT const distance = max( distanceX, distanceY );

			UINT const kernelRadius = max( inTexKernelRadius, int(float(distance * 2.f)) );
			UINT const kernelDim = 2 * kernelRadius + 1;

			UINT const kernelOffset = maxKernelRadius - kernelRadius;

			UINT const sampleX0 = (  x < kernelRadius ? 0 : ( x - kernelRadius ) );
			UINT const sampleY0 = y;

			UINT const sampleX1 = ( min( dstWidth, x + kernelRadius + 1 ) );

			UINT const kernelX0 = kernelRadius + sampleX0 - x;
			UINT const kernelX1 = kernelRadius + sampleX1 - x;

			float sum = 0.f;

			float accR = 0.f;
			float accG = 0.f;
			float accB = 0.f;

			for ( UINT currentX = sampleX0; currentX < sampleX1; ++currentX )
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

	testEntityID = GEntityManager.CreateEntity();
	testEntity = GEntityManager.GetEntity( testEntityID );
	ltcLightTest = testEntity->AddComponentTransform();
	testObjectStaticMeshHandle = testEntity->AddComponentStaticMesh();
	SComponentHandle testLtcLightComponentHandle = testEntity->AddComponentLight();
	SComponentLight& ltcLight = GComponentLightManager.GetComponent( testLtcLightComponentHandle );
	
	testObjectTransform = &GComponentTransformManager.GetComponent( ltcLightTest );
	SComponentStaticMesh& ltcLightStaticMesh = GComponentStaticMeshManager.GetComponent( testObjectStaticMeshHandle );

	testObjectTransform->m_position.Set( -1.5f, 0.5f, 10.f );
	testObjectTransform->m_scale.Set( 1.f, 1.f, 1.f );
	testObjectTransform->m_rotation = Quaternion::FromAxisAngle( Vec3::RIGHT.data, -90.f * MathConsts::DegToRad );

	ltcLightStaticMesh.m_color.Set( 1.f, 1.f, 1.f );
	ltcLightStaticMesh.m_tiling.Set( 1.f, 1.f );
	ltcLightStaticMesh.m_geometryInfoID = G_PLANE;
	ltcLightStaticMesh.m_layer = RL_UNLIT;
	ltcLightStaticMesh.m_shaderID = Byte(ST_OBJECT_DRAW_SIMPLE);
	memset( ltcLightStaticMesh.m_textureID, UINT8_MAX, sizeof( ltcLightStaticMesh.m_textureID ) );
	GComponentStaticMeshManager.RegisterRenderComponents( ltcLightTest.m_index, testObjectStaticMeshHandle.m_index );

	ltcLight.m_color.Set( 2.f, 2.f, 2.f );
	ltcLight.m_textureID = UINT8_MAX;
	ltcLight.m_lightShader = Byte( ELightFlags::LF_LTC );
	GComponentLightManager.RegisterRenderComponents( ltcLightTest.m_index, testLtcLightComponentHandle.m_index );

	testEntityID = GEntityManager.CreateEntity();
	testEntity = GEntityManager.GetEntity( testEntityID );
	ltcLightTest = testEntity->AddComponentTransform();
	testObjectStaticMeshHandle = testEntity->AddComponentStaticMesh();
	testLtcLightComponentHandle = testEntity->AddComponentLight();
	SComponentLight& ltcLightTex = GComponentLightManager.GetComponent( testLtcLightComponentHandle );

	testObjectTransform = &GComponentTransformManager.GetComponent( ltcLightTest );
	SComponentStaticMesh& ltcLightTexStaticMesh = GComponentStaticMeshManager.GetComponent( testObjectStaticMeshHandle );

	testObjectTransform->m_position.Set( 5.5f, 0.5f, 10.f );
	testObjectTransform->m_scale.Set( 1.f, 1.f, 1.f );
	testObjectTransform->m_rotation = Quaternion::FromAxisAngle( Vec3::RIGHT.data, -90.f * MathConsts::DegToRad );

	ltcLightTexStaticMesh.m_color.Set( 1.f, 1.f, 1.f );
	ltcLightTexStaticMesh.m_tiling.Set( 1.f, 1.f );
	ltcLightTexStaticMesh.m_geometryInfoID = G_PLANE;
	ltcLightTexStaticMesh.m_layer = RL_UNLIT;
	ltcLightTexStaticMesh.m_shaderID = Byte(ST_OBJECT_DRAW_SIMPLE_TEXTURE);
	memset( ltcLightTexStaticMesh.m_textureID, UINT8_MAX, sizeof( ltcLightTexStaticMesh.m_textureID ) );
	ltcLightTexStaticMesh.m_textureID[ 0 ] = T_LENA;
	GComponentStaticMeshManager.RegisterRenderComponents( ltcLightTest.m_index, testObjectStaticMeshHandle.m_index );

	ltcLightTex.m_color.Set( 2.f, 2.f, 2.f );
	ltcLightTex.m_textureID = T_LENA_TEST;
	ltcLightTex.m_lightShader = Byte( ELightFlags::LF_LTC_TEXTURE );
	GComponentLightManager.RegisterRenderComponents( ltcLightTest.m_index, testLtcLightComponentHandle.m_index );
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
		L"../content/textures/lena.dds",
		L"../content/textures/lena_test.dds",
		//L"../content/textures/stained_glass.dds",
		//L"../content/textures/stained_glass_test.dds",
	};
	CT_ASSERT( ARRAYSIZE( textures ) == T_MAX );

#if defined( CREATE_LTC_TEX )
	{
		DirectX::TexMetadata texMeta;
		DirectX::ScratchImage image;
		CheckResult( DirectX::LoadFromDDSFile( L"../content/textures/lena.dds", DirectX::DDS_FLAGS_NONE, &texMeta, image ) );
		//CheckResult( DirectX::LoadFromDDSFile( L"../content/textures/stained_glass.dds", DirectX::DDS_FLAGS_NONE, &texMeta, image ) );

		UINT const mipLevels = texMeta.mipLevels;
		DirectX::ScratchImage dstImage;
		dstImage.Initialize2D( texMeta.format, texMeta.width << 1, texMeta.height << 1, 1, mipLevels );

		for ( UINT i = 0; i < mipLevels; ++i )
		{
			UINT const size = image.GetImage( i, 0, 0 )->width;
			TestTextureMipMap1D( dstImage.GetImage( i, 0, 0 )->pixels, size << 1, size << 1, image.GetImage( i, 0, 0 )->pixels, size, size, i );
		}

		DirectX::SaveToDDSFile( dstImage.GetImages(), dstImage.GetImageCount(), dstImage.GetMetadata(), 0, L"../content/textures/lena_test.dds" );
		//DirectX::SaveToDDSFile( dstImage.GetImages(), dstImage.GetImageCount(), dstImage.GetMetadata(), 0, L"../content/textures/stained_glass_test.dds" );
	}
#endif

	GRender.Init();
	GComponentLightManager.SetDirectLightColor( Vec3::ZERO );
	GComponentLightManager.SetDirectLightDir( Vec3( 0.f, 1.f, 1.f ).GetNormalized() );
	GComponentLightManager.SetAmbientLightColor( Vec3::ZERO );

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
		DirectX::TexMetadata texMeta;
		DirectX::ScratchImage image;
		if ( DirectX::LoadFromWICFile( textures[ texutreID ], DirectX::WIC_FLAGS_NONE, &texMeta, image ) != S_OK )
		{
			CheckResult( DirectX::LoadFromDDSFile( textures[ texutreID ], DirectX::DDS_FLAGS_NONE, &texMeta, image ) );
		}

		STexture texture;
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

		SComponentTransform& testObjectTransform = GComponentTransformManager.GetComponent( ltcLightTest );
		//testObjectTransform.m_rotation = testObjectTransform.m_rotation * Quaternion::FromAxisAngle( Vec3::UP.data, GTimer.GameDelta() );

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