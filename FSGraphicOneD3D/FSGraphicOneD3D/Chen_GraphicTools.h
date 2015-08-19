#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include <DirectXColors.h>
#include <DirectXPackedVector.h>
#include <vector>

using namespace DirectX;
using namespace std;

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
	SIMPLE_VERTEX() {}
	SIMPLE_VERTEX(XMFLOAT3 _pos, XMVECTORF32 _color) : pos(_pos), rgba(_color) { }
	XMFLOAT3 pos;
	XMVECTORF32 rgba;
}*SIMPLE_VERTEX_ptr;


// Constant Buffer Sturcture
ID3D11Buffer*   g_pConstantBuffer = NULL;

// Define the constant data used to communicate with shaders.
typedef struct SEND_TO_VRAM {
	XMFLOAT4 constantColor;
	XMFLOAT2 constantOffset;
	XMFLOAT2 padding;
}*SEND_TO_VRAM_ptr;


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

vector<SIMPLE_VERTEX> DrawParametricLine(const SIMPLE_VERTEX &_pos0, const SIMPLE_VERTEX &_pos1) {
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
		pixel.pos.z = 0.0f;
		pixel.rgba = _pos0.rgba;
		verts.push_back(pixel);
	}
	return verts;
}

vector<SIMPLE_VERTEX> DrawCircle(float _cx, float _cy, float _r, int _numSegments, XMVECTORF32 _color = Colors::Yellow) {
	vector<SIMPLE_VERTEX> verts;

	for ( int i = 0; i < _numSegments; i++ ) {
		SIMPLE_VERTEX vert;
		vert.pos.x = _cx + (_r * cosf(i *  XM_2PI / _numSegments));
		vert.pos.y = _cy + (_r * sinf(i *  XM_2PI / _numSegments));
		vert.rgba = _color;
		verts.push_back(vert);
	}

	return verts;
}

vector<SIMPLE_VERTEX> DrawCircle2(XMFLOAT2 _cPoint, float _radius, int _numSegments, XMVECTORF32 _color = Colors::Yellow) {
	vector<SIMPLE_VERTEX> verts;
	float wedgeAngle = XM_2PI / _numSegments;
	for ( int i = 0; i < _numSegments; i++ ) {
		SIMPLE_VERTEX vert;
		float theta = i * wedgeAngle;
		vert.pos.x = _cPoint.x + _radius * cosf(theta);
		vert.pos.y = _cPoint.y - _radius * sinf(theta);
		vert.rgba = _color;
		verts.push_back(vert);
	}
	return verts;
}