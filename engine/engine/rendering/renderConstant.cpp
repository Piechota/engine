#include "../headers.h"
#include "renderConstant.h"

D3D12_RASTERIZER_DESC const GRasterizerStates[ ERS_MAX ] =
{
	//ERS_Default
	{
		/*FillMode*/					D3D12_FILL_MODE_SOLID
		/*CullMode*/					,D3D12_CULL_MODE_BACK
		/*FrontCounterClockwise*/		,FALSE
		/*DepthBias*/					,D3D12_DEFAULT_DEPTH_BIAS
		/*DepthBiasClamp*/				,D3D12_DEFAULT_DEPTH_BIAS_CLAMP
		/*SlopeScaledDepthBias*/		,D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS
		/*DepthClipEnable*/				,FALSE
		/*MultisampleEnable*/			,FALSE
		/*AntialiasedLineEnable*/		,FALSE
		/*ForcedSampleCount*/			,0
		/*ConservativeRaster*/			,D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF
	},
};

D3D12_RENDER_TARGET_BLEND_DESC const GRenderTargetBlendStates[ ERTBS_MAX ] =
{
	//ERTBS_Disabled
	{
		/*RenderTarget[0].BlendEnable*/				FALSE
		/*RenderTarget[0].LogicOpEnable*/			,FALSE
		/*RenderTarget[0].SrcBlend*/				,D3D12_BLEND_ONE
		/*RenderTarget[0].DestBlend*/				,D3D12_BLEND_ZERO
		/*RenderTarget[0].BlendOp*/					,D3D12_BLEND_OP_ADD
		/*RenderTarget[0].SrcBlendAlpha*/			,D3D12_BLEND_SRC_ALPHA
		/*RenderTarget[0].DestBlendAlpha*/			,D3D12_BLEND_INV_SRC_ALPHA
		/*RenderTarget[0].BlendOpAlpha*/			,D3D12_BLEND_OP_ADD
		/*RenderTarget[0].LogicOp*/					,D3D12_LOGIC_OP_NOOP
		/*RenderTarget[0].RenderTargetWriteMask*/	,D3D12_COLOR_WRITE_ENABLE_ALL
	},

	//ERTBS_AlphaBlend
	{
		/*RenderTarget[0].BlendEnable*/				TRUE
		/*RenderTarget[0].LogicOpEnable*/			,FALSE
		/*RenderTarget[0].SrcBlend*/				,D3D12_BLEND_SRC_ALPHA
		/*RenderTarget[0].DestBlend*/				,D3D12_BLEND_INV_SRC_ALPHA
		/*RenderTarget[0].BlendOp*/					,D3D12_BLEND_OP_ADD
		/*RenderTarget[0].SrcBlendAlpha*/			,D3D12_BLEND_SRC_ALPHA
		/*RenderTarget[0].DestBlendAlpha*/			,D3D12_BLEND_INV_SRC_ALPHA
		/*RenderTarget[0].BlendOpAlpha*/			,D3D12_BLEND_OP_ADD
		/*RenderTarget[0].LogicOp*/					,D3D12_LOGIC_OP_NOOP
		/*RenderTarget[0].RenderTargetWriteMask*/	,D3D12_COLOR_WRITE_ENABLE_ALL
	},

	//ERTBS_RGBAdd
	{
		/*RenderTarget[0].BlendEnable*/				TRUE
		/*RenderTarget[0].LogicOpEnable*/			,FALSE
		/*RenderTarget[0].SrcBlend*/				,D3D12_BLEND_ONE
		/*RenderTarget[0].DestBlend*/				,D3D12_BLEND_ONE
		/*RenderTarget[0].BlendOp*/					,D3D12_BLEND_OP_ADD
		/*RenderTarget[0].SrcBlendAlpha*/			,D3D12_BLEND_ONE
		/*RenderTarget[0].DestBlendAlpha*/			,D3D12_BLEND_ZERO
		/*RenderTarget[0].BlendOpAlpha*/			,D3D12_BLEND_OP_ADD
		/*RenderTarget[0].LogicOp*/					,D3D12_LOGIC_OP_NOOP
		/*RenderTarget[0].RenderTargetWriteMask*/	,D3D12_COLOR_WRITE_ENABLE_ALL
	},
};

