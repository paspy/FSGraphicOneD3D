#pragma once

#include <Windows.h>
#include <d3d11.h>
#include <DirectXMath.h>
#include <DirectXColors.h>
#include <DirectXPackedVector.h>
#include <vector>
#include "Chen_Utils.h"
#include "XTime.h"

using namespace DirectX;
using namespace std;

inline float DegreesToradians(float _degree) {
	return (_degree * XM_PI / 180.0f);
}

class D3DApp {
	public:
		D3DApp(HINSTANCE hinst/*, WNDPROC proc*/);
		virtual ~D3DApp();

		HINSTANCE	AppInstance() const		{ return application; }
		HWND		MainWnd() const			{ return window; }
		float		AspectRatio() const		{ return static_cast<float>(m_clientWidth) / m_clientHeight; }

		virtual bool Init();
		virtual void OnResize();
		virtual void UpdateScene(double _dt) = 0;
		virtual void DrawScene() = 0;
		virtual LRESULT MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

		virtual void OnMouseDown(WPARAM btnState, int x, int y) {}
		virtual void OnMouseUp(WPARAM btnState, int x, int y) {}
		virtual void OnMouseMove(WPARAM btnState, int x, int y) {}

		bool Run();

	protected:
		bool InitMainWindow();
		bool InitDirect3D();
		void ShowFPS();

	protected:
		// window related
		HINSTANCE						application;
		HWND							window;
		//WNDPROC						appWndProc;
		HRESULT							hr;
		bool							m_appPaused = false;
		bool							m_minimized = false;
		bool							m_maximized = false;
		bool							m_resizing = false;
		UINT							m_4xMsaaQuality = 0;

		//timer
		XTime							m_timer;
		bool							m_timerStop;
		
		// properties
		D3D_DRIVER_TYPE					m_d3dDriverType;
		wstring							m_mainWindTitle;
		int								m_clientWidth;
		int								m_clientHeight;
		bool							m_enable4xMsaa;

		// D3D stuff
		ID3D11Device					*m_d3dDevice;
		ID3D11DeviceContext				*m_d3dImmediateContext;
		IDXGISwapChain					*m_swapChain;
		ID3D11Texture2D					*m_depthStencilBuffer;
		ID3D11RenderTargetView			*m_renderTargetView;
		ID3D11DepthStencilView			*m_depthStencilView;
		D3D11_VIEWPORT					 m_screenViewport;	// non-pointer

};



typedef struct DVertex4 {
	XMFLOAT3 pos;
	XMVECTORF32 rgba;
}*DVertex4_ptr;

typedef struct DVertex4T {
	XMFLOAT3 pos;		// 0 byte offset
	XMFLOAT3 normal;	// 12 byte offset
	XMFLOAT2 tex0;		// 24 byte offset
	XMFLOAT2 tex1;		// 32 byte offset

}*DVertex4T_ptr;


//D3D11_INPUT_ELEMENT_DESC vertTexureLayout[] = {
//	{ "POSITION",	0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,	D3D11_INPUT_PER_VERTEX_DATA, 0 },
//	{ "NORMAL",		0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12,	D3D11_INPUT_PER_VERTEX_DATA, 0 },
//	{ "TEXCOORD",	0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 24,	D3D11_INPUT_PER_VERTEX_DATA, 0 },
//	{ "TEXCOORD",	1, DXGI_FORMAT_R32G32B32_FLOAT, 0, 32,	D3D11_INPUT_PER_VERTEX_DATA, 0 },
//};

/*
VertexOut VS(float3 iPos : POSITION,
float3 iNormal : NORMAL,
float2 iTex0 : TEXCOORD0,
float2 iTex1 : TEXCOORD1)
*/