#pragma once

enum ERasterizerStates
{
	ERS_Default,
	ERS_MAX
};
extern D3D12_RASTERIZER_DESC const GRasterizerStates[ ERS_MAX ];

enum ERenderTargetBlendStates
{
	ERTBS_Disabled,
	ERTBS_AlphaBlend,
	ERTBS_RGBAdd,

	ERTBS_MAX
};
extern D3D12_RENDER_TARGET_BLEND_DESC const GRenderTargetBlendStates[ ERTBS_MAX ];

enum EDepthStencilStates
{
	EDSS_Disabled,
	EDSS_DepthEnable,
	EDSS_DepthTest,

	EDSS_MAX
};
extern D3D12_DEPTH_STENCIL_DESC const GDepthStencilStates[ EDSS_MAX ];

extern UINT GDescriptorHandleIncrementSize[];

extern D3D12_HEAP_PROPERTIES const GHeapPropertiesGPUOnly;
extern D3D12_HEAP_PROPERTIES const GHeapPropertiesDefault;
extern D3D12_HEAP_PROPERTIES const GHeapPropertiesUpload;


extern Byte const GDXGIFormatsBitsSize[];