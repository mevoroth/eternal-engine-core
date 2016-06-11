#ifndef _IMGUI_COMMON_HLSL_
#define _IMGUI_COMMON_HLSL_

cbuffer ImguiBuffer : register(b0)
{
	float4x4 ProjectionMatrix;
};

struct VSIn
{
	float2 Pos : POSITION;
	float4 Col : COLOR0;
	float2 UV  : TEXCOORD0;
};

struct PSIn
{
	float4 Pos : SV_POSITION;
	float4 Col : COLOR0;
	float2 UV  : TEXCOORD0;
};


sampler ImguiSampler;
Texture2D ImguiTexture;

#endif
