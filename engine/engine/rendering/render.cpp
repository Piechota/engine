#include "render.h"
#include "constBuffers.h"
#include "../timer.h"
#include "../math.h"
#include "../../DirectXTex/DirectXTex.h"

extern SViewObject GViewObject;
CRender GRender;

/*void CRender::Test( Byte const textureID, wchar_t* const fileName, STexture texture )
{
	struct SConstBuffer
	{
		float m_invDstTextureSize;
		float m_dstTextureSize;
		float m_mipMap;
		int m_maxKernelRadius;
	} cbuffer;

	D3D12_RESOURCE_DESC descTexture = {};
	descTexture.DepthOrArraySize = 1;
	descTexture.SampleDesc.Count = 1;
	descTexture.Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
	descTexture.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	descTexture.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	descTexture.MipLevels = texture.m_mipLevels;
	descTexture.Width = texture.m_width;
	descTexture.Height = texture.m_height;
	descTexture.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

	ID3D12Resource* textureRes;
	CheckResult(m_device->CreateCommittedResource(&GHeapPropertiesReadback, D3D12_HEAP_FLAG_NONE, &descTexture, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, nullptr, IID_PPV_ARGS(&textureRes)));

	D3D12_RESOURCE_DESC descResource = {};
	descResource.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	descResource.Alignment = 0;
	descResource.Height = 1;
	descResource.DepthOrArraySize = 1;
	descResource.MipLevels = 1;
	descResource.Format = DXGI_FORMAT_UNKNOWN;
	descResource.SampleDesc.Count = 1;
	descResource.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	descResource.Flags = D3D12_RESOURCE_FLAG_NONE;
	descResource.Width = (texture.m_width + 1) * (texture.m_width + 1) * 4;

	ID3D12Resource* kernelRes;
	CheckResult(m_device->CreateCommittedResource(&GHeapPropertiesUpload, D3D12_HEAP_FLAG_NONE, &descResource, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&kernelRes)));

	float* kernel = nullptr;
	CheckResult(kernelRes->Map(0, nullptr, (void**)(&kernel)));

	Math::GaussianKelner2D( kernel, 500.f, texture.m_width / 2 );
	kernelRes->Unmap( 0, nullptr );

	UINT const mipMap0 = m_texturesDH.m_descriptorsNum;

	for ( UINT i = 0; i < texture.m_mipLevels; ++i )
	{
		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.Texture2D.MipLevels = -1;
		srvDesc.Texture2D.MostDetailedMip = i;
		srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

		m_device->CreateShaderResourceView(textureRes, &srvDesc, m_texturesDH.GetCPUDescriptor(mipMap0 + i));
	}

	DirectX::ScratchImage dstImage;
	dstImage.Initialize2D( DXGI_FORMAT_R8G8B8A8_UNORM, texture.m_width, texture.m_width, 1, texture.m_mipLevels );

	m_computeCA->Reset();
	m_computeCL->Reset( m_computeCA, m_shaders[ST_LTC_TEXTURE].GetPSO() );
	m_computeCL->SetDescriptorHeaps(1, &m_texturesDH.m_pDescriptorHeap);
	m_computeCL->SetComputeRootSignature( m_ltcTextureRS );
	m_computeCL->SetComputeRootDescriptorTable( 1, m_texturesDH.GetGPUDescriptor( textureID ) );
	m_computeCL->SetComputeRootShaderResourceView( 3, kernelRes->GetGPUVirtualAddress() );

	for ( UINT i = 0; i < texture.m_mipLevels; ++i )
	{
		UINT const mipmapSize = texture.m_width >> i;
		UINT const groupNum = ( ( mipmapSize + 15 ) & ~15 ) / 16;

		cbuffer.m_dstTextureSize = float( mipmapSize );
		cbuffer.m_invDstTextureSize = 1.f / (cbuffer.m_dstTextureSize-1.f);
		cbuffer.m_maxKernelRadius = mipmapSize / 2;
		cbuffer.m_mipMap = float( i );

		D3D12_GPU_VIRTUAL_ADDRESS constBufferAddress;
		GRender.SetConstBuffer( constBufferAddress, ( Byte* )&cbuffer, sizeof( cbuffer ) );
		m_computeCL->SetComputeRootConstantBufferView( 0, constBufferAddress );
		m_computeCL->SetComputeRootDescriptorTable( 2, m_texturesDH.GetGPUDescriptor( mipMap0 + i ) );

		m_computeCL->Dispatch( groupNum, groupNum, 1 );
	}

	m_computeCL->Close();

	m_computeCQ->ExecuteCommandLists( 1, (ID3D12CommandList* const*)&m_computeCL );
	WaitForComputeQueue();

	for ( UINT i = 0; i < texture.m_mipLevels; ++i )
	{
		UINT const mipmapSize = texture.m_width >> i;
		CheckResult( textureRes->ReadFromSubresource( dstImage.GetImage( i, 0, 0 )->pixels, mipmapSize * 4, mipmapSize * mipmapSize * 4, i, nullptr ) );
	}

	DirectX::SaveToDDSFile( dstImage.GetImages(), dstImage.GetImageCount(), dstImage.GetMetadata(), 0, fileName );

	kernelRes->Release();
	textureRes->Release();
}
*/
void CRender::InitCommands()
{
	D3D12_COMMAND_QUEUE_DESC descCQ = {};
	descCQ.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	descCQ.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	CheckResult(m_device->CreateCommandQueue(&descCQ, IID_PPV_ARGS(&m_graphicsCQ)));
	m_graphicsCQ->SetName(L"GraphicsCommandQueue");

	CheckResult(m_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_graphicsCA)));
	m_graphicsCA->SetName(L"GraphicsCommandAllocator");

	CheckResult(m_device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_graphicsCA, nullptr, IID_PPV_ARGS(&m_graphicsCL)));
	m_graphicsCL->SetName(L"GraphicsCommandList");
	CheckResult(m_graphicsCL->Close());

	descCQ.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	descCQ.Type = D3D12_COMMAND_LIST_TYPE_COPY;
	CheckResult(m_device->CreateCommandQueue(&descCQ, IID_PPV_ARGS(&m_copyCQ)));
	m_copyCQ->SetName(L"CopyCommandQueue");

	CheckResult(m_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_COPY, IID_PPV_ARGS(&m_copyCA)));
	m_copyCA->SetName(L"CopyCommandAllocator");

	CheckResult(m_device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_COPY, m_copyCA, nullptr, IID_PPV_ARGS(&m_copyCL)));
	m_copyCL->SetName(L"CopyCommandList");
	CheckResult(m_copyCL->Close());

	descCQ.Flags = D3D12_COMMAND_QUEUE_FLAG_DISABLE_GPU_TIMEOUT;
	descCQ.Type = D3D12_COMMAND_LIST_TYPE_COMPUTE;
	CheckResult(m_device->CreateCommandQueue(&descCQ, IID_PPV_ARGS(&m_computeCQ)));
	m_computeCQ->SetName(L"ComputeCommandQueue");

	CheckResult(m_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_COMPUTE, IID_PPV_ARGS(&m_computeCA)));
	m_computeCA->SetName(L"ComputeCommandAllocator");

	CheckResult(m_device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_COMPUTE, m_computeCA, nullptr, IID_PPV_ARGS(&m_computeCL)));
	m_computeCL->SetName(L"ComputeCommandList");
	CheckResult(m_computeCL->Close());
}

void CRender::InitFrameData()
{
	for (UINT frameID = 0; frameID < FRAME_NUM; ++frameID)
	{
		SRenderFrameData& frameData = m_frameData[frameID];

		CheckResult(m_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&frameData.m_frameCA)));
		frameData.m_frameCA->SetName(L"FrameCommandAllocator");

		CheckResult(m_device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, frameData.m_frameCA, nullptr, IID_PPV_ARGS(&frameData.m_frameCL)));
		frameData.m_frameCL->SetName(L"FrameCommandList");
		CheckResult(frameData.m_frameCL->Close());
	}
}

void CRender::InitConstBuffer()
{
	D3D12_RESOURCE_DESC descResource = {};
	descResource.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	descResource.Alignment = 0;
	descResource.Height = 1;
	descResource.DepthOrArraySize = 1;
	descResource.MipLevels = 1;
	descResource.Format = DXGI_FORMAT_UNKNOWN;
	descResource.SampleDesc.Count = 1;
	descResource.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	descResource.Flags = D3D12_RESOURCE_FLAG_NONE;
	descResource.Width = 1024 * MAX_OBJECTS;

	CheckResult(m_device->CreateCommittedResource(&GHeapPropertiesUpload, D3D12_HEAP_FLAG_NONE, &descResource, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&m_constBufferResource)));
	m_constBufferResource->SetName(L"ConstBufferResource");

	CheckResult(m_constBufferResource->Map(0, nullptr, (void**)(&m_pConstBufferData)));
}

void CRender::InitSwapChain()
{
	DXGI_SWAP_CHAIN_DESC descSwapChain = {};
	descSwapChain.BufferCount = FRAME_NUM;
	descSwapChain.BufferDesc.Width = m_wndWidth;
	descSwapChain.BufferDesc.Height = m_wndHeight;
	descSwapChain.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	descSwapChain.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	descSwapChain.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	descSwapChain.OutputWindow = m_hwnd;
	descSwapChain.SampleDesc.Count = 1;
	descSwapChain.Windowed = TRUE;

	IDXGISwapChain* swapChain;
	CheckResult(m_factor->CreateSwapChain(m_graphicsCQ, &descSwapChain, &swapChain));
	m_swapChain = (IDXGISwapChain3*)swapChain;
}

