
cbuffer ConstPerObject {
	float4x4 WVP;
	float4x4 World;
	int texIndex;
};



// textures!
Texture2D ObjTexture;
SamplerState ObjSamplerState;


struct VS_OUTPUT {
	float4 Position : SV_POSITION;
	float4 Color : COLOR;
	float2 TexCoord : TEXCOORD;
	float3 Normal : NORMAL;
};


VS_OUTPUT main(float4 inPos : POSITION,
			   float4 inColor : COLOR,
			   float2 inTexCoord : TEXCOORD,
			   float3 inNormal : NORMAL) {
	VS_OUTPUT output;

	output.Position = mul(inPos, WVP);

	output.Normal = mul(inNormal, World);
	
	output.Color = inColor;

	output.TexCoord = inTexCoord;

	output.TexCoord.x += 0.25 * texIndex;

	return output;
}

