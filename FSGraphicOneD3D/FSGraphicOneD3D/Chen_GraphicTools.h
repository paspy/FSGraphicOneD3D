#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include <DirectXColors.h>
#include <DirectXPackedVector.h>
#include <DirectXCollision.h>
#include <D3Dcompiler.h>
#include <vector>

using namespace DirectX;
using namespace std;


namespace DXColors {
	XMGLOBALCONST XMVECTORF32 White = { 1.0f, 1.0f, 1.0f, 1.0f };
	XMGLOBALCONST XMVECTORF32 Black = { 0.0f, 0.0f, 0.0f, 1.0f };
	XMGLOBALCONST XMVECTORF32 Red = { 1.0f, 0.0f, 0.0f, 1.0f };
	XMGLOBALCONST XMVECTORF32 Green = { 0.0f, 1.0f, 0.0f, 1.0f };
	XMGLOBALCONST XMVECTORF32 Blue = { 0.0f, 0.0f, 1.0f, 1.0f };
	XMGLOBALCONST XMVECTORF32 Yellow = { 1.0f, 1.0f, 0.0f, 1.0f };
	XMGLOBALCONST XMVECTORF32 Cyan = { 0.0f, 1.0f, 1.0f, 1.0f };
	XMGLOBALCONST XMVECTORF32 Magenta = { 1.0f, 0.0f, 1.0f, 1.0f };

	XMGLOBALCONST XMVECTORF32 Silver = { 0.75f, 0.75f, 0.75f, 1.0f };
	XMGLOBALCONST XMVECTORF32 LightSteelBlue = { 0.69f, 0.77f, 0.87f, 1.0f };
}


typedef struct Vertex4 {
	XMFLOAT3 pos;
	XMVECTORF32 rgba;
}*Vertex4_ptr;

typedef struct Vertex4T {
	XMFLOAT3 pos;		// 0 byte offset
	XMFLOAT3 normal;	// 12 byte offset
	XMFLOAT2 tex0;		// 24 byte offset
	XMFLOAT2 tex1;		// 32 byte offset
	
}*Vertex4T_ptr;


typedef struct SIMPLE_VERTEX {
	XMFLOAT2 pos;
	XMVECTORF32 rgba;
}*SIMPLE_VERTEX_ptr;




D3D11_INPUT_ELEMENT_DESC vertTexureLayout[] = {
	{ "POSITION",	0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,	D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "NORMAL",		0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12,	D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD",	0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 24,	D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD",	1, DXGI_FORMAT_R32G32B32_FLOAT, 0, 32,	D3D11_INPUT_PER_VERTEX_DATA, 0 },
};

/*
VertexOut VS(float3 iPos : POSITION,
			 float3 iNormal : NORMAL,
			 float2 iTex0 : TEXCOORD0,
			 float2 iTex1 : TEXCOORD1)
*/

inline float Lerpf(float _A, float _B, float _ratio) {
	return (((_B - _A) * _ratio) + _A);
}

inline float DegreesToradians(float _degree) {
	return (_degree * XM_PI / 180.0f);
}

vector<SIMPLE_VERTEX> DrawParametricLine(const SIMPLE_VERTEX _pos0, const SIMPLE_VERTEX _pos1) {
	float deltaX = abs(_pos1.pos.x - _pos0.pos.x);
	float deltaY = abs(_pos1.pos.y - _pos0.pos.y);
	float m = max(deltaX, deltaY);
	vector<SIMPLE_VERTEX> verts;
	for (int i = 0; i < m; i++) {
		float ratio = i / m;
		float x = Lerpf(_pos0.pos.x, _pos1.pos.x, ratio);
		float y = Lerpf(_pos0.pos.y, _pos1.pos.y, ratio);
		SIMPLE_VERTEX pixel;
		pixel.pos.x = x;
		pixel.pos.y = y;
		pixel.rgba = _pos0.rgba;
		verts.push_back(pixel);
	}
	return verts;
}

vector<SIMPLE_VERTEX> DrawCircle(float _cx, float _cy, float _r, int _numSegments, XMVECTORF32 _color = DXColors::Yellow) {
	vector<SIMPLE_VERTEX> verts;
	for (int i = 0; i < _numSegments; i++) {
		float theta = 2.0f * XM_PI * float(i) / float(_numSegments);
		float x = _r * cosf(theta); 
		float y = _r * sinf(theta);
		SIMPLE_VERTEX pixel;
		pixel.pos.x = x + _cx;
		pixel.pos.y = y + _cy;
		pixel.rgba = _color;
		verts.push_back(pixel);
	}
	return verts;
}