void CRender::InitRenderTargets()
{
	UINT rtvID = 0;
	for (; rtvID < FRAME_NUM; ++rtvID)
	{
		CheckResult(m_swapChain->GetBuffer(rtvID, IID_PPV_ARGS(&m_rederTarget[rtvID])));
		m_device->CreateRenderTargetView(m_rederTarget[rtvID], nullptr, m_renderTargetDH.GetCPUDescriptor(rtvID));
		std::wstring const rtvName = L"RenderTarget" + std::to_wstring(rtvID);
		m_rederTarget[rtvID]->SetName(rtvName.c_str());
	}

	D3D12_RESOURCE_DESC gbufferDesc = {};
	gbufferDesc.DepthOrArraySize = 1;
	gbufferDesc.SampleDesc.Count = 1;
	gbufferDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
	gbufferDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	gbufferDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	gbufferDesc.MipLevels = 1;
	gbufferDesc.Width = m_wndWidth;
	gbufferDesc.Height = m_wndHeight;

	D3D12_RENDER_TARGET_VIEW_DESC gbufferViewDesc = {};
	gbufferViewDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;

	D3D12_CLEAR_VALUE gbufferClearValue;
	gbufferClearValue.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	gbufferClearValue.Color[ 0 ] = 0.f;
	gbufferClearValue.Color[ 1 ] = 0.f;
	gbufferClearValue.Color[ 2 ] = 0.f;
	gbufferClearValue.Color[ 3 ] = 0.f;

	gbufferViewDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	gbufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	m_globalBufferDescriptorsOffsets[ GBB_DIFFUSE_METALNESS ].m_rtvOffset = rtvID;
	CheckResult( m_device->CreateCommittedResource( &GHeapPropertiesGPUOnly, D3D12_HEAP_FLAG_NONE, &gbufferDesc, D3D12_RESOURCE_STATE_RENDER_TARGET, &gbufferClearValue, IID_PPV_ARGS( &m_globalBufferBuffers[ GBB_DIFFUSE_METALNESS ] ) ) );
	m_device->CreateRenderTargetView( m_globalBufferBuffers[ GBB_DIFFUSE_METALNESS ], &gbufferViewDesc, m_renderTargetDH.GetCPUDescriptor(rtvID) );
	m_globalBufferBuffers[ GBB_DIFFUSE_METALNESS ]->SetName( L"GBufferDiffuse" );
	++rtvID;

	gbufferClearValue.Format = DXGI_FORMAT_R10G10B10A2_UNORM;
	gbufferViewDesc.Format = DXGI_FORMAT_R10G10B10A2_UNORM;
	gbufferDesc.Format = DXGI_FORMAT_R10G10B10A2_UNORM;
	m_globalBufferDescriptorsOffsets[ GBB_NORMAL ].m_rtvOffset = rtvID;
	CheckResult( m_device->CreateCommittedResource( &GHeapPropertiesGPUOnly, D3D12_HEAP_FLAG_NONE, &gbufferDesc, D3D12_RESOURCE_STATE_RENDER_TARGET, &gbufferClearValue, IID_PPV_ARGS( &m_globalBufferBuffers[ GBB_NORMAL ] ) ) );
	m_device->CreateRenderTargetView( m_globalBufferBuffers[ GBB_NORMAL ], &gbufferViewDesc, m_renderTargetDH.GetCPUDescriptor(rtvID) );
	m_globalBufferBuffers[ GBB_NORMAL ]->SetName( L"GBufferNormal" );
	++rtvID;

	gbufferClearValue.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	gbufferViewDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	gbufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	m_globalBufferDescriptorsOffsets[ GBB_EMISSIVE_ROUGHNESS ].m_rtvOffset = rtvID;
	CheckResult( m_device->CreateCommittedResource( &GHeapPropertiesGPUOnly, D3D12_HEAP_FLAG_NONE, &gbufferDesc, D3D12_RESOURCE_STATE_RENDER_TARGET, &gbufferClearValue, IID_PPV_ARGS( &m_globalBufferBuffers[ GBB_EMISSIVE_ROUGHNESS ] ) ) );
	m_device->CreateRenderTargetView( m_globalBufferBuffers[ GBB_EMISSIVE_ROUGHNESS ], &gbufferViewDesc, m_renderTargetDH.GetCPUDescriptor(rtvID) );
	m_globalBufferBuffers[ GBB_EMISSIVE_ROUGHNESS ]->SetName( L"GBufferEmissive" );
	++rtvID;

	D3D12_RESOURCE_DESC depthDesc = {};
	depthDesc.DepthOrArraySize = 1;
	depthDesc.SampleDesc.Count = 1;
	depthDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
	depthDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	depthDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	depthDesc.MipLevels = 1;
	depthDesc.Width = m_wndWidth;
	depthDesc.Height = m_wndHeight;
	depthDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;

	D3D12_CLEAR_VALUE depthClearValue;
	depthClearValue.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthClearValue.DepthStencil.Depth = 1.f;
	depthClearValue.DepthStencil.Stencil = 0;
	m_globalBufferDescriptorsOffsets[ GBB_DEPTH ].m_rtvOffset = 0;
	CheckResult( m_device->CreateCommittedResource( &GHeapPropertiesGPUOnly, D3D12_HEAP_FLAG_NONE, &depthDesc, D3D12_RESOURCE_STATE_DEPTH_WRITE, &depthClearValue, IID_PPV_ARGS( &m_globalBufferBuffers[ GBB_DEPTH ] ) ) );
	m_globalBufferBuffers[ GBB_DEPTH ]->SetName( L"MainDepth" );

	D3D12_DEPTH_STENCIL_VIEW_DESC depthViewDesc = {};
	depthViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthViewDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	depthViewDesc.Flags = D3D12_DSV_FLAG_NONE;
	depthViewDesc.Texture2D.MipSlice = 0;

	m_device->CreateDepthStencilView( m_globalBufferBuffers[ GBB_DEPTH ], &depthViewDesc, m_depthBuffertDH.GetCPUDescriptor( 0 ) );

	D3D12_RESOURCE_DESC rainDepthDesc = {};
	rainDepthDesc.DepthOrArraySize = 1;
	rainDepthDesc.SampleDesc.Count = 1;
	rainDepthDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
	rainDepthDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	rainDepthDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	rainDepthDesc.MipLevels = 1;
	rainDepthDesc.Width = 1024;
	rainDepthDesc.Height = 1024;
	rainDepthDesc.Format = DXGI_FORMAT_D32_FLOAT;

	D3D12_CLEAR_VALUE rainDepthClearValue;
	rainDepthClearValue.Format = DXGI_FORMAT_D32_FLOAT;
	rainDepthClearValue.DepthStencil.Depth = 1.f;
	rainDepthClearValue.DepthStencil.Stencil = 0;
	m_globalBufferDescriptorsOffsets[ GBB_RAIN_DEPTH ].m_rtvOffset = 1;
	CheckResult( m_device->CreateCommittedResource( &GHeapPropertiesGPUOnly, D3D12_HEAP_FLAG_NONE, &rainDepthDesc, D3D12_RESOURCE_STATE_DEPTH_WRITE, &rainDepthClearValue, IID_PPV_ARGS( &m_globalBufferBuffers[ GBB_RAIN_DEPTH ] ) ) );
	m_globalBufferBuffers[ GBB_RAIN_DEPTH ]->SetName( L"RainDepth" );

	D3D12_DEPTH_STENCIL_VIEW_DESC rainDepthViewDesc = {};
	rainDepthViewDesc.Format = DXGI_FORMAT_D32_FLOAT;
	rainDepthViewDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	rainDepthViewDesc.Flags = D3D12_DSV_FLAG_NONE;
	rainDepthViewDesc.Texture2D.MipSlice = 0;

	m_device->CreateDepthStencilView( m_globalBufferBuffers[ GBB_RAIN_DEPTH ], &rainDepthViewDesc, m_depthBuffertDH.GetCPUDescriptor( 1 ) );

	DXGI_FORMAT const globalBuffersForamts[] = { DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_R10G10B10A2_UNORM, DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_R24_UNORM_X8_TYPELESS, DXGI_FORMAT_R32_FLOAT };
	CT_ASSERT( ARRAYSIZE( globalBuffersForamts ) == GBB_MAX );

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Texture2D.MipLevels = 1;
	srvDesc.Texture2D.MostDetailedMip = 0;

	for ( UINT i = 0; i < GBB_MAX; ++i )
	{
		m_globalBufferDescriptorsOffsets[ i ].m_srvOffset = m_texturesDHFree.PopBack();
		srvDesc.Format = globalBuffersForamts[ i ];
		m_device->CreateShaderResourceView(m_globalBufferBuffers[ i ], &srvDesc, m_texturesDH.GetCPUDescriptorScaled(m_globalBufferDescriptorsOffsets[ i ].m_srvOffset) );
	}
}

void CRender::InitDescriptorHeaps()
{
	for ( UINT dhIncSize = 0; dhIncSize < D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES; ++dhIncSize )
	{
		GDescriptorHandleIncrementSize[ dhIncSize ] = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE(dhIncSize));
	}

	InitDescriptorHeap( m_renderTargetDH, FRAME_NUM + GBB_MAX, D3D12_DESCRIPTOR_HEAP_TYPE_RTV, D3D12_DESCRIPTOR_HEAP_FLAG_NONE );
	InitDescriptorHeap( m_depthBuffertDH, 2, D3D12_DESCRIPTOR_HEAP_TYPE_DSV, D3D12_DESCRIPTOR_HEAP_FLAG_NONE );
	InitDescriptorHeap( m_texturesDH, MAX_TEXTURES, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE );

	m_texturesDHFree.Resize( MAX_TEXTURES );
	for ( UINT16 i = 0; i < MAX_TEXTURES; ++i )
	{
		m_texturesDHFree[ i ] = ( MAX_TEXTURES - i - 1 ) * GDescriptorHandleIncrementSize[ D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV ];
	}		
}

