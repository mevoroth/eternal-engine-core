#include "imgui.common.hlsl"

float4 PS(PSIn IN) : SV_Target
{
	float4 TexCol = ImguiTexture.Sample(ImguiSampler, IN.UV);
	return IN.Col * TexCol * TexCol.a; // Temporary fix
}
