#include "render.h"

CEnvironmentParticleManager::CEnvironmentParticleManager()
	: m_particlesGPU( nullptr )
	, m_particleCA( nullptr )
	, m_particleCL( nullptr )
	, m_particleRS( nullptr )
	, m_particlesNum( 0 )
{}

void CEnvironmentParticleManager::Init( UINT const initParticleNum, UINT const boxesNum, float const boxesSize )
{
	ID3D12Device* const device = GRender.GetDevice();

	D3D12_ROOT_PARAMETER rootParameters[3];
	rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameters[0].Descriptor = {0, 0};
	rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameters[1].Descriptor = {1, 0};
	rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	rootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_UAV;
	rootParameters[2].Descriptor = { 0, 0 };
	rootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	D3D12_ROOT_SIGNATURE_DESC descRootSignature;
	descRootSignature.NumParameters = ARRAYSIZE(rootParameters);
	descRootSignature.pParameters = rootParameters;
	descRootSignature.NumStaticSamplers = 0;
	descRootSignature.pStaticSamplers = nullptr;
	descRootSignature.Flags = D3D12_ROOT_SIGNATURE_FLAG_NONE;
	ID3DBlob* signature;
	ID3DBlob* error = nullptr;
	CheckResult(D3D12SerializeRootSignature(&descRootSignature, D3D_ROOT_SIGNATURE_VERSION_1, &signature, &error), error);
	CheckResult(device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&m_particleRS)));
	signature->Release();

	CheckResult( device->CreateCommandAllocator( D3D12_COMMAND_LIST_TYPE::D3D12_COMMAND_LIST_TYPE_COMPUTE, IID_PPV_ARGS( &m_particleCA ) ) );
	CheckResult( device->CreateCommandList( 0, D3D12_COMMAND_LIST_TYPE::D3D12_COMMAND_LIST_TYPE_COMPUTE, m_particleCA, nullptr, IID_PPV_ARGS( &m_particleCL ) ) );
	CheckResult(m_particleCL->Close());

	m_particleRS->SetName( L"Environment Particle Root Signature" );
	m_particleCA->SetName( L"Environment Particle Command Allocator" );
	m_particleCL->SetName( L"Environment Particle Command List" );

	m_particleShaderInit.InitComputeShader( L"../shaders/environmentParticleInit.hlsl", m_particleRS );

	InitParticles( initParticleNum, boxesNum, boxesSize );
}

void CEnvironmentParticleManager::AllocateBuffers()
{
	if ( m_particlesGPU )
	{
		m_particlesGPU->Release();
	}

	ID3D12Device* const device = GRender.GetDevice();

	D3D12_RESOURCE_DESC particleResDesc = {};
	particleResDesc.Dimension = D3D12_RESOURCE_DIMENSION::D3D12_RESOURCE_DIMENSION_BUFFER;
	particleResDesc.Width = m_particlesNum * 10 * sizeof( float ); //position + velocity
	particleResDesc.Height = 1;
	particleResDesc.DepthOrArraySize = 1;
	particleResDesc.MipLevels = 1;
	particleResDesc.Format = DXGI_FORMAT_UNKNOWN;
	particleResDesc.SampleDesc.Count = 1;
	particleResDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	particleResDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;

	CheckResult( device->CreateCommittedResource( &GHeapPropertiesGPUOnly, D3D12_HEAP_FLAG_NONE, &particleResDesc, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, nullptr, IID_PPV_ARGS( &m_particlesGPU ) ) );

	m_particlesGPU->SetName( L"Environment Particle Buffer" );
}

