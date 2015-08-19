#pragma once

#include "d3dApp.h"
typedef struct SIMPLE_VERTEX {
	SIMPLE_VERTEX() {}
	SIMPLE_VERTEX(XMFLOAT3 _pos, XMVECTORF32 _color) : pos(_pos), rgba(_color) {}
	XMFLOAT3 pos;
	XMVECTORF32 rgba;
}*SIMPLE_VERTEX_ptr;

// Define the constant data used to communicate with shaders.
typedef struct SEND_TO_VRAM {
	XMFLOAT4 constantColor;
	XMFLOAT2 constantOffset;
	XMFLOAT2 padding;
}*SEND_TO_VRAM_ptr;

class LAB7 : public D3DApp {
	public:
		LAB7(HINSTANCE hinst);
		~LAB7();

		bool Init();
		void OnResize();
		void UpdateScene(double _dt);
		void DrawScene();

	private:
		void BuildGeometryBuffers();
		void BuildShader();
		void BuildVertexLayout();

	private:

		ID3D11Buffer					*m_circleVertexBuffer;
		ID3D11Buffer					*m_constantBuffer;
		ID3D11InputLayout				*m_inputLayout;
		ID3D11VertexShader				*m_vertexShader;
		ID3D11PixelShader				*m_pixelShader;

		SEND_TO_VRAM					m_vertConstData;
		vector<SIMPLE_VERTEX> m_vertices;

};