D3D12_DEPTH_STENCIL_DESC const GDepthStencilStates[ EDSS_MAX ] =
{
	//EDSS_Disabled
	{
		/*DepthEnable*/							FALSE
		/*DepthWriteMask*/						,D3D12_DEPTH_WRITE_MASK_ALL
		/*DepthFunc*/							,D3D12_COMPARISON_FUNC_LESS
		/*StencilEnable*/						,FALSE
		/*StencilReadMask*/						,D3D12_DEFAULT_STENCIL_READ_MASK
		/*StencilWriteMask*/					,D3D12_DEFAULT_STENCIL_WRITE_MASK
		/*FrontFace.StencilFailOp*/				,{ D3D12_STENCIL_OP_REPLACE
		/*FrontFace.StencilDepthFailOp*/		,D3D12_STENCIL_OP_REPLACE
		/*FrontFace.StencilPassOp*/				,D3D12_STENCIL_OP_REPLACE
		/*FrontFace.StencilFunc*/				,D3D12_COMPARISON_FUNC_ALWAYS }
		/*BackFace.StencilFailOp*/				,{ D3D12_STENCIL_OP_REPLACE
		/*BackFace.StencilDepthFailOp*/			,D3D12_STENCIL_OP_REPLACE
		/*BackFace.StencilPassOp*/				,D3D12_STENCIL_OP_REPLACE
		/*BackFace.StencilFunc*/				,D3D12_COMPARISON_FUNC_ALWAYS }
	},

	//EDSS_DepthEnable
	{
		/*DepthEnable*/							TRUE
		/*DepthWriteMask*/						,D3D12_DEPTH_WRITE_MASK_ALL
		/*DepthFunc*/							,D3D12_COMPARISON_FUNC_LESS
		/*StencilEnable*/						,FALSE
		/*StencilReadMask*/						,D3D12_DEFAULT_STENCIL_READ_MASK
		/*StencilWriteMask*/					,D3D12_DEFAULT_STENCIL_WRITE_MASK
		/*FrontFace.StencilFailOp*/				,{ D3D12_STENCIL_OP_REPLACE
		/*FrontFace.StencilDepthFailOp*/		,D3D12_STENCIL_OP_REPLACE
		/*FrontFace.StencilPassOp*/				,D3D12_STENCIL_OP_REPLACE
		/*FrontFace.StencilFunc*/				,D3D12_COMPARISON_FUNC_ALWAYS }
		/*BackFace.StencilFailOp*/				,{ D3D12_STENCIL_OP_REPLACE
		/*BackFace.StencilDepthFailOp*/			,D3D12_STENCIL_OP_REPLACE
		/*BackFace.StencilPassOp*/				,D3D12_STENCIL_OP_REPLACE
		/*BackFace.StencilFunc*/				,D3D12_COMPARISON_FUNC_ALWAYS }
	},

	//EDSS_DepthTest
	{
	/*DepthEnable*/							TRUE
	/*DepthWriteMask*/						,D3D12_DEPTH_WRITE_MASK_ZERO
	/*DepthFunc*/							,D3D12_COMPARISON_FUNC_LESS
	/*StencilEnable*/						,FALSE
	/*StencilReadMask*/						,D3D12_DEFAULT_STENCIL_READ_MASK
	/*StencilWriteMask*/					,D3D12_DEFAULT_STENCIL_WRITE_MASK
	/*FrontFace.StencilFailOp*/				,{ D3D12_STENCIL_OP_REPLACE
	/*FrontFace.StencilDepthFailOp*/		,D3D12_STENCIL_OP_REPLACE
	/*FrontFace.StencilPassOp*/				,D3D12_STENCIL_OP_REPLACE
	/*FrontFace.StencilFunc*/				,D3D12_COMPARISON_FUNC_ALWAYS }
	/*BackFace.StencilFailOp*/				,{ D3D12_STENCIL_OP_REPLACE
	/*BackFace.StencilDepthFailOp*/			,D3D12_STENCIL_OP_REPLACE
	/*BackFace.StencilPassOp*/				,D3D12_STENCIL_OP_REPLACE
	/*BackFace.StencilFunc*/				,D3D12_COMPARISON_FUNC_ALWAYS }
	}
};

