//float4 main(float4 colorFromRasterizer : COLOR) : SV_TARGET {
//	return colorFromRasterizer;
//}

struct VS_OUTPUT {
	float4 outPos : SV_POSITION;
	float4 outColor : COLOR;
};

float4 main(VS_OUTPUT input) : SV_TARGET{
	return input.outColor;
}