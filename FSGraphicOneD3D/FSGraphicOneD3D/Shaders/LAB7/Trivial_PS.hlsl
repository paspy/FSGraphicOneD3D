//float4 main(float4 colorFromRasterizer : COLOR) : SV_TARGET {
//	return colorFromRasterizer;
//}

Texture2D ObjTexture;
SamplerState ObjSamplerState;

struct VS_OUTPUT {
	float4 outPos : SV_POSITION;
	float4 outColor : COLOR;
	float2 TexCoord : TEXCOORD;
};

float4 main(VS_OUTPUT input) : SV_TARGET{
	//return input.outColor;


	float4 rawDiffuse = ObjTexture.Sample(ObjSamplerState, input.TexCoord);
	float4 diffuse;
	diffuse.a = rawDiffuse.b;
	diffuse.r = rawDiffuse.g;
	diffuse.g = rawDiffuse.r;
	diffuse.b = rawDiffuse.a;

	if (input.outColor.x == 0 &&
		input.outColor.y == 0 &&
		input.outColor.z == 0 &&
		input.outColor.w == 1 ) {
		return diffuse;
	} else {
		return input.outColor;
	}

}