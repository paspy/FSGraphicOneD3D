#pragma once
#include "d3dApp.h"

//typedef struct SIMPLE_VERTEX {
//	SIMPLE_VERTEX() {}
//	SIMPLE_VERTEX(XMFLOAT3 _pos, XMFLOAT4 _color) : pos(_pos), color(_color) {}
//	XMFLOAT3 pos;
//	XMFLOAT4 color;
//}*SIMPLE_VERTEX_ptr;

typedef struct SIMPLE_VERTEX {
	SIMPLE_VERTEX() {}
	SIMPLE_VERTEX(XMFLOAT3 _pos, XMFLOAT2 _tex) : pos(_pos), texCoord(_tex) {}
	XMFLOAT3 pos;
	XMFLOAT2 texCoord;
}*SIMPLE_VERTEX_ptr;

// Define the constant data used to communicate with shaders.
typedef struct SEND_TO_VRAM {
	XMFLOAT4 constantColor;
	XMFLOAT2 constantOffset;
	XMFLOAT2 padding;
}*SEND_TO_VRAM_ptr;


struct cbPerObject {
	XMMATRIX WVP;
	int texIndex;
};


class LAB7 : public D3DApp {
	public:
		LAB7(HINSTANCE hinst);
		~LAB7();

		bool Init();
		void OnResize();
		void UpdateKeyboardInput(double _dt);
		void UpdateCamera();
		void UpdateScene(double _dt);
		void DrawScene();

		void OnMouseDown(WPARAM _btnState, int _x, int _y);
		void OnMouseUp	(WPARAM _btnState, int _x, int _y);
		void OnMouseMove(WPARAM _btnState, int _x, int _y);

	private:
		void BuildCameraBuffer();
		void BuildGeometryBuffers();
		void BuildTextureAndState();
		void BuildShader();
		void BuildVertexLayout();
		void BuildRenderStates();

	private:

		ID3D11Buffer					*m_circleVertexBuffer;
		ID3D11Buffer					*m_cubeIndexBuffer;
		ID3D11InputLayout				*m_inputLayout;
		ID3D11VertexShader				*m_vertexShader;
		ID3D11PixelShader				*m_pixelShader;

		// deprecated and used to apply constat color to obj; Old LAB 7 stuff
		ID3D11Buffer					*m_constantBuffer;		
		SEND_TO_VRAM					m_vertConstData;
		vector<SIMPLE_VERTEX>			m_vertices;


		cbPerObject						cbPerObj;
		ID3D11Buffer					*cbPerObjectBuffer = nullptr;

		// World, View, Projection Mat
		XMMATRIX						WVP;
		XMMATRIX						World;
		XMMATRIX						camView;
		XMMATRIX						camProjection;

		// Camera
		XMVECTOR						camPosition;
		XMVECTOR						camTarget;
		XMVECTOR						camUp;

		// Object
		XMMATRIX cubeWorldMat;

		// Render States
		ID3D11RasterizerState			*m_wireFrame = nullptr;

		// texture
		ID3D11ShaderResourceView		*m_cubesTexture = nullptr;
		ID3D11SamplerState				*m_cubesTexSamplerState = nullptr;


};