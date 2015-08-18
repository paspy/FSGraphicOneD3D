#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include <DirectXColors.h>
#include <DirectXPackedVector.h>
#include <DirectXCollision.h>

using namespace DirectX;



typedef struct Vertex4 {
	XMFLOAT4 xyzw;
	XMFLOAT4 rgba;

}*Vertex4_ptr;


// create input layout describing our geometry
D3D11_INPUT_ELEMENT_DESC vLayout[] = {
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
};