void CRender::InitDescriptorHeap( SDescriptorHeap& heap, UINT const descriptorsNum, D3D12_DESCRIPTOR_HEAP_TYPE const type, D3D12_DESCRIPTOR_HEAP_FLAGS const flags )
{
	heap.m_pDescriptorHeap = nullptr;
	heap.NumDescriptors = descriptorsNum;
	heap.Type = type;
	heap.Flags = flags;

	CheckResult(m_device->CreateDescriptorHeap(&heap, IID_PPV_ARGS(&heap.m_pDescriptorHeap)));
}

void CRender::InitGraphicsRootSignature()
{
	D3D12_DESCRIPTOR_RANGE descriptorRange[] =
	{
		{ D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0, 0, D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND },
		{ D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 1, 0, D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND },
		{ D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 2, 0, D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND },
		{ D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 3, 0, D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND },
		{ D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 4, 0, D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND },
		{ D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 5, 0, D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND },
		{ D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 6, 0, D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND },
	};
	D3D12_ROOT_PARAMETER rootParameters[10];

	rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameters[0].Descriptor = {0, 0};
	rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameters[1].Descriptor = {1, 0};
	rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	rootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_SRV;
	rootParameters[2].Descriptor = {0, 0};
	rootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;

	rootParameters[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameters[3].DescriptorTable = { 1, &descriptorRange[ 0 ] };
	rootParameters[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	rootParameters[4].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameters[4].DescriptorTable = { 1, &descriptorRange[ 1 ] };
	rootParameters[4].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	rootParameters[5].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameters[5].DescriptorTable = { 1, &descriptorRange[ 2 ] };
	rootParameters[5].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	rootParameters[6].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameters[6].DescriptorTable = { 1, &descriptorRange[ 3 ] };
	rootParameters[6].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	rootParameters[7].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameters[7].DescriptorTable = { 1, &descriptorRange[ 4 ] };
	rootParameters[7].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	rootParameters[8].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameters[8].DescriptorTable = { 1, &descriptorRange[ 5 ] };
	rootParameters[8].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	rootParameters[9].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameters[9].DescriptorTable = { 1, &descriptorRange[ 6 ] };
	rootParameters[9].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	D3D12_STATIC_SAMPLER_DESC const samplers[] =
	{
		{
			/*Filter*/				D3D12_FILTER_MIN_MAG_MIP_POINT
			/*AddressU*/			,D3D12_TEXTURE_ADDRESS_MODE_WRAP
		/*AddressV*/			,D3D12_TEXTURE_ADDRESS_MODE_WRAP
		/*AddressW*/			,D3D12_TEXTURE_ADDRESS_MODE_WRAP
		/*MipLODBias*/			,0
		/*MaxAnisotropy*/		,0
		/*ComparisonFunc*/		,D3D12_COMPARISON_FUNC_NEVER
		/*BorderColor*/			,D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK
		/*MinLOD*/				,0.f
		/*MaxLOD*/				,D3D12_FLOAT32_MAX
		/*ShaderRegister*/		,0
		/*RegisterSpace*/		,0
		/*ShaderVisibility*/	,D3D12_SHADER_VISIBILITY_PIXEL
		},
		{
			/*Filter*/				D3D12_FILTER_MIN_MAG_MIP_LINEAR
			/*AddressU*/			,D3D12_TEXTURE_ADDRESS_MODE_WRAP
		/*AddressV*/			,D3D12_TEXTURE_ADDRESS_MODE_WRAP
		/*AddressW*/			,D3D12_TEXTURE_ADDRESS_MODE_WRAP
		/*MipLODBias*/			,0
		/*MaxAnisotropy*/		,0
		/*ComparisonFunc*/		,D3D12_COMPARISON_FUNC_NEVER
		/*BorderColor*/			,D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK
		/*MinLOD*/				,0.f
		/*MaxLOD*/				,D3D12_FLOAT32_MAX
		/*ShaderRegister*/		,1
		/*RegisterSpace*/		,0
		/*ShaderVisibility*/	,D3D12_SHADER_VISIBILITY_PIXEL
		},
		{
			/*Filter*/				D3D12_FILTER_MIN_MAG_MIP_LINEAR
			/*AddressU*/			,D3D12_TEXTURE_ADDRESS_MODE_CLAMP
		/*AddressV*/			,D3D12_TEXTURE_ADDRESS_MODE_CLAMP
		/*AddressW*/			,D3D12_TEXTURE_ADDRESS_MODE_CLAMP
		/*MipLODBias*/			,0
		/*MaxAnisotropy*/		,0
		/*ComparisonFunc*/		,D3D12_COMPARISON_FUNC_NEVER
		/*BorderColor*/			,D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK
		/*MinLOD*/				,0.f
		/*MaxLOD*/				,D3D12_FLOAT32_MAX
		/*ShaderRegister*/		,2
		/*RegisterSpace*/		,0
		/*ShaderVisibility*/	,D3D12_SHADER_VISIBILITY_PIXEL
		}
	};

	D3D12_ROOT_SIGNATURE_DESC descRootSignature;
	descRootSignature.NumParameters = ARRAYSIZE(rootParameters);
	descRootSignature.pParameters = rootParameters;
	descRootSignature.NumStaticSamplers = ARRAYSIZE(samplers);
	descRootSignature.pStaticSamplers = samplers;
	descRootSignature.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT | D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS | D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS | D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS;

	ID3DBlob* signature;
	ID3DBlob* error = nullptr;
	CheckResult(D3D12SerializeRootSignature(&descRootSignature, D3D_ROOT_SIGNATURE_VERSION_1, &signature, &error), error);
	CheckResult(m_device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&m_graphicsRS)));
	signature->Release();
}
void CRender::InitLTCTextureRootSignature()
{
	D3D12_DESCRIPTOR_RANGE descriptorRange[] =
	{
		{ D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0, 0, D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND },
		{ D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 1, 0, 0, D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND },
	};

	D3D12_ROOT_PARAMETER rootParameters[4];
	rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameters[0].Descriptor = {0, 0};
	rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameters[1].DescriptorTable = { 1, &descriptorRange[ 0 ] };
	rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	rootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameters[2].DescriptorTable = { 1, &descriptorRange[ 1 ] };
	rootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	rootParameters[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_SRV;
	rootParameters[3].Descriptor = {1, 0};
	rootParameters[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	D3D12_STATIC_SAMPLER_DESC const samplers[] =
	{
		{
		/*Filter*/				D3D12_FILTER_MIN_MAG_MIP_LINEAR
		/*AddressU*/			,D3D12_TEXTURE_ADDRESS_MODE_CLAMP
		/*AddressV*/			,D3D12_TEXTURE_ADDRESS_MODE_CLAMP
		/*AddressW*/			,D3D12_TEXTURE_ADDRESS_MODE_CLAMP
		/*MipLODBias*/			,0
		/*MaxAnisotropy*/		,0
		/*ComparisonFunc*/		,D3D12_COMPARISON_FUNC_NEVER
		/*BorderColor*/			,D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK
		/*MinLOD*/				,0.f
		/*MaxLOD*/				,D3D12_FLOAT32_MAX
		/*ShaderRegister*/		,0
		/*RegisterSpace*/		,0
		/*ShaderVisibility*/	,D3D12_SHADER_VISIBILITY_ALL
		}
	};

	D3D12_ROOT_SIGNATURE_DESC descRootSignature;
	descRootSignature.NumParameters = ARRAYSIZE(rootParameters);
	descRootSignature.pParameters = rootParameters;
	descRootSignature.NumStaticSamplers = ARRAYSIZE(samplers);
	descRootSignature.pStaticSamplers = samplers;
	descRootSignature.Flags = D3D12_ROOT_SIGNATURE_FLAG_NONE;

	ID3DBlob* signature;
	ID3DBlob* error = nullptr;
	CheckResult(D3D12SerializeRootSignature(&descRootSignature, D3D_ROOT_SIGNATURE_VERSION_1, &signature, &error), error);
	CheckResult(m_device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&m_ltcTextureRS)));
	signature->Release();
}

void CRender::InitRootSignatures()
{
	InitGraphicsRootSignature();
	InitLTCTextureRootSignature();
}

