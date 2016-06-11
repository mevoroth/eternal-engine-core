#include "imgui.common.hlsl"

PSIn VS(VSIn IN)
{
	PSIn OUT = (PSIn)0;

	OUT.Pos	= mul(ProjectionMatrix, float4(IN.Pos.xy, 0.f, 1.f));
	OUT.Col	= IN.Col;
	OUT.UV	= IN.UV;

	return OUT;
}
