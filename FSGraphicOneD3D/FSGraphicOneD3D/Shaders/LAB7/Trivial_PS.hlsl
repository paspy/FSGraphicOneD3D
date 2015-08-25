
struct BaseLight {
	float3 direction;
	float4 ambient;
	float4 diffuse;
};

cbuffer ConstPerObject {
	float4x4 WVP;
	float4x4 World;
	int texIndex;

	float4 difColor;
	bool hasTexture;
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

	////Set diffuse color of material
	//float4 diffuse = difColor;

	////If material has a diffuse texture map, set it now
	//if (hasTexture == true)
	//	diffuse = ObjTexture.Sample(ObjSamplerState, input.TexCoord);

	float4 rawDiffuse = ObjTexture.Sample(ObjSamplerState, input.TexCoord);
	float4 diffuse;
	diffuse.a = rawDiffuse.b;
	diffuse.r = rawDiffuse.g;
	diffuse.g = rawDiffuse.r;
	diffuse.b = rawDiffuse.a;

	float3 finalColor;

	finalColor = (diffuse * baseLight.ambient).rgb;
	finalColor += (saturate(dot(baseLight.direction, input.Normal) * baseLight.diffuse * diffuse)).rgb;

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