void CRender::InitShaders()
{
	DXGI_FORMAT const objectDrawRenderTargets[] = { DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_R10G10B10A2_UNORM, DXGI_FORMAT_R8G8B8A8_UNORM };
	ERenderTargetBlendStates const objectDrawRenderTargetsBlend[] = { ERTBS_Disabled, ERTBS_Disabled, ERTBS_Disabled };
	m_shaders[ ST_OBJECT_DRAW ].InitShader( L"../shaders/objectDraw.hlsl", SSimpleObjectVertexFormat::desc, SSimpleObjectVertexFormat::descNum, 3, objectDrawRenderTargets, objectDrawRenderTargetsBlend, DXGI_FORMAT_D24_UNORM_S8_UINT, EDSS_DepthEnable );

	D3D_SHADER_MACRO geometryOnlyMacro[2];
	memset( geometryOnlyMacro, 0, sizeof( geometryOnlyMacro ) );
	geometryOnlyMacro[0].Name = "GEOMETRY_ONLY";
	m_shaders[ ST_OBJECT_DRAW_GEOMETRY_ONLY ].InitShader( L"../shaders/objectDraw.hlsl", SSimpleObjectVertexFormat::desc, SSimpleObjectVertexFormat::descNum, 0, nullptr, nullptr, DXGI_FORMAT_D32_FLOAT, EDSS_DepthEnable, ERS_Default, geometryOnlyMacro );

	DXGI_FORMAT const sdfDrawRenderTargets[] = { DXGI_FORMAT_R8G8B8A8_UNORM };
	ERenderTargetBlendStates const sdfDrawRenderTargetsBlend[] = { ERTBS_AlphaBlend };
	m_shaders[ ST_SDF_DRAW ].InitShader( L"../shaders/sdfDraw.hlsl", SPosUvVertexFormat::desc, SPosUvVertexFormat::descNum, 1, sdfDrawRenderTargets, sdfDrawRenderTargetsBlend );

	DXGI_FORMAT const envParticleDrawRenderTargets[] = { DXGI_FORMAT_R8G8B8A8_UNORM };
	ERenderTargetBlendStates const envParticleDrawRenderTargetsBlend[] = { ERTBS_RGBAdd };
	m_shaders[ ST_ENV_PARTICLE ].InitShader( L"../shaders/environmentParticleDraw.hlsl", nullptr, 0, 1, envParticleDrawRenderTargets, envParticleDrawRenderTargetsBlend, DXGI_FORMAT_D24_UNORM_S8_UINT, EDSS_DepthTest );

	DXGI_FORMAT const rectDrawRenderTargets[] = { DXGI_FORMAT_R8G8B8A8_UNORM };
	ERenderTargetBlendStates const rectDrawRenderTargetsBlend = ERTBS_Disabled;
	m_shaders[ ST_RECT_DRAW ].InitShader( L"../shaders/rectDraw.hlsl", nullptr, 0, 1, rectDrawRenderTargets, &rectDrawRenderTargetsBlend, DXGI_FORMAT_D24_UNORM_S8_UINT );

	DXGI_FORMAT const objectSimpleDrawRenderTargets[] = { DXGI_FORMAT_R8G8B8A8_UNORM };
	ERenderTargetBlendStates const objectSimpleDrawRenderTargetsBlend = ERTBS_Disabled;
	m_shaders[ ST_OBJECT_DRAW_SIMPLE ].InitShader( L"../shaders/objectDrawSimple.hlsl", SSimpleObjectVertexFormat::desc, SSimpleObjectVertexFormat::descNum, 1, objectSimpleDrawRenderTargets, &objectSimpleDrawRenderTargetsBlend, DXGI_FORMAT_D24_UNORM_S8_UINT, EDSS_DepthEnable );

	D3D_SHADER_MACRO objectSimpleTextureMacro[2];
	memset( objectSimpleTextureMacro, 0, sizeof( objectSimpleTextureMacro ) );
	objectSimpleTextureMacro[0].Name = "TEXTURE";
	m_shaders[ ST_OBJECT_DRAW_SIMPLE_TEXTURE ].InitShader( L"../shaders/objectDrawSimple.hlsl", SSimpleObjectVertexFormat::desc, SSimpleObjectVertexFormat::descNum, 1, objectSimpleDrawRenderTargets, &objectSimpleDrawRenderTargetsBlend, DXGI_FORMAT_D24_UNORM_S8_UINT, EDSS_DepthEnable, ERS_Default, objectSimpleTextureMacro );

	m_shaders[ST_LTC_TEXTURE].InitComputeShader( L"../shaders/ltcTexture.hlsl", m_ltcTextureRS );


	DXGI_FORMAT const lightRenderTargets[] = { DXGI_FORMAT_R8G8B8A8_UNORM };
	ERenderTargetBlendStates const lightRenderTargetsBlend[] = { ERTBS_RGBAdd };
	D3D_SHADER_MACRO lightDefinitions[ LF_MAX ];
	memset( lightDefinitions, 0, sizeof( lightDefinitions ) );

	UINT definitionID;
	for ( UINT i = 0; i < LF_LTC; ++i )
	{
		definitionID = 0;

		if ( i & LF_DIRECT )
		{
			lightDefinitions[ definitionID ].Name = "DIRECT";
			++definitionID;
		}

		if ( i & LF_POINT )
		{
			lightDefinitions[ definitionID ].Name = "POINT";
			++definitionID;
		}

		if ( i & LF_AMBIENT )
		{
			lightDefinitions[ definitionID ].Name = "AMBIENT";
			++definitionID;
		}

		lightDefinitions[ definitionID ].Name = nullptr;

		m_shaderLight[i].InitShader( L"../shaders/deferredLight.hlsl", SPosVertexFormat::desc, SPosVertexFormat::descNum, 1, lightRenderTargets, lightRenderTargetsBlend, DXGI_FORMAT_D24_UNORM_S8_UINT, EDSS_Disabled, ERS_Default, lightDefinitions );
	}

	m_shaderLight[LF_LTC].InitShader( L"../shaders/ltcLight.hlsl", SPosVertexFormat::desc, SPosVertexFormat::descNum, 1, lightRenderTargets, lightRenderTargetsBlend );

	lightDefinitions[ 0 ].Name = "TEXTURE";
	lightDefinitions[ 1 ].Name = nullptr;
	m_shaderLight[LF_LTC_TEXTURE].InitShader( L"../shaders/ltcLight.hlsl", SPosVertexFormat::desc, SPosVertexFormat::descNum, 1, lightRenderTargets, lightRenderTargetsBlend, DXGI_FORMAT_D24_UNORM_S8_UINT, EDSS_Disabled, ERS_Default, lightDefinitions );
}

void CRender::Init()
{
	m_texturesFreeIDs.Resize( MAX_TEXTURES - 1 );
	for ( UINT i = 0; i < (MAX_TEXTURES - 1); ++i )
	{
		m_texturesFreeIDs[ i ] = MAX_TEXTURES - i - 1;
	}

	m_constBufferOffset = 0;

	m_viewport.MaxDepth = 1.f;
	m_viewport.MinDepth = 0.f;
	m_viewport.Width = float(m_wndWidth);
	m_viewport.Height = float(m_wndHeight);
	m_viewport.TopLeftX = 0.f;
	m_viewport.TopLeftY = 0.f;

	m_scissorRect.right = (long)m_wndWidth;
	m_scissorRect.bottom = (long)m_wndHeight;
	m_scissorRect.left = 0;
	m_scissorRect.top = 0;

	m_fenceValue = 0;
	m_frameID = 0;

#ifdef _DEBUG
	CheckResult(D3D12GetDebugInterface(IID_PPV_ARGS(&m_debugController)));
	m_debugController->EnableDebugLayer();
#endif

	CheckResult(CreateDXGIFactory1(IID_PPV_ARGS(&m_factor)));
	CheckResult(D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&m_device)));

	CheckResult(m_device->CreateFence(m_fenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_fence)));
	++m_fenceValue;
	m_fenceEvent = CreateEventEx(nullptr, FALSE, FALSE, EVENT_ALL_ACCESS);

	m_geometryResources.Add( SGeometry() );

	memset( m_texturesResources, 0, sizeof( m_texturesResources ) );

	InitDescriptorHeaps();
	InitCommands();
	InitFrameData();
	InitConstBuffer();
	InitSwapChain();
	InitRenderTargets();
	InitRootSignatures();
	InitShaders();
}

void CRender::DrawRenderData( ID3D12GraphicsCommandList* commandList, TArray< SCommonRenderData > const& renderData )
{
	D3D_PRIMITIVE_TOPOLOGY currentTopology = D3D_PRIMITIVE_TOPOLOGY_UNDEFINED;
	Byte currentShader = ST_MAX;
	Byte currentGeometry = UINT8_MAX;
	Byte currentTexture[ SCommonRenderData::MAX_TEXTURES_NUM ];
	memset( currentTexture, 0, sizeof( currentTexture ) );

	D3D12_GPU_VIRTUAL_ADDRESS const constBufferStart = m_constBufferResource->GetGPUVirtualAddress();
	SCommonRenderData const* pRenderData = renderData.begin();
	SCommonRenderData const* const pRenderDataEnd = renderData.end();
	for (;pRenderData != pRenderDataEnd; ++pRenderData)
	{
		commandList->SetGraphicsRootConstantBufferView(1, constBufferStart + pRenderData->m_cbOffset );

		UINT const textureNum = pRenderData->m_texturesNum;
		for ( UINT textureID = 0; textureID < textureNum; ++textureID )
		{
			Byte const texture = m_texturesIDs[ pRenderData->m_texturesOffset + textureID ];
			if ( currentTexture[ textureID ] != texture )
			{
				if ( texture )
				{
					commandList->SetGraphicsRootDescriptorTable( textureID + 3, m_texturesDH.GetGPUDescriptorScaled( m_texturesDescriptorHeapOffset[ texture ] ) );
					currentTexture[ textureID ] = texture;
				}
			}
		}

		if ( currentTopology != pRenderData->m_topology )
		{
			commandList->IASetPrimitiveTopology( pRenderData->m_topology );
			currentTopology = pRenderData->m_topology;
		}

		if (currentShader != pRenderData->m_shaderID)
		{
			commandList->SetPipelineState( m_shaders[ pRenderData->m_shaderID ].GetPSO());
			currentShader = pRenderData->m_shaderID;
		}

		if ( currentGeometry != pRenderData->m_geometryID )
		{
			currentGeometry = pRenderData->m_geometryID;
			SGeometry const& geometry = m_geometryResources[ currentGeometry ];

			if ( geometry.m_indicesRes )
			{
				commandList->IASetIndexBuffer( &geometry.m_indexBufferView );
			}
			else
			{
				commandList->IASetIndexBuffer( nullptr );
			}

			if ( geometry.m_vertexRes )
			{
				commandList->IASetVertexBuffers( 0, 1, &geometry.m_vertexBufferView );
			}
			else
			{
				commandList->IASetVertexBuffers( 0, 0, nullptr );
			}
		}

		switch ( pRenderData->m_drawType )
		{
			case EDrawType::DrawIndexedInstanced:
				commandList->DrawIndexedInstanced( pRenderData->m_indicesNum, pRenderData->m_instancesNum, pRenderData->m_indicesStart, pRenderData->m_verticesStart, 0 );
				break;
			case EDrawType::DrawInstanced:
				commandList->DrawInstanced(pRenderData->m_indicesNum, pRenderData->m_instancesNum, pRenderData->m_verticesStart, 0);
				break;
			case EDrawType::DrawInvalid:
				ASSERT_STR( false, "Invalid draw type" );
				break;
		}
	}
}

