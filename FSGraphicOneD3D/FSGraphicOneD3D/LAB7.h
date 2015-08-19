#pragma once
#include "d3dApp.h"

typedef struct SIMPLE_VERTEX {
	SIMPLE_VERTEX() {}
	SIMPLE_VERTEX(XMFLOAT3 _pos, XMFLOAT4 _color) : pos(_pos), color(_color){}
	XMFLOAT3 pos;
	XMFLOAT4 color;
}*SIMPLE_VERTEX_ptr;

// Define the constant data used to communicate with shaders.
typedef struct SEND_TO_VRAM {
	XMFLOAT4 constantColor;
	XMFLOAT2 constantOffset;
	XMFLOAT2 padding;
}*SEND_TO_VRAM_ptr;


struct cbPerObject {
	XMMATRIX WVP;
};


class LAB7 : public D3DApp {
	public:
		LAB7(HINSTANCE hinst);
		~LAB7();

		bool Init();
		void OnResize();
		void UpdateScene(double _dt);
		void DrawScene();

	private:
		void BuildCameraBuffer();
		void BuildGeometryBuffers();
		void BuildShader();
		void BuildVertexLayout();
		void BuildRenderStates();

	private:

		ID3D11Buffer					*m_circleVertexBuffer;
		ID3D11Buffer					*m_cubeIndexBuffer;
		ID3D11Buffer					*m_constantBuffer;
		ID3D11InputLayout				*m_inputLayout;
		ID3D11VertexShader				*m_vertexShader;
		ID3D11PixelShader				*m_pixelShader;

		SEND_TO_VRAM					m_vertConstData;
		vector<SIMPLE_VERTEX>			m_vertices;

		
		cbPerObject	cbPerObj;
		ID3D11Buffer *cbPerObjectBuffer = nullptr;

		XMMATRIX WVP;
		XMMATRIX World;
		XMMATRIX camView;
		XMMATRIX camProjection;

		XMVECTOR camPosition;
		XMVECTOR camTarget;
		XMVECTOR camUp;

		XMMATRIX cubeWorldMat;
		float rot = 0.01f;

		// Render States
		ID3D11RasterizerState			*m_wireFrame = nullptr;

};