UINT GDescriptorHandleIncrementSize[ D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES ];

D3D12_HEAP_PROPERTIES const GHeapPropertiesGPUOnly =
{
	/*Type*/					D3D12_HEAP_TYPE_CUSTOM
	/*CPUPageProperty*/			,D3D12_CPU_PAGE_PROPERTY_NOT_AVAILABLE
	/*MemoryPoolPreference*/	,D3D12_MEMORY_POOL_L1
	/*CreationNodeMask*/		,1
	/*VisibleNodeMask*/			,1
};

D3D12_HEAP_PROPERTIES const GHeapPropertiesDefault =
{
	/*Type*/					D3D12_HEAP_TYPE_DEFAULT
	/*CPUPageProperty*/			,D3D12_CPU_PAGE_PROPERTY_UNKNOWN
	/*MemoryPoolPreference*/	,D3D12_MEMORY_POOL_UNKNOWN
	/*CreationNodeMask*/		,1
	/*VisibleNodeMask*/			,1
};
D3D12_HEAP_PROPERTIES const GHeapPropertiesUpload =
{
	/*Type*/					D3D12_HEAP_TYPE_UPLOAD
	/*CPUPageProperty*/			,D3D12_CPU_PAGE_PROPERTY_UNKNOWN
	/*MemoryPoolPreference*/	,D3D12_MEMORY_POOL_UNKNOWN
	/*CreationNodeMask*/		,1
	/*VisibleNodeMask*/			,1
};

