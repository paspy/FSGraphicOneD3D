//float4 main(float4 colorFromRasterizer : COLOR) : SV_TARGET {
//	return colorFromRasterizer;
//}

Texture2D ObjTexture;
SamplerState ObjSamplerState;

struct VS_OUTPUT {
	float4 outPos : SV_POSITION;
	float4 TexCoord : TEXCOORD;
	float4 outNormal : NORMAL;
};

float4 main(VS_OUTPUT input) : SV_TARGET{
	//return input.outColor;

	float4 diffuse = ObjTexture.Sample(ObjSamplerState, input.TexCoord);
	float4 colorSwaped;
	colorSwaped.a = diffuse.b;
	colorSwaped.r = diffuse.g;
	colorSwaped.g = diffuse.r;
	colorSwaped.b = diffuse.a;

	//clip(diffuse.a - 0.50);

	return colorSwaped;
	//return float4(1,0,0,1);
}