void CRender::DrawLights( ID3D12GraphicsCommandList * commandList )
{
	commandList->SetGraphicsRootDescriptorTable( 3, m_texturesDH.GetGPUDescriptorScaled( m_globalBufferDescriptorsOffsets[ GBB_DIFFUSE_METALNESS ].m_srvOffset ) );
	commandList->SetGraphicsRootDescriptorTable( 4, m_texturesDH.GetGPUDescriptorScaled( m_globalBufferDescriptorsOffsets[ GBB_NORMAL ].m_srvOffset ) );
	commandList->SetGraphicsRootDescriptorTable( 5, m_texturesDH.GetGPUDescriptorScaled( m_globalBufferDescriptorsOffsets[ GBB_EMISSIVE_ROUGHNESS ].m_srvOffset ) );
	commandList->SetGraphicsRootDescriptorTable( 6, m_texturesDH.GetGPUDescriptorScaled( m_globalBufferDescriptorsOffsets[ GBB_DEPTH ].m_srvOffset ) );
	commandList->SetGraphicsRootDescriptorTable( 7, m_texturesDH.GetGPUDescriptorScaled( m_commonTexturesDescriptorHeapOffset[ ECommonTexuters::LTC_AMP ] ) );
	commandList->SetGraphicsRootDescriptorTable( 8, m_texturesDH.GetGPUDescriptorScaled( m_commonTexturesDescriptorHeapOffset[ ECommonTexuters::LTC_MAT ] ) );

	D3D12_GPU_VIRTUAL_ADDRESS const constBufferStart = m_constBufferResource->GetGPUVirtualAddress();

	UINT const lightNum = m_lightRenderData.Size();
	UINT lightShader = LF_MAX;
	for ( UINT lightID = 0; lightID < lightNum; ++lightID )
	{
		SLightRenderData const& light = m_lightRenderData[ lightID ];

		if ( light.m_texturesNum )
		{
			Byte const textureID = m_texturesIDs[ light.m_texturesOffset ];
			if ( textureID )
			{
				commandList->SetGraphicsRootDescriptorTable( 9, m_texturesDH.GetGPUDescriptorScaled( m_texturesDescriptorHeapOffset[ textureID ] ) );
			}
		}

		if ( lightShader != light.m_lightShader )
		{
			lightShader = light.m_lightShader;
			commandList->SetPipelineState( m_shaderLight[ lightShader ].GetPSO() );
		}

		commandList->SetGraphicsRootConstantBufferView(1, constBufferStart + light.m_cbOffset );
		DrawFullscreenTriangle( commandList );
	}
}

void CRender::DrawEnviroParticleRenderData( ID3D12GraphicsCommandList* commandList )
{
	D3D12_VIEWPORT viewport;
	D3D12_RECT scissorRect;
	viewport.MaxDepth = 1.f;
	viewport.MinDepth = 0.f;
	viewport.Width = 1024.f;
	viewport.Height = 1024.f;
	viewport.TopLeftX = 0.f;
	viewport.TopLeftY = 0.f;

	scissorRect.right = 1024;
	scissorRect.bottom = 1024;
	scissorRect.left = 0;
	scissorRect.top = 0;

	commandList->RSSetScissorRects(1, &scissorRect);
	commandList->RSSetViewports(1, &viewport);

	D3D12_CPU_DESCRIPTOR_HANDLE depthBufferDH = m_depthBuffertDH.GetCPUDescriptor( 1 );
	commandList->OMSetRenderTargets(0, nullptr, true, &depthBufferDH);
	commandList->ClearDepthStencilView( depthBufferDH, D3D12_CLEAR_FLAG_DEPTH, 1.f, 0, 0, nullptr );

	D3D_PRIMITIVE_TOPOLOGY currentTopology = D3D_PRIMITIVE_TOPOLOGY_UNDEFINED;
	Byte currentShader = ST_MAX;
	Byte currentGeometry = UINT8_MAX;

	D3D12_GPU_VIRTUAL_ADDRESS const constBufferStart = m_constBufferResource->GetGPUVirtualAddress();
	SShadowRenderData const* pRenderData = m_enviroParticleRenderData.begin();
	SShadowRenderData const* const pRenderDataEnd = m_enviroParticleRenderData.end();
	for (;pRenderData != pRenderDataEnd; ++pRenderData)
	{
		commandList->SetGraphicsRootConstantBufferView(1, constBufferStart + pRenderData->m_cbOffset );

		if ( currentTopology != pRenderData->m_topology )
		{
			commandList->IASetPrimitiveTopology( pRenderData->m_topology );
			currentTopology = pRenderData->m_topology;
		}

		if (currentShader != pRenderData->m_shaderID)
		{
			commandList->SetPipelineState( m_shaders[ pRenderData->m_shaderID ].GetPSO());
			currentShader = pRenderData->m_shaderID;
		}

		if ( currentGeometry != pRenderData->m_geometryID )
		{
			currentGeometry = pRenderData->m_geometryID;
			SGeometry const& geometry = m_geometryResources[ currentGeometry ];

			if ( geometry.m_indicesRes )
			{
				commandList->IASetIndexBuffer( &geometry.m_indexBufferView );
			}
			else
			{
				commandList->IASetIndexBuffer( nullptr );
			}

			if ( geometry.m_vertexRes )
			{
				commandList->IASetVertexBuffers( 0, 1, &geometry.m_vertexBufferView );
			}
			else
			{
				commandList->IASetVertexBuffers( 0, 0, nullptr );
			}
		}

		switch ( pRenderData->m_drawType )
		{
			case EDrawType::DrawIndexedInstanced:
				commandList->DrawIndexedInstanced( pRenderData->m_indicesNum, pRenderData->m_instancesNum, pRenderData->m_indicesStart, pRenderData->m_verticesStart, 0 );
				break;
			case EDrawType::DrawInstanced:
				commandList->DrawInstanced(pRenderData->m_indicesNum, pRenderData->m_instancesNum, pRenderData->m_verticesStart, 0);
				break;
			case EDrawType::DrawInvalid:
				ASSERT_STR( false, "Invalid draw type" );
				break;
		}
	}

	//commandList->RSSetScissorRects(1, &m_scissorRect);
	//commandList->RSSetViewports(1, &m_viewport);
}
void CRender::DrawDebug( ID3D12GraphicsCommandList* commandList )
{
#ifdef _DEBUG
	commandList->SetGraphicsRootDescriptorTable( 3, m_texturesDH.GetGPUDescriptorScaled( m_globalBufferDescriptorsOffsets[ GBB_RAIN_DEPTH ].m_srvOffset ) );
	commandList->SetPipelineState( m_shaders[ EShaderType::ST_RECT_DRAW ].GetPSO() );
	DrawRect( commandList, Vec4( -0.75f, -.75f, 0.5f, 0.5f ) );
#endif
}


void CRender::PrepareView()
{
	SComponentCamera mainCamera = GComponentCameraManager.GetMainCamera();
	SCameraMatrices& cameraMatrices = GViewObject.m_camera;

	cameraMatrices.m_viewToScreen = mainCamera.m_projectionMatrix;
	cameraMatrices.m_viewToScreen.Inverse( cameraMatrices.m_screenToView );

	cameraMatrices.m_viewToWorld = Matrix4x4::GetTranslateRotationSize( mainCamera.m_position, mainCamera.m_rotation, Vec3::ONE );
	cameraMatrices.m_viewToWorld.Inverse( cameraMatrices.m_worldToView );

	cameraMatrices.m_screenToWorld = Math::Mul( cameraMatrices.m_screenToView, cameraMatrices.m_viewToWorld );
	cameraMatrices.m_worldToScreen = Math::Mul( cameraMatrices.m_worldToView, cameraMatrices.m_viewToScreen );

	Matrix4x4 enviroParticleTranslate;
	enviroParticleTranslate.m_w = mainCamera.m_position + GEnvironmentParticleManager.GetProjectPositionOffset() + ( mainCamera.m_rotation * Vec3::FORWARD ) * GEnvironmentParticleManager.GetPositionOffset();
	GViewObject.m_enviroParticleWorldToScreen = Math::Mul( GEnvironmentParticleManager.GetViewToWorld(), enviroParticleTranslate );
	GViewObject.m_enviroParticleWorldToScreen.Inverse();
	GViewObject.m_enviroParticleWorldToScreen = Math::Mul( GViewObject.m_enviroParticleWorldToScreen, GEnvironmentParticleManager.GetViewToScreen() );
}

void CRender::PrepareGlobalConstBuffer()
{
	Matrix4x4 const scaleMatrix
	( 
		0.5f, 0.f, 0.f, 0.f,
		0.f, -0.5f, 0.f, 0.f,
		0.f, 0.f, 1.f, 0.f,
		0.5f, 0.5f, 0.f, 1.f 
	);

	SCameraMatrices const& cameraMatrices = GViewObject.m_camera;
	SGlobalBuffer globalCB;
	globalCB.m_enviroProjection = Math::Mul( GViewObject.m_enviroParticleWorldToScreen, scaleMatrix );
	globalCB.m_enviroProjection.Transpose();

	globalCB.m_worldToScreen = GViewObject.m_camera.m_worldToScreen;
	globalCB.m_worldToScreen.Transpose();

	Matrix4x4 tViewToWorld = cameraMatrices.m_viewToWorld;
	tViewToWorld.Transpose();
	memcpy( globalCB.m_viewToWorld, &tViewToWorld, 3 * sizeof( Vec4 ) );

	globalCB.m_perspectiveValues.Set(1.f / cameraMatrices.m_viewToScreen.m_a00, 1.f / cameraMatrices.m_viewToScreen.m_a11, cameraMatrices.m_viewToScreen.m_a32, -cameraMatrices.m_viewToScreen.m_a22 );
	globalCB.m_cameraPositionWS = GComponentCameraManager.GetMainCameraPosition();
	globalCB.m_deltaTime = GTimer.GameDelta();
	globalCB.m_time = GTimer.GetSeconds( GTimer.TimeFromStart() );

	GRender.SetConstBuffer( m_globalConstBufferAddress, (Byte*)&globalCB, sizeof( globalCB ) );
}