void CEnvironmentParticleManager::InitParticles(UINT const initParticleNum, UINT const boxesNum, float const boxesSize)
{
	m_particlesNum = initParticleNum;
	m_boxesSize = boxesSize;

	m_boxCenterOffset = Vec3( 0.5f, -0.5f, 0.5f ) * m_boxesSize;
	m_fade.Set( m_boxesSize * float( 2 * boxesNum ), m_boxesSize * float( 2 * boxesNum - 1 ) );
	m_boxesNum[ 0 ] = ( 2 * boxesNum + 1 );
	m_boxesNum[ 1 ] = m_boxesNum[ 0 ] * m_boxesNum[ 0 ];

	m_renderData.m_verticesStart = 0;
	m_renderData.m_indicesStart = 0;
	m_renderData.m_indicesNum = 6 * m_particlesNum;
	m_renderData.m_instancesNum = m_boxesNum[ 0 ] * m_boxesNum[ 1 ];
	m_renderData.m_geometryID = 0;
	m_renderData.m_topology = D3D_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	m_renderData.m_texturesNum = 1;
	m_renderData.m_shaderID = EShaderType::ST_ENV_PARTICLE;
	m_renderData.m_drawType = EDrawType::DrawInstanced;
	m_size = float( boxesNum ) * m_boxesSize;

	m_boxMatrix.m_a00 = m_boxesSize;
	m_boxMatrix.m_a11 = m_boxesSize;
	m_boxMatrix.m_a22 = m_boxesSize;

	AllocateBuffers();

	struct CBuffer
	{
		float m_velocity[ 3 ];
		float m_padding;
		float m_initSize[ 2 ];
		float m_sizeRand[ 2 ];
		float m_velocityOffsetRand[ 2 ];
		float m_speedRand[ 2 ];
		float m_tileSize[ 2 ];
		UINT m_tileNum[ 2 ];
		UINT m_seed;
		UINT m_particleNum;
	} cbuffer;

	//RAIN
	//cbuffer.m_velocity[ 0 ] = 4.f;
	//cbuffer.m_velocity[ 1 ] = -3.f;
	//cbuffer.m_velocity[ 2 ] = 1.f;
	//cbuffer.m_initSize[ 0 ] = .5f;
	//cbuffer.m_initSize[ 1 ] = 8.f;
	//cbuffer.m_sizeRand[ 0 ] = 0.005f;
	//cbuffer.m_sizeRand[ 1 ] = 0.01f;
	//cbuffer.m_velocityOffsetRand[ 0 ] = -.2f;
	//cbuffer.m_velocityOffsetRand[ 1 ] = .2f;
	//cbuffer.m_speedRand[ 0 ] = 2.f;
	//cbuffer.m_speedRand[ 1 ] = 3.f;
	//cbuffer.m_tileSize[ 0 ] = 1.f;
	//cbuffer.m_tileSize[ 1 ] = 1.f;
	//cbuffer.m_tileNum[ 0 ] = 1;
	//cbuffer.m_tileNum[ 1 ] = 1;
	//SNOW
	cbuffer.m_velocity[ 0 ] = 2.f;
	cbuffer.m_velocity[ 1 ] = -3.f;
	cbuffer.m_velocity[ 2 ] = 1.f;
	cbuffer.m_initSize[ 0 ] = 1.f;
	cbuffer.m_initSize[ 1 ] = 1.f;
	cbuffer.m_sizeRand[ 0 ] = 0.02f;
	cbuffer.m_sizeRand[ 1 ] = 0.04f;
	cbuffer.m_velocityOffsetRand[ 0 ] = -.4f;
	cbuffer.m_velocityOffsetRand[ 1 ] = .4f;
	cbuffer.m_speedRand[ 0 ] = .1f;
	cbuffer.m_speedRand[ 1 ] = .2f;
	cbuffer.m_tileSize[ 0 ] = 1.f / 4.f;
	cbuffer.m_tileSize[ 1 ] = 1.f / 4.f;
	cbuffer.m_tileNum[ 0 ] = 4;
	cbuffer.m_tileNum[ 1 ] = 4;

	cbuffer.m_seed = rand();
	cbuffer.m_particleNum = m_particlesNum;
	( ( Vec3* )( &cbuffer.m_velocity[ 0 ] ) )->Normalize();
	Vec3 const forward = *( ( Vec3* )( &cbuffer.m_velocity[ 0 ] ) );
	
	InitProjectionMatrix( forward, boxesNum );

	D3D12_GPU_VIRTUAL_ADDRESS constBufferAddress;
	GRender.SetConstBuffer( constBufferAddress, (Byte*)&cbuffer, sizeof( cbuffer ) );

	m_particleCA->Reset();
	m_particleCL->Reset( m_particleCA, m_particleShaderInit.GetPSO() );
	
	m_particleCL->SetComputeRootSignature( m_particleRS );
	m_particleCL->SetComputeRootConstantBufferView( 1, constBufferAddress );
	m_particleCL->SetComputeRootUnorderedAccessView( 2, m_particlesGPU->GetGPUVirtualAddress() );
	
	m_particleCL->Dispatch( ( m_particlesNum + 63 ) & ~63, 1, 1 );
	
	m_particleCL->Close();
	
	GRender.ExecuteComputeQueue( 1, (ID3D12CommandList* const*)(&m_particleCL) );
	GRender.WaitForComputeQueue();
}

