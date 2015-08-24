
struct BaseLight {
	float3 direction;
	float4 ambient;
	float4 diffuse;
};

cbuffer ConstPerFrame {
	BaseLight baseLight;
};

Texture2D ObjTexture;
SamplerState ObjSamplerState;

struct VS_OUTPUT {
	float4 Position : SV_POSITION;
	float4 Color : COLOR;
	float2 TexCoord : TEXCOORD;
	float3 Normal : NORMAL;
};

float4 main(VS_OUTPUT input) : SV_TARGET {
	input.Normal = normalize(input.Normal);

	float4 rawDiffuse = ObjTexture.Sample(ObjSamplerState, input.TexCoord);
	float4 diffuse;
	diffuse.a = rawDiffuse.b;
	diffuse.r = rawDiffuse.g;
	diffuse.g = rawDiffuse.r;
	diffuse.b = rawDiffuse.a;

	float3 finalColor;

	finalColor = diffuse * baseLight.ambient;
	finalColor += saturate(dot(baseLight.direction, input.Normal) * baseLight.diffuse * diffuse);

	if (input.Color.x == 0 &&
		input.Color.y == 0 &&
		input.Color.z == 0 &&
		input.Color.w == 1 ) {
		//return diffuse;
		return float4(finalColor, diffuse.a);
	} else {
		return input.Color;
	}

}