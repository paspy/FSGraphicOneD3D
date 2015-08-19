//struct INPUT_VERTEX {
//	float3 coordinate : POSITION;
//};
//
//struct OUTPUT_VERTEX {
//	float4 colorOut : COLOR;
//	float4 projectedCoordinate : SV_POSITION;
//};
//
//cbuffer THIS_IS_VRAM : register(b0) {
//	float4 constantColor;
//	float2 constantOffset;
//	float2 padding;
//};
//
//OUTPUT_VERTEX main(INPUT_VERTEX fromVertexBuffer) {
//	OUTPUT_VERTEX sendToRasterizer = (OUTPUT_VERTEX)0;
//	sendToRasterizer.projectedCoordinate.w = 1;
//
//	sendToRasterizer.projectedCoordinate.xy = fromVertexBuffer.coordinate.xy;
//
//	sendToRasterizer.projectedCoordinate.xy += constantOffset;
//
//	sendToRasterizer.colorOut = constantColor;
//
//	return sendToRasterizer;
//}

cbuffer cbPerObject {
	float4x4 WVP;
};


struct VS_OUTPUT {
	float4 outPos : SV_POSITION;
	float4 outColor : COLOR;
};


VS_OUTPUT main(float4 inPos : POSITION, float4 inColor : COLOR) {
	VS_OUTPUT output;

	output.outPos = mul(inPos, WVP);
	output.outColor = inColor;

	return output;
}

