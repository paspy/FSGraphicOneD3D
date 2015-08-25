
struct BaseLight {
	float3 direction;
	float3 position;
	float  range;
	float3 spotLightDir;
	float cone;
	float3 attenuation;
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
	float4 WorldPos : POSITION;
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

	float3 finalColor = float3(0.0f, 0.0f, 0.0f);

	//Create the vector between light position and pixels position
	float3 lightToPixelVec = (float4(baseLight.position, 1.0) - input.WorldPos).xyz;

	//Find the distance between the light pos and pixel pos
	float d = length(lightToPixelVec);

	//Create the ambient light
	float3 finalAmbient = (diffuse * baseLight.ambient).xyz;

	//If pixel is too far, return pixel color with ambient light
	if (d > baseLight.range)
		return float4(finalAmbient, diffuse.a);

	//Turn lightToPixelVec into a unit length vector describing
	//the pixels direction from the lights position
	lightToPixelVec /= d;

	//Calculate how much light the pixel gets by the angle
	//in which the light strikes the pixels surface
	float howMuchLight = dot(lightToPixelVec, input.Normal);

	//If light is striking the front side of the pixel
	if (howMuchLight > 0.0f) {
		//Add light to the finalColor of the pixel

		finalColor += (howMuchLight * diffuse * baseLight.diffuse).rgb;

		//Calculate Light's Falloff factor
		finalColor /= baseLight.attenuation[0] + (baseLight.attenuation[1] * d) + (baseLight.attenuation[2] * (d*d));

		//Calculate falloff from center to edge of pointlight cone
		finalColor *= pow(max(dot(-lightToPixelVec, baseLight.spotLightDir), 0.0f), baseLight.cone);
	}

	//make sure the values are between 1 and 0, and add the ambient
	finalColor = saturate(finalColor + finalAmbient);

	//finalColor = (diffuse * baseLight.ambient).rgb;
	//finalColor += (saturate(dot(baseLight.direction, input.Normal) * baseLight.diffuse * diffuse)).rgb;

	if (input.Color.x == 0 &&
		input.Color.y == 0 &&
		input.Color.z == 0 &&
		input.Color.w == 1 ) {

		//Return Final Color
		return float4(finalColor, diffuse.a);
	} else {
		return input.Color;
	}

}