Byte const GDXGIFormatsBitsSize[] =
{
	0,					//DXGI_FORMAT_UNKNOWN	                    
	16 * 8,				//DXGI_FORMAT_R32G32B32A32_TYPELESS       
	16 * 8,				//DXGI_FORMAT_R32G32B32A32_FLOAT          
	16 * 8,				//DXGI_FORMAT_R32G32B32A32_UINT           
	16 * 8,				//DXGI_FORMAT_R32G32B32A32_SINT           
	12 * 8,				//DXGI_FORMAT_R32G32B32_TYPELESS          
	12 * 8,				//DXGI_FORMAT_R32G32B32_FLOAT             
	12 * 8,				//DXGI_FORMAT_R32G32B32_UINT              
	12 * 8,				//DXGI_FORMAT_R32G32B32_SINT              
	8 * 8,				//DXGI_FORMAT_R16G16B16A16_TYPELESS       
	8 * 8,				//DXGI_FORMAT_R16G16B16A16_FLOAT          
	8 * 8,				//DXGI_FORMAT_R16G16B16A16_UNORM          
	8 * 8,				//DXGI_FORMAT_R16G16B16A16_UINT           
	8 * 8,				//DXGI_FORMAT_R16G16B16A16_SNORM          
	8 * 8,				//DXGI_FORMAT_R16G16B16A16_SINT           
	8 * 8,				//DXGI_FORMAT_R32G32_TYPELESS             
	8 * 8,				//DXGI_FORMAT_R32G32_FLOAT                
	8 * 8,				//DXGI_FORMAT_R32G32_UINT                 
	8 * 8,				//DXGI_FORMAT_R32G32_SINT                 
	8 * 8,				//DXGI_FORMAT_R32G8X24_TYPELESS           
	8 * 8,				//DXGI_FORMAT_D32_FLOAT_S8X24_UINT        
	8 * 8,				//DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS    
	8 * 8,				//DXGI_FORMAT_X32_TYPELESS_G8X24_UINT     
	4 * 8,				//DXGI_FORMAT_R10G10B10A2_TYPELESS        
	4 * 8,				//DXGI_FORMAT_R10G10B10A2_UNORM           
	4 * 8,				//DXGI_FORMAT_R10G10B10A2_UINT            
	4 * 8,				//DXGI_FORMAT_R11G11B10_FLOAT             
	4 * 8,				//DXGI_FORMAT_R8G8B8A8_TYPELESS           
	4 * 8,				//DXGI_FORMAT_R8G8B8A8_UNORM              
	4 * 8,				//DXGI_FORMAT_R8G8B8A8_UNORM_SRGB         
	4 * 8,				//DXGI_FORMAT_R8G8B8A8_UINT               
	4 * 8,				//DXGI_FORMAT_R8G8B8A8_SNORM              
	4 * 8,				//DXGI_FORMAT_R8G8B8A8_SINT               
	4 * 8,				//DXGI_FORMAT_R16G16_TYPELESS             
	4 * 8,				//DXGI_FORMAT_R16G16_FLOAT                
	4 * 8,				//DXGI_FORMAT_R16G16_UNORM                
	4 * 8,				//DXGI_FORMAT_R16G16_UINT                 
	4 * 8,				//DXGI_FORMAT_R16G16_SNORM                
	4 * 8,				//DXGI_FORMAT_R16G16_SINT                 
	4 * 8,				//DXGI_FORMAT_R32_TYPELESS                
	4 * 8,				//DXGI_FORMAT_D32_FLOAT                   
	4 * 8,				//DXGI_FORMAT_R32_FLOAT                   
	4 * 8,				//DXGI_FORMAT_R32_UINT                    
	4 * 8,				//DXGI_FORMAT_R32_SINT                    
	4 * 8,				//DXGI_FORMAT_R24G8_TYPELESS              
	4 * 8,				//DXGI_FORMAT_D24_UNORM_S8_UINT           
	4 * 8,				//DXGI_FORMAT_R24_UNORM_X8_TYPELESS       
	4 * 8,				//DXGI_FORMAT_X24_TYPELESS_G8_UINT        
	2 * 8,				//DXGI_FORMAT_R8G8_TYPELESS               
	2 * 8,				//DXGI_FORMAT_R8G8_UNORM                  
	2 * 8,				//DXGI_FORMAT_R8G8_UINT                   
	2 * 8,				//DXGI_FORMAT_R8G8_SNORM                  
	2 * 8,				//DXGI_FORMAT_R8G8_SINT                   
	2 * 8,				//DXGI_FORMAT_R16_TYPELESS                
	2 * 8,				//DXGI_FORMAT_R16_FLOAT                   
	2 * 8,				//DXGI_FORMAT_D16_UNORM                   
	2 * 8,				//DXGI_FORMAT_R16_UNORM                   
	2 * 8,				//DXGI_FORMAT_R16_UINT                    
	2 * 8,				//DXGI_FORMAT_R16_SNORM                   
	2 * 8,				//DXGI_FORMAT_R16_SINT                    
	1 * 8,				//DXGI_FORMAT_R8_TYPELESS                 
	1 * 8,				//DXGI_FORMAT_R8_UNORM                    
	1 * 8,				//DXGI_FORMAT_R8_UINT                     
	1 * 8,				//DXGI_FORMAT_R8_SNORM                    
	1 * 8,				//DXGI_FORMAT_R8_SINT                     
	1 * 8,				//DXGI_FORMAT_A8_UNORM                    
	0 * 8 + 1,			//DXGI_FORMAT_R1_UNORM                    
	4 * 8,				//DXGI_FORMAT_R9G9B9E5_SHAREDEXP          
	4 * 8,				//DXGI_FORMAT_R8G8_B8G8_UNORM             
	4 * 8,				//DXGI_FORMAT_G8R8_G8B8_UNORM             
	2 * 8 + 1,			//DXGI_FORMAT_BC1_TYPELESS                
	2 * 8 + 1,			//DXGI_FORMAT_BC1_UNORM                   
	2 * 8 + 1,			//DXGI_FORMAT_BC1_UNORM_SRGB              
	2 * 8 + 4,			//DXGI_FORMAT_BC2_TYPELESS                
	2 * 8 + 4,			//DXGI_FORMAT_BC2_UNORM                   
	2 * 8 + 4,			//DXGI_FORMAT_BC2_UNORM_SRGB              
	3 * 8,				//DXGI_FORMAT_BC3_TYPELESS                
	3 * 8,				//DXGI_FORMAT_BC3_UNORM                   
	3 * 8,				//DXGI_FORMAT_BC3_UNORM_SRGB              
	1 * 8,				//DXGI_FORMAT_BC4_TYPELESS                
	1 * 8,				//DXGI_FORMAT_BC4_UNORM                   
	1 * 8,				//DXGI_FORMAT_BC4_SNORM                   
	2 * 8,				//DXGI_FORMAT_BC5_TYPELESS                
	2 * 8,				//DXGI_FORMAT_BC5_UNORM                   
	2 * 8,				//DXGI_FORMAT_BC5_SNORM                   
	2 * 8,				//DXGI_FORMAT_B5G6R5_UNORM                
	2 * 8,				//DXGI_FORMAT_B5G5R5A1_UNORM              
	4 * 8,				//DXGI_FORMAT_B8G8R8A8_UNORM              
	4 * 8,				//DXGI_FORMAT_B8G8R8X8_UNORM              
	4 * 8,				//DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM  
	4 * 8,				//DXGI_FORMAT_B8G8R8A8_TYPELESS           
	4 * 8,				//DXGI_FORMAT_B8G8R8A8_UNORM_SRGB         
	4 * 8,				//DXGI_FORMAT_B8G8R8X8_TYPELESS           
	4 * 8,				//DXGI_FORMAT_B8G8R8X8_UNORM_SRGB         
	6 * 8,				//DXGI_FORMAT_BC6H_TYPELESS               
	6 * 8,				//DXGI_FORMAT_BC6H_UF16                   
	6 * 8,				//DXGI_FORMAT_BC6H_SF16                   
	7 * 3 + 8,			//DXGI_FORMAT_BC7_TYPELESS                
	7 * 3 + 8,			//DXGI_FORMAT_BC7_UNORM                   
	7 * 3 + 8,			//DXGI_FORMAT_BC7_UNORM_SRGB              
	4 * 8,				//DXGI_FORMAT_AYUV                        
	4 * 8,				//DXGI_FORMAT_Y410                        
	8 * 8,				//DXGI_FORMAT_Y416                        
	1 * 8,				//DXGI_FORMAT_NV12                        
	2 * 8,				//DXGI_FORMAT_P010                        
	2 * 8,				//DXGI_FORMAT_P016                        
	3 * 8,				//DXGI_FORMAT_420_OPAQUE                  
	4 * 8,				//DXGI_FORMAT_YUY2                        
	8 * 8,				//DXGI_FORMAT_Y210                        
	8 * 8,				//DXGI_FORMAT_Y216                        
	1 * 8,				//DXGI_FORMAT_NV11                        
	1 * 8,				//DXGI_FORMAT_AI44                        
	1 * 8,				//DXGI_FORMAT_IA44                        
	1 * 8,				//DXGI_FORMAT_P8                          
	2 * 8,				//DXGI_FORMAT_A8P8                        
	2 * 8,				//DXGI_FORMAT_B4G4R4A4_UNORM              
						//
	1 * 8,				//DXGI_FORMAT_P208                        
	1 * 8,				//DXGI_FORMAT_V208                        
	1 * 8,				//DXGI_FORMAT_V408                        
						//
						//
	UINT8_MAX			//DXGI_FORMAT_FORCE_UINT                  
};