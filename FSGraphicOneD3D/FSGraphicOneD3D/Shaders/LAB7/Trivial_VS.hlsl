
cbuffer ConstPerObject {
	float4x4 WVP;
	int texIndex;
};

// textures!
Texture2D ObjTexture;
SamplerState ObjSamplerState;


struct VS_OUTPUT {
	float4 outPos : SV_POSITION;
	float4 outColor : COLOR;
	float2 TexCoord : TEXCOORD;
};


VS_OUTPUT main(float4 inPos : POSITION, float4 inColor : COLOR ,float2 inTexCoord : TEXCOORD) {
	VS_OUTPUT output;

	output.outPos = mul(inPos, WVP);
	
	output.outColor = inColor;

	output.TexCoord = inTexCoord;

	output.TexCoord.x += 0.25 * texIndex;

	return output;
}