void CRender::PreDrawFrame()
{
	PrepareView();
	PrepareGlobalConstBuffer();

	GComponentStaticMeshManager.FillRenderData();
	GComponentStaticMeshManager.FillEnviroParticleRenderData();
	GComponentLightManager.FillRenderData();
	GEnvironmentParticleManager.FillRenderData();
	GTextRenderManager.FillRenderData();
	GDynamicGeometryManager.PreDraw();

	WaitForGraphicsQueue();
}

void CRender::DrawFrame()
{
	ID3D12GraphicsCommandList* commandList = m_frameData[m_frameID].m_frameCL;
	D3D12_CPU_DESCRIPTOR_HANDLE const frameRT = m_renderTargetDH.GetCPUDescriptor( m_frameID );
	m_frameData[m_frameID].m_frameCA->Reset();
	commandList->Reset( m_frameData[ m_frameID ].m_frameCA, nullptr );
	commandList->SetDescriptorHeaps(1, &m_texturesDH.m_pDescriptorHeap);
	commandList->SetGraphicsRootSignature(m_graphicsRS);
	commandList->SetGraphicsRootConstantBufferView( 0, m_globalConstBufferAddress );
	commandList->SetGraphicsRootShaderResourceView( 2, GEnvironmentParticleManager.GetParticlesBufferAddress() );

	D3D12_RESOURCE_BARRIER barriers[ 6 ];
	barriers[0].Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barriers[0].Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barriers[0].Transition.pResource = m_rederTarget[m_frameID];
	barriers[0].Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
	barriers[0].Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
	barriers[0].Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

	barriers[1].Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barriers[1].Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barriers[1].Transition.pResource = m_globalBufferBuffers[ GBB_DIFFUSE_METALNESS ];
	barriers[1].Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
	barriers[1].Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	barriers[1].Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

	barriers[2].Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barriers[2].Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barriers[2].Transition.pResource = m_globalBufferBuffers[ GBB_NORMAL ];
	barriers[2].Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
	barriers[2].Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	barriers[2].Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

	barriers[3].Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barriers[3].Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barriers[3].Transition.pResource = m_globalBufferBuffers[ GBB_EMISSIVE_ROUGHNESS ];
	barriers[3].Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
	barriers[3].Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	barriers[3].Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

	barriers[4].Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barriers[4].Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barriers[4].Transition.pResource = m_globalBufferBuffers[ GBB_DEPTH ];
	barriers[4].Transition.StateAfter = D3D12_RESOURCE_STATE_DEPTH_READ | D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
	barriers[4].Transition.StateBefore = D3D12_RESOURCE_STATE_DEPTH_WRITE;
	barriers[4].Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

	barriers[5].Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barriers[5].Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barriers[5].Transition.pResource = m_globalBufferBuffers[ GBB_RAIN_DEPTH ];
	barriers[5].Transition.StateAfter = D3D12_RESOURCE_STATE_DEPTH_READ | D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
	barriers[5].Transition.StateBefore = D3D12_RESOURCE_STATE_DEPTH_WRITE;
	barriers[5].Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

	DrawEnviroParticleRenderData( commandList );

	commandList->RSSetScissorRects(1, &m_scissorRect);
	commandList->RSSetViewports(1, &m_viewport);

	D3D12_CPU_DESCRIPTOR_HANDLE renderTargetDH = m_renderTargetDH.GetCPUDescriptor( m_globalBufferDescriptorsOffsets[ GBB_DIFFUSE_METALNESS ].m_rtvOffset );
	D3D12_CPU_DESCRIPTOR_HANDLE depthBufferDH = m_depthBuffertDH.GetCPUDescriptor( 0 );
	commandList->OMSetRenderTargets(3, &renderTargetDH, true, &depthBufferDH);

	commandList->ClearDepthStencilView( depthBufferDH, D3D12_CLEAR_FLAG_DEPTH, 1.f, 0, 0, nullptr );
	commandList->ClearRenderTargetView( renderTargetDH, Vec4::ZERO.data, 0, nullptr );
	commandList->ClearRenderTargetView( m_renderTargetDH.GetCPUDescriptor( m_globalBufferDescriptorsOffsets[ GBB_NORMAL ].m_rtvOffset), Vec4::ZERO.data, 0, nullptr );
	commandList->ClearRenderTargetView( m_renderTargetDH.GetCPUDescriptor( m_globalBufferDescriptorsOffsets[ GBB_EMISSIVE_ROUGHNESS ].m_rtvOffset), Vec4::ZERO.data, 0, nullptr );

	DrawRenderData( commandList, m_commonRenderData[RL_OPAQUE] );
	commandList->ResourceBarrier(6, barriers);

	commandList->ClearRenderTargetView( frameRT, Vec4::ZERO.data, 0, nullptr );
	commandList->OMSetRenderTargets(1, &frameRT, true, &depthBufferDH);

	DrawLights( commandList );

	commandList->SetGraphicsRootDescriptorTable( 7, m_texturesDH.GetGPUDescriptorScaled( m_globalBufferDescriptorsOffsets[ GBB_RAIN_DEPTH ].m_srvOffset ) );

	DrawRenderData( commandList, m_commonRenderData[RL_TRANSLUCENT] );
	
	//DrawDebug( commandList );
	
	barriers[1].Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
	barriers[1].Transition.StateBefore = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;

	barriers[2].Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
	barriers[2].Transition.StateBefore = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;

	barriers[3].Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
	barriers[3].Transition.StateBefore = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;

	barriers[4].Transition.StateAfter = D3D12_RESOURCE_STATE_DEPTH_WRITE;
	barriers[4].Transition.StateBefore = D3D12_RESOURCE_STATE_DEPTH_READ | D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;

	barriers[5].Transition.StateAfter = D3D12_RESOURCE_STATE_DEPTH_WRITE;
	barriers[5].Transition.StateBefore = D3D12_RESOURCE_STATE_DEPTH_READ | D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;

	commandList->ResourceBarrier(5, &barriers[1]);

	DrawRenderData( commandList, m_commonRenderData[RL_UNLIT] );
	DrawRenderData( commandList, m_commonRenderData[RL_OVERLAY] );

	barriers[0].Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
	barriers[0].Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;

	commandList->ResourceBarrier(1, barriers);

	CheckResult(commandList->Close());
	m_graphicsCQ->ExecuteCommandLists(1, (ID3D12CommandList**)(&commandList));

	CheckResult(m_swapChain->Present(1, 0));

	m_texturesIDs.Clear();
	m_constBufferOffset = 0;

	for (UINT layerID = 0; layerID < RL_MAX; ++layerID)
	{
		m_commonRenderData[layerID].Clear();
	}
	m_lightRenderData.Clear();
	m_enviroParticleRenderData.Clear();
	m_frameID = m_swapChain->GetCurrentBackBufferIndex();
}

void CRender::Release()
{
	UINT const endFenceValue = m_fence->GetCompletedValue() + 1;
	CheckResult(m_graphicsCQ->Signal(m_fence, endFenceValue));
	CheckResult(m_fence->SetEventOnCompletion(endFenceValue, m_fenceEvent));
	WaitForSingleObject(m_fenceEvent, INFINITE);

	GEnvironmentParticleManager.Release();

	m_copyCQ->Release();
	m_copyCA->Release();
	m_copyCL->Release();

	m_graphicsCQ->Release();
	m_graphicsCA->Release();
	m_graphicsCL->Release();

	m_computeCQ->Release();
	m_computeCA->Release();
	m_computeCL->Release();

	for (unsigned int shaderID = 0; shaderID < ST_MAX; ++shaderID)
	{
		if ( m_shaders[ shaderID ].IsValid() )
		{
			m_shaders[ shaderID ].Release();
		}
	}
	for ( unsigned int shaderID = 0; shaderID < LF_MAX; ++shaderID )
	{
		if ( m_shaderLight[ shaderID ].IsValid() )
		{
			m_shaderLight[ shaderID ].Release();
		}
	}

	m_graphicsRS->Release();
	m_ltcTextureRS->Release();
	m_renderTargetDH.Release();
	m_texturesDH.Release();
	for (UINT frameID = 0; frameID < FRAME_NUM; ++frameID)
	{
		m_rederTarget[frameID]->Release();

		SRenderFrameData& frameData = m_frameData[frameID];
		frameData.m_frameCA->Release();
		frameData.m_frameCL->Release();
	}

	m_constBufferResource->Unmap(0, nullptr);
	m_constBufferResource->Release();
	m_pConstBufferData = nullptr;

	for ( UINT gbufferID = 0; gbufferID < GBB_MAX; ++gbufferID )
	{
		m_globalBufferBuffers[ gbufferID ]->Release();
	}

	m_depthBuffertDH.Release();
	m_fullscreenTriangleRes->Release();

	UINT const geometryNum = m_geometryResources.Size();
	for (UINT geometryID = 0; geometryID < geometryNum; ++geometryID)
	{
		m_geometryResources[geometryID].Release();
	}

	tTextureResManager::iterator tEnd =	GTextureResources.end();
	for (tTextureResManager::iterator itr = GTextureResources.begin(); itr != tEnd; itr++)
	{
		m_texturesResources[ itr->second ]->Release();
	}

	m_swapChain->Release();
	m_fence->Release();
	m_factor->Release();
	m_device->Release();

#ifdef _DEBUG
	m_debugController->Release();
#endif
}

