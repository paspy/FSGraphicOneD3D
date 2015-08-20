//float4 main(float4 colorFromRasterizer : COLOR) : SV_TARGET {
//	return colorFromRasterizer;
//}

Texture2D ObjTexture;
SamplerState ObjSamplerState;

struct VS_OUTPUT {
	float4 outPos : SV_POSITION;
	//float4 outColor : COLOR;
	float2 TexCoord : TEXCOORD;
};

float4 main(VS_OUTPUT input) : SV_TARGET{
	//return input.outColor;
	return ObjTexture.Sample(ObjSamplerState, input.TexCoord);
}