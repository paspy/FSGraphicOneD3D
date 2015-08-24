
SamplerState ObjSamplerState;
TextureCube SkyMap;

struct SKYMAP_VS_OUTPUT {
	float4 Position : SV_POSITION;
	float3 texCoord : TEXCOORD;
};

float4 main(SKYMAP_VS_OUTPUT input) : SV_Target {
	return SkyMap.Sample(ObjSamplerState, input.texCoord);
}

//float4 main() : SV_TARGET
//{
//	return float4(1.0f, 1.0f, 1.0f, 1.0f);
//}