Byte CRender::AddGeometry( SGeometry const& geometry )
{
	Byte const geometryID = Byte( m_geometryResources.Size() );
	m_geometryResources.Add( geometry );

	return geometryID;
}

SGeometry& CRender::GetGeometry( Byte const geometryID )
{
	return m_geometryResources[ geometryID ];
}

void CRender::ReleaseGeometry( Byte const geometryID )
{
	m_geometryResources[ geometryID ].Release();
}

void CRender::DrawFullscreenTriangle( ID3D12GraphicsCommandList* commandList )
{
	commandList->IASetPrimitiveTopology( D3D_PRIMITIVE_TOPOLOGY::D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
	commandList->IASetVertexBuffers( 0, 1, &m_fullscreenTriangleView );
	commandList->DrawInstanced( 3, 1, 0, 0 );
}

void CRender::DrawRect( ID3D12GraphicsCommandList* commandList, Vec4 screenPositionSize )
{
	screenPositionSize.z *= 0.5f;
	screenPositionSize.w *= 0.5f;

	D3D12_GPU_VIRTUAL_ADDRESS constBufferAddress;
	GRender.SetConstBuffer( constBufferAddress, (Byte*)&screenPositionSize, sizeof( screenPositionSize ) );

	commandList->SetGraphicsRootConstantBufferView( 1, constBufferAddress );
	commandList->IASetPrimitiveTopology( D3D_PRIMITIVE_TOPOLOGY::D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP );
	commandList->DrawInstanced( 4, 1, 0, 0 );
}

void CRender::CreateFullscreenTriangleRes()
{
	UINT const verticesNum = 3;

	D3D12_RESOURCE_DESC descVertices = {};
	descVertices.DepthOrArraySize = 1;
	descVertices.SampleDesc.Count = 1;
	descVertices.Flags = D3D12_RESOURCE_FLAG_NONE;
	descVertices.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	descVertices.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	descVertices.MipLevels = 1;
	descVertices.Width = verticesNum * sizeof( SPosVertexFormat );
	descVertices.Height = 1;
	descVertices.Format = DXGI_FORMAT_UNKNOWN;

	CheckResult( m_device->CreateCommittedResource( &GHeapPropertiesGPUOnly, D3D12_HEAP_FLAG_NONE, &descVertices, D3D12_RESOURCE_STATE_COPY_DEST, nullptr, IID_PPV_ARGS( &m_fullscreenTriangleRes ) ) );

	ID3D12Resource* verticesUploadRes;
	CheckResult(m_device->CreateCommittedResource(&GHeapPropertiesUpload, D3D12_HEAP_FLAG_NONE, &descVertices, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&verticesUploadRes)));
	m_uploadResources.Add(verticesUploadRes);
	void* pGPU;
	verticesUploadRes->Map(0, nullptr, &pGPU);

	SPosVertexFormat const vertices[] =
	{
		Vec2( 3.f, -1.f ),
		Vec2( -1.f, -1.f ),
		Vec2( -1.f, 3.f ),
	};
	memcpy( pGPU, vertices, sizeof( SPosVertexFormat ) * verticesNum );
	verticesUploadRes->Unmap( 0, nullptr );
	m_copyCL->CopyResource( m_fullscreenTriangleRes, verticesUploadRes );

	m_fullscreenTriangleView.BufferLocation = m_fullscreenTriangleRes->GetGPUVirtualAddress();
	m_fullscreenTriangleView.SizeInBytes = verticesNum * sizeof( SPosVertexFormat );
	m_fullscreenTriangleView.StrideInBytes = sizeof( SPosVertexFormat );

	D3D12_RESOURCE_BARRIER barrier;
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = m_fullscreenTriangleRes;
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER;
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	m_resourceBarrier.Add( barrier );
}

void CRender::CreateCommonTextures()
{
	wchar_t const* textures[] =
	{
		L"../content/textures/common/ltc_amp.dds",
		L"../content/textures/common/ltc_mat.dds",
	};
	CT_ASSERT( ARRAYSIZE( textures ) == COMMON_TEXTURE_MAX );

	for (unsigned int texture = 0; texture < ARRAYSIZE(textures); ++texture)
	{
		DirectX::TexMetadata texMeta;
		DirectX::ScratchImage image;
		if ( DirectX::LoadFromWICFile( textures[ texture ], DirectX::WIC_FLAGS_NONE, &texMeta, image ) != S_OK )
		{
			CheckResult( DirectX::LoadFromDDSFile( textures[ texture ], DirectX::DDS_FLAGS_NONE, &texMeta, image ) );
		}

		STextureInfo textureInfo;
		textureInfo.m_width = UINT(texMeta.width);
		textureInfo.m_height = UINT(texMeta.height);
		textureInfo.m_format = texMeta.format;
		textureInfo.m_mipLevels = Byte(texMeta.mipLevels);

		ASSERT( textureInfo.m_format != DXGI_FORMAT_UNKNOWN );
		UINT16 const textureID = GRender.LoadResource( textureInfo, image.GetPixels() );
		GTextureResources[ textures[ texture ] ] = textureID;
		m_commonTexturesDescriptorHeapOffset[ texture ] = m_texturesDescriptorHeapOffset[ textureID ];
	}
}

void CRender::BeginLoadResources(unsigned int const textureNum)
{
	m_copyCA->Reset();
	m_copyCL->Reset(m_copyCA, nullptr);
	CreateFullscreenTriangleRes();
	CreateCommonTextures();
}

UINT16 CRender::LoadResource( STextureInfo const& texture, Byte const* const data )
{
	UINT16 const textureFreeID = m_texturesFreeIDs.PopBack();
	m_texturesInfo[ textureFreeID ] = texture;

	D3D12_RESOURCE_DESC descTexture = {};
	descTexture.DepthOrArraySize = 1;
	descTexture.SampleDesc.Count = 1;
	descTexture.Flags = D3D12_RESOURCE_FLAG_NONE;
	descTexture.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	descTexture.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	descTexture.MipLevels = texture.m_mipLevels;
	descTexture.Width = texture.m_width;
	descTexture.Height = texture.m_height;
	descTexture.Format = texture.m_format;

	ID3D12Resource* textureRes;
	CheckResult(m_device->CreateCommittedResource(&GHeapPropertiesGPUOnly, D3D12_HEAP_FLAG_NONE, &descTexture, D3D12_RESOURCE_STATE_COPY_DEST, nullptr, IID_PPV_ARGS(&textureRes)));
	m_texturesResources[ textureFreeID ] = textureRes;

	UINT64 bufferSize;
	D3D12_PLACED_SUBRESOURCE_FOOTPRINT* pFootprints = new D3D12_PLACED_SUBRESOURCE_FOOTPRINT[ texture.m_mipLevels ];
	UINT* pNumRows = new UINT[ texture.m_mipLevels ];
	UINT64* pRowPitches = new UINT64[ texture.m_mipLevels ];
	m_device->GetCopyableFootprints(&descTexture, 0, texture.m_mipLevels, 0, pFootprints, pNumRows, pRowPitches, &bufferSize);

	descTexture.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	descTexture.Format = DXGI_FORMAT_UNKNOWN;
	descTexture.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	descTexture.MipLevels = 1;
	descTexture.Height = 1;
	descTexture.Width = bufferSize;

	ID3D12Resource* textureUploadRes;
	CheckResult(m_device->CreateCommittedResource(&GHeapPropertiesUpload, D3D12_HEAP_FLAG_NONE, &descTexture, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&textureUploadRes)));
	m_uploadResources.Add(textureUploadRes);

	void* pGPU;
	textureUploadRes->Map(0, nullptr, &pGPU);

	Byte const* srcRow = data;
	for ( UINT mipLevel = 0; mipLevel < texture.m_mipLevels; ++mipLevel )
	{
		for ( UINT rowID = 0; rowID < pNumRows[ mipLevel ]; ++rowID )
		{
			memcpy( ( BYTE* )pGPU + pFootprints[ mipLevel ].Offset + rowID * pFootprints[ mipLevel ].Footprint.RowPitch, srcRow, pRowPitches[ mipLevel ] );
			srcRow += pRowPitches[ mipLevel ];
		}

		D3D12_TEXTURE_COPY_LOCATION dst;
		dst.pResource = textureRes;
		dst.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
		dst.SubresourceIndex = mipLevel;

		D3D12_TEXTURE_COPY_LOCATION src;
		src.pResource = textureUploadRes;
		src.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
		src.PlacedFootprint = pFootprints[ mipLevel ];

		m_copyCL->CopyTextureRegion( &dst, 0, 0, 0, &src, nullptr );
	}
	textureUploadRes->Unmap(0, nullptr);

	delete[] pFootprints;
	delete[] pNumRows;
	delete[] pRowPitches;

	D3D12_RESOURCE_BARRIER barrier;
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = textureRes;
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	m_resourceBarrier.Add( barrier );

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Texture2D.MipLevels = texture.m_mipLevels;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Format = texture.m_format;

	m_texturesDescriptorHeapOffset[ textureFreeID ] = m_texturesDHFree.PopBack();
	m_device->CreateShaderResourceView( textureRes, &srvDesc, m_texturesDH.GetCPUDescriptorScaled( m_texturesDescriptorHeapOffset[ textureFreeID ] ) );

	return textureFreeID;
}

