#pragma once
#include "d3dApp.h"

typedef struct Vertex3D {
	Vertex3D() {}
	Vertex3D(XMFLOAT3 _pos, XMFLOAT2 _tex) : pos(_pos), texCoord(_tex) { XMStoreFloat4(&color,Colors::Black); }
	Vertex3D(XMFLOAT3 _pos, XMFLOAT4 _color) : pos(_pos), color(_color) {}
	XMFLOAT3 pos;
	XMFLOAT4 color;
	XMFLOAT2 texCoord;
}*Vertex3D_ptr;


struct ConstPerObject {
	XMMATRIX WVP;
	int texIndex;
};


class LAB10 : public D3DApp {
	public:
		LAB10(HINSTANCE hinst);
		~LAB10();

		bool Init();
		void OnResize();
		void UpdateKeyboardInput(double _dt);
		void UpdateScene(double _dt);
		void UpdateCamera();
		void DrawScene();

		void OnMouseDown(WPARAM _btnState, int _x, int _y);
		void OnMouseUp	(WPARAM _btnState, int _x, int _y);
		void OnMouseMove(WPARAM _btnState, int _x, int _y);

		void SetCamProj(XMMATRIX _c) { camProjection = _c; }

	private:
		void BuildCameraBuffer();
		void BuildGeometryBuffers();
		void BuildGridBuffers();
		void BuildTextureAndState();
		void BuildShader();
		void BuildVertexLayout();
		void BuildRenderStates();

	private:

		ID3D11Buffer					*m_cubeVertexBuffer;
		ID3D11Buffer					*m_cubeIndexBuffer;
		ID3D11Buffer					*m_gridVertexBuffer;
		ID3D11InputLayout				*m_inputLayout;
		ID3D11VertexShader				*m_vertexShader;
		ID3D11PixelShader				*m_pixelShader;

		ConstPerObject					m_cbPerObj;
		ID3D11Buffer					*m_constPerObjectBuffer = nullptr;

		// World, View, Projection Mat
		XMMATRIX						WVP;
		XMMATRIX						World;
		XMMATRIX						camView;
		XMMATRIX						camProjection;

		// Camera
		XMVECTOR						camPosition;
		XMVECTOR						camTarget;
		XMVECTOR						camUp;

		XMVECTOR						DefaultForward			= XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
		XMVECTOR						DefaultRight			= XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
		XMVECTOR						camForward				= XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
		XMVECTOR						camRight				= XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);

		XMMATRIX						camRotationMatrix;
		XMMATRIX						groundWorld;

		float							moveLeftRight			= 0.0f;
		float							moveBackForward			= -5.0f;

		float							camYaw					= 0.0f;
		float							camPitch				= 0.0f;

		// Object
		vector<Vertex3D>			m_gridVerts;
		XMMATRIX						cubeWorldMat;
		XMMATRIX						gridWorldMat;

		// Render States
		ID3D11RasterizerState			*m_wireFrame			= nullptr;

		// texture
		ID3D11ShaderResourceView		*m_cubeShaderResView	= nullptr;
		ID3D11Texture2D					*m_cubeTexture2D		= nullptr;
		ID3D11SamplerState				*m_cubesTexSamplerState = nullptr;

		// blending transparency
		ID3D11BlendState				*m_blendTransparency	= nullptr;
		ID3D11RasterizerState			*m_cwCullingMode		= nullptr;
		ID3D11RasterizerState			*m_ccwCullingMode		= nullptr;


};