void CEnvironmentParticleManager::InitProjectionMatrix( Vec3 const forward, UINT const boxesNum )
{
	Vec3 up = abs( Vec3::Dot( Vec3::UP, forward ) ) == 1.f ? Vec3::FORWARD : Vec3::UP;
	Vec3 right = Vec3::Cross( up, forward );
	up = Vec3::Cross( forward, right );

	right.Normalize();
	up.Normalize();

	m_viewToWorld.m_x.Set( right.x, right.y, right.z, 0.f );
	m_viewToWorld.m_y.Set( up.x, up.y, up.z, 0.f );
	m_viewToWorld.m_z.Set( forward.x, forward.y, forward.z, 0.f );

	Matrix4x4 worldToView;
	m_viewToWorld.Inverse( worldToView );
	float const vertexPositionOffset = ( float( boxesNum ) + 0.5f ) * m_boxesSize;

	Vec4 vertices[]
	{
		Vec4( +vertexPositionOffset, +vertexPositionOffset, +vertexPositionOffset, 1.f ), Vec4( -vertexPositionOffset, +vertexPositionOffset, +vertexPositionOffset, 1.f ),
		Vec4( -vertexPositionOffset, -vertexPositionOffset, +vertexPositionOffset, 1.f ), Vec4( +vertexPositionOffset, -vertexPositionOffset, +vertexPositionOffset, 1.f ),
		Vec4( +vertexPositionOffset, +vertexPositionOffset, -vertexPositionOffset, 1.f ), Vec4( -vertexPositionOffset, +vertexPositionOffset, -vertexPositionOffset, 1.f ),
		Vec4( -vertexPositionOffset, -vertexPositionOffset, -vertexPositionOffset, 1.f ), Vec4( +vertexPositionOffset, -vertexPositionOffset, -vertexPositionOffset, 1.f )
	};

	float maxAxis[] = { 0.f, 0.f, 0.f };

	for ( UINT i = 0; i < ARRAYSIZE( vertices ); ++i )
	{
		Vec4 const vertex = Math::Mul( vertices[ i ], worldToView );

		maxAxis[ 0 ] = max( maxAxis[ 0 ], abs( vertex.data[ 0 ] ) );
		maxAxis[ 1 ] = max( maxAxis[ 1 ], abs( vertex.data[ 1 ] ) );
		maxAxis[ 2 ] = max( maxAxis[ 2 ], abs( vertex.data[ 2 ] ) );
	}

	m_viewToScreen.m_x.x = 1.f/ maxAxis[ 0 ];
	m_viewToScreen.m_y.y = 1.f/ maxAxis[ 1 ];
	m_viewToScreen.m_z.z = 1.f/ ( 500.f * 2.f * maxAxis[ 2 ] );

	m_projectPositionOffset = forward * ( 1.f - 500.f * 2.f );
	m_positionOffset = m_boxesSize * float( m_boxesNum[ 0 ] >> 1 );
}

void CEnvironmentParticleManager::FillRenderData()
{
	m_renderData.m_texturesOffset = GRender.GetTexturesOffset();
	m_renderData.m_texturesNum = 1;
	//GRender.AddTextureID( T_RAIN_DROP );
	GRender.AddTextureID( T_SNOW );
	//Vec2 const uvScale( 1.f, 1.f );

	Vec3 const cameraPosition = GComponentCameraManager.GetMainCameraPosition();
	Vec3 const cameraForward = GComponentCameraManager.GetMainCameraForward();
	Vec3 const startPosition = Math::Snap( cameraPosition + cameraForward * m_positionOffset, m_boxesSize ) - m_boxCenterOffset;
	Vec3 const boxesMin = startPosition + m_boxCenterOffset - m_size;
	Matrix4x4 tObjectToWorld = m_boxMatrix;
	tObjectToWorld.m_w = boxesMin;
	tObjectToWorld.Transpose();

	Vec2 const uvScale( 1.f / 4.f, 1.f / 4.f );
	float const softFactor = 0.5f;
	CConstBufferCtx const cbCtx = GRender.GetConstBufferCtx( m_renderData.m_cbOffset, m_renderData.m_shaderID );
	cbCtx.SetParam( &tObjectToWorld,		3 * sizeof( Vec4 ),				EShaderParameters::ObjectToWorld );
	cbCtx.SetParam( &uvScale,				sizeof( uvScale ),				EShaderParameters::UVScale );
	cbCtx.SetParam( &m_fade,				sizeof( m_fade ),				EShaderParameters::Fade );
	cbCtx.SetParam( &m_boxesNum,			sizeof( m_boxesNum ),			EShaderParameters::BoxesNum );
	cbCtx.SetParam( &m_boxesSize,			sizeof( m_boxesSize ),			EShaderParameters::Size );
	cbCtx.SetParam( &softFactor,			sizeof( softFactor ),			EShaderParameters::Soft );

	GRender.AddCommonRenderData( m_renderData, ERenderLayer::RL_TRANSLUCENT );
}
void CEnvironmentParticleManager::Release()
{
	m_particlesGPU->Release();

	m_particleCA->Release();
	m_particleCL->Release();

	m_particleRS->Release();
	m_particleShaderInit.Release();
}

D3D12_GPU_VIRTUAL_ADDRESS CEnvironmentParticleManager::GetParticlesBufferAddress() const
{
	return m_particlesGPU->GetGPUVirtualAddress();
}

CEnvironmentParticleManager GEnvironmentParticleManager;