Byte CRender::LoadResource( SGeometryData const & geometryData )
{
	SGeometry geometry;

	UINT const verticesNum = geometryData.m_vertices.Size();
	UINT const indicesNum = geometryData.m_indices.Size();

	D3D12_RESOURCE_DESC descVertices = {};
	descVertices.DepthOrArraySize = 1;
	descVertices.SampleDesc.Count = 1;
	descVertices.Flags = D3D12_RESOURCE_FLAG_NONE;
	descVertices.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	descVertices.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	descVertices.MipLevels = 1;
	descVertices.Width = verticesNum * sizeof( SSimpleObjectVertexFormat );
	descVertices.Height = 1;
	descVertices.Format = DXGI_FORMAT_UNKNOWN;

	CheckResult( m_device->CreateCommittedResource( &GHeapPropertiesGPUOnly, D3D12_HEAP_FLAG_NONE, &descVertices, D3D12_RESOURCE_STATE_COPY_DEST, nullptr, IID_PPV_ARGS( &geometry.m_vertexRes ) ) );

	ID3D12Resource* verticesUploadRes;
	CheckResult(m_device->CreateCommittedResource(&GHeapPropertiesUpload, D3D12_HEAP_FLAG_NONE, &descVertices, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&verticesUploadRes)));
	m_uploadResources.Add(verticesUploadRes);
	void* pGPU;
	verticesUploadRes->Map(0, nullptr, &pGPU);
	memcpy( pGPU, geometryData.m_vertices.Data(), sizeof( SSimpleObjectVertexFormat ) * verticesNum );
	verticesUploadRes->Unmap( 0, nullptr );
	m_copyCL->CopyResource( geometry.m_vertexRes, verticesUploadRes );

	D3D12_RESOURCE_DESC descIndices = {};
	descIndices.DepthOrArraySize = 1;
	descIndices.SampleDesc.Count = 1;
	descIndices.Flags = D3D12_RESOURCE_FLAG_NONE;
	descIndices.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	descIndices.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	descIndices.MipLevels = 1;
	descIndices.Width = indicesNum * sizeof( UINT16 );
	descIndices.Height = 1;
	descIndices.Format = DXGI_FORMAT_UNKNOWN;

	CheckResult( m_device->CreateCommittedResource( &GHeapPropertiesGPUOnly, D3D12_HEAP_FLAG_NONE, &descIndices, D3D12_RESOURCE_STATE_COPY_DEST, nullptr, IID_PPV_ARGS( &geometry.m_indicesRes ) ) );

	ID3D12Resource* indicesUploadRes;
	CheckResult(m_device->CreateCommittedResource(&GHeapPropertiesUpload, D3D12_HEAP_FLAG_NONE, &descIndices, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&indicesUploadRes)));
	m_uploadResources.Add(indicesUploadRes);
	indicesUploadRes->Map(0, nullptr, &pGPU);
	memcpy( pGPU, geometryData.m_indices.Data(), sizeof( UINT16 ) * indicesNum );
	indicesUploadRes->Unmap( 0, nullptr );
	m_copyCL->CopyResource( geometry.m_indicesRes, indicesUploadRes );

	geometry.m_vertexBufferView.BufferLocation = geometry.m_vertexRes->GetGPUVirtualAddress();
	geometry.m_vertexBufferView.SizeInBytes = verticesNum * sizeof( SSimpleObjectVertexFormat );
	geometry.m_vertexBufferView.StrideInBytes = sizeof( SSimpleObjectVertexFormat );

	geometry.m_indexBufferView.BufferLocation = geometry.m_indicesRes->GetGPUVirtualAddress();
	geometry.m_indexBufferView.Format = DXGI_FORMAT_R16_UINT;
	geometry.m_indexBufferView.SizeInBytes = indicesNum * sizeof( UINT16 );

	Byte const geometryID = Byte( m_geometryResources.Size() );
	m_geometryResources.Add( geometry );

	D3D12_RESOURCE_BARRIER barrier;
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = geometry.m_vertexRes;
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER;
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	m_resourceBarrier.Add( barrier );

	barrier.Transition.pResource = geometry.m_indicesRes;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_INDEX_BUFFER;
	m_resourceBarrier.Add( barrier );

	return geometryID;
}

void CRender::EndLoadResources()
{
	m_copyCL->Close();
	m_copyCQ->ExecuteCommandLists(1, (ID3D12CommandList**)(&m_copyCL));

	GTextRenderManager.Init();
	GEnvironmentParticleManager.Init( 128, 2, 10.f );
}

void CRender::ExecuteComputeQueue( UINT const commandListNum, ID3D12CommandList* const* pCommandLists )
{
	m_computeCQ->ExecuteCommandLists( commandListNum, pCommandLists );
}

void CRender::WaitForResourcesLoad()
{
	m_graphicsCA->Reset();
	m_graphicsCL->Reset(m_graphicsCA, nullptr);

	m_graphicsCL->ResourceBarrier(m_resourceBarrier.Size(), m_resourceBarrier.Data());
	m_graphicsCL->Close();

	m_copyCQ->Signal( m_fence, m_fenceValue );
	m_graphicsCQ->Wait( m_fence, m_fenceValue );
	m_graphicsCQ->ExecuteCommandLists(1, (ID3D12CommandList**)(&m_graphicsCL));
	++m_fenceValue;

	WaitForGraphicsQueue();

	UINT const uploadResNum = m_uploadResources.Size();
	for ( UINT uploadResID = 0; uploadResID < uploadResNum; ++uploadResID )
	{
		m_uploadResources[uploadResID]->Release();
	}
	m_uploadResources.Free();
}

CConstBufferCtx CRender::GetConstBufferCtx( D3D12_GPU_VIRTUAL_ADDRESS& outCbOffset, Byte const shader )
{
	UINT16 const cbSize = m_shaders[ shader ].GetBufferSize();
	ASSERT_STR( ( cbSize & 0xFF ) == 0, "Const buffer not alignet to 256B" );
	ASSERT_STR( m_constBufferOffset + cbSize <= 1024 * MAX_OBJECTS, "Not enough space for constant buffer" );
	Byte* constBufferPtr = &m_pConstBufferData[ m_constBufferOffset ];
	outCbOffset = m_constBufferOffset;
	m_constBufferOffset += cbSize;

	return CConstBufferCtx( &m_shaders[ shader ], constBufferPtr );
}

CConstBufferCtx CRender::GetLightConstBufferCtx( D3D12_GPU_VIRTUAL_ADDRESS& outCbOffset, Byte const shader )
{
	UINT16 const cbSize = m_shaderLight[ shader ].GetBufferSize();
	ASSERT_STR( ( cbSize & 0xFF ) == 0, "Const buffer not alignet ty 256B" );
	ASSERT_STR( m_constBufferOffset + cbSize <= 1024 * MAX_OBJECTS, "Not enough space for constant buffer" );
	Byte* constBufferPtr = &m_pConstBufferData[ m_constBufferOffset ];
	outCbOffset = m_constBufferOffset;
	m_constBufferOffset += cbSize;

	return CConstBufferCtx( &m_shaderLight[ shader ], constBufferPtr );
}

void CRender::SetConstBuffer( D3D12_GPU_VIRTUAL_ADDRESS& outConstBufferAddress, Byte* const pData, UINT const size )
{
	UINT const cbSize = (size + 255) & ~255;
	ASSERT_STR( ( cbSize & 0xFF ) == 0, "Const buffer not alignet ty 256B" );
	ASSERT_STR( m_constBufferOffset + cbSize <= 1024 * MAX_OBJECTS, "Not enough space for constant buffer" );

	Byte* constBufferPtr = &m_pConstBufferData[ m_constBufferOffset ];
	outConstBufferAddress = m_constBufferResource->GetGPUVirtualAddress() + m_constBufferOffset;
	memcpy( constBufferPtr, pData, size );

	m_constBufferOffset += cbSize;
}

void CRender::WaitForCopyQueue()
{
	CheckResult(m_copyCQ->Signal(m_fence, m_fenceValue));
	CheckResult(m_fence->SetEventOnCompletion(m_fenceValue, m_fenceEvent));
	WaitForSingleObject(m_fenceEvent, INFINITE);
	++m_fenceValue;
}
void CRender::WaitForGraphicsQueue()
{
	CheckResult(m_graphicsCQ->Signal(m_fence, m_fenceValue));
	CheckResult(m_fence->SetEventOnCompletion(m_fenceValue, m_fenceEvent));
	WaitForSingleObject(m_fenceEvent, INFINITE);
	++m_fenceValue;
}
void CRender::WaitForComputeQueue()
{
	CheckResult(m_computeCQ->Signal(m_fence, m_fenceValue));
	CheckResult(m_fence->SetEventOnCompletion(m_fenceValue, m_fenceEvent));
	WaitForSingleObject(m_fenceEvent, INFINITE);
	++m_fenceValue;
}

void CRender::ReinitShaders()
{
	UINT const endFenceValue = m_fence->GetCompletedValue() + 1;
	CheckResult(m_graphicsCQ->Signal(m_fence, endFenceValue));
	CheckResult(m_fence->SetEventOnCompletion(endFenceValue, m_fenceEvent));
	WaitForSingleObject(m_fenceEvent, INFINITE);

	for (unsigned int shaderID = 0; shaderID < ST_MAX; ++shaderID)
	{
		if ( m_shaders[ shaderID ].IsValid() )
		{
			m_shaders[ shaderID ].Release();
		}
	}
	for ( unsigned int shaderID = 0; shaderID < LF_MAX; ++shaderID )
	{
		if ( m_shaderLight[ shaderID ].IsValid() )
		{
			m_shaderLight[ shaderID ].Release();
		}
	}

	InitShaders();
}


void CConstBufferCtx::SetParam( void const* pData, UINT16 const size, EShaderParameters const param ) const
{
	UINT16 const paramOffset = m_shader->GetOffset( param );
	if ( paramOffset != 0xFFFF )
	{
		memcpy( m_pConstBuffer + paramOffset, pData, size );
	}
}
