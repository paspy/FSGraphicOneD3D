// D3D LAB 1a "Line Land".
// Author: L.Norri CD DRX, FullSail University

// Introduction:
// Welcome to the first lab of the Direct3D Graphics Programming class.
// This is the ONLY guided lab in this course! 
// Future labs will demand the habits & foundation you develop right now!  
// It is CRITICAL that you follow each and every step. ESPECIALLY THE READING!!!

// TO BEGIN: Open the word document that acompanies this lab and start from the top.

//************************************************************
//************ INCLUDES & DEFINES ****************************
//************************************************************

#include <iostream>
#include <sstream>
#include <ctime>
#include "XTime.h"
#include "Chen_Utils.h"
#include "Chen_GraphicTools.h"
#include "Chen_Geometries.h"

#include "Trivial_VS.csh"
#include "Trivial_PS.csh"

using namespace std;

// TODO: PART 2 STEP 6

#define BACKBUFFER_WIDTH	500
#define BACKBUFFER_HEIGHT	500

//************************************************************
//************ SIMPLE WINDOWS APP CLASS **********************
//************************************************************

class CHEN_D3D_APP {
	public:
		CHEN_D3D_APP(HINSTANCE hinst, WNDPROC proc);
		bool Run();
		bool ShutDown();
		void OnResize();
		void ShowFPS();
		void UpdateScene(double _dt);
		void DrawScene(); 

	private:
		HINSTANCE						application;
		WNDPROC							appWndProc;
		HWND							window;

		HRESULT							hr;
		bool							m_appPaused				= false;
		bool							m_minimized				= false;
		bool							m_maximized				= false;
		bool							m_resizing				= false;
		bool							m_enable4xMsaa			= false;
		UINT							m_4xMsaaQuality			= 0;

		XTime							m_timer;

		std::wstring					m_mainWindTitle			= L"Graphic I - Chen Lu";
		int								m_clientWidth			= BACKBUFFER_WIDTH;
		int								m_clientHeight			= BACKBUFFER_HEIGHT;

		ID3D11Device					*m_d3dDevice			= nullptr;
		ID3D11DeviceContext				*m_d3dImmediateContext	= nullptr;
		IDXGISwapChain					*m_swapChain			= nullptr;
		ID3D11Texture2D					*m_depthStencilBuffer	= nullptr;
		ID3D11RenderTargetView			*m_renderTargetView		= nullptr;
		ID3D11DepthStencilView			*m_depthStencilView		= nullptr;
		D3D_DRIVER_TYPE					m_d3dDriverType			= D3D_DRIVER_TYPE_HARDWARE;
		D3D11_VIEWPORT					m_screenViewport;

		ID3D11Buffer					*m_circleVB				= nullptr;
		ID3D11InputLayout				*m_inputLayout			= nullptr;
		ID3D11VertexShader				*m_vertexShader			= nullptr;
		ID3D11PixelShader				*m_pixelShader			= nullptr;

		XMFLOAT4X4						m_world;
		XMFLOAT4X4						m_view;
		XMFLOAT4X4						m_proj;
		void BuildGeometryBuffers();
		void BuildShader();
		void BuildVertexLayout();

		vector<SIMPLE_VERTEX> m_vertices;
};

//************************************************************
//************ CREATION OF OBJECTS & RESOURCES ***************
//************************************************************

CHEN_D3D_APP::CHEN_D3D_APP(HINSTANCE hinst, WNDPROC proc) {
	// ****************** BEGIN WARNING ***********************// 
	// WINDOWS CODE, I DON'T TEACH THIS YOU MUST KNOW IT ALREADY! 
	application = hinst;
	appWndProc = proc;

	WNDCLASSEX  wndClass;
	ZeroMemory(&wndClass, sizeof(wndClass));
	wndClass.cbSize = sizeof(WNDCLASSEX);
	wndClass.lpfnWndProc = appWndProc;
	wndClass.lpszClassName = L"DirectXApplication";
	wndClass.hInstance = application;
	wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndClass.hbrBackground = (HBRUSH)(COLOR_WINDOWFRAME);
	//wndClass.hIcon			= LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_FSICON));
	RegisterClassEx(&wndClass);

	RECT window_size = { 0, 0, BACKBUFFER_WIDTH, BACKBUFFER_HEIGHT };
	AdjustWindowRect(&window_size, WS_OVERLAPPEDWINDOW, false);

	window = CreateWindow(L"DirectXApplication", m_mainWindTitle.c_str(), WS_OVERLAPPEDWINDOW & ~(WS_THICKFRAME | WS_MAXIMIZEBOX),
		CW_USEDEFAULT, CW_USEDEFAULT, window_size.right - window_size.left, window_size.bottom - window_size.top,
		NULL, NULL, application, this);

	ShowWindow(window, SW_SHOW);
	//********************* END WARNING ************************//
	
	ZeroMemory(&m_screenViewport, sizeof(D3D11_VIEWPORT));

	XMMATRIX I = XMMatrixIdentity();
	XMStoreFloat4x4(&m_world, I);
	XMStoreFloat4x4(&m_view, I);
	XMStoreFloat4x4(&m_proj, I);

	UINT createDeviceFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)  
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
	


	D3D_FEATURE_LEVEL featureLevel;
	HRESULT hr = D3D11CreateDevice( NULL,                 // default adapter
									m_d3dDriverType,
									NULL,                 // no software device
									createDeviceFlags,
									NULL, 
									NULL,              // default feature level array
									D3D11_SDK_VERSION,
									&m_d3dDevice,
									&featureLevel,
									&m_d3dImmediateContext);
	if (FAILED(hr)) {
		MessageBox(0, L"D3D11CreateDevice Failed.", 0, 0);
	}

	if (featureLevel != D3D_FEATURE_LEVEL_11_0) {
		MessageBox(0, L"Direct3D Feature Level 11 unsupported.", 0, 0);
	}

	HR(m_d3dDevice->CheckMultisampleQualityLevels(DXGI_FORMAT_R8G8B8A8_UNORM, 4, &m_4xMsaaQuality));
	assert(m_4xMsaaQuality > 0);


	// create a struct to hold information about the swap chain
	DXGI_SWAP_CHAIN_DESC swapChainDesc;

	// clear out the struct for use
	ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));

	// fill the swap chain description struct
	swapChainDesc.BufferDesc.Width = m_clientWidth;
	swapChainDesc.BufferDesc.Height = m_clientHeight;
	swapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
	swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	if (m_enable4xMsaa) {
		swapChainDesc.SampleDesc.Count = 4;
		swapChainDesc.SampleDesc.Quality = m_4xMsaaQuality - 1;
	} else {
		swapChainDesc.SampleDesc.Count = 1;
		swapChainDesc.SampleDesc.Quality = 0;
	}
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferCount = 1;
	swapChainDesc.OutputWindow = window;
	swapChainDesc.Windowed = true;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	swapChainDesc.Flags = 0;

	// To correctly create the swap chain, we must use the IDXGIFactory that was
	// used to create the device.  If we tried to use a different IDXGIFactory instance
	// (by calling CreateDXGIFactory), we get an error: "IDXGIFactory::CreateSwapChain: 
	// This function is being called with a device from a different IDXGIFactory."
	IDXGIDevice* dxgiDevice = 0;
	HR(m_d3dDevice->QueryInterface(__uuidof(IDXGIDevice), (void**)&dxgiDevice));

	IDXGIAdapter* dxgiAdapter = 0;
	HR(dxgiDevice->GetParent(__uuidof(IDXGIAdapter), (void**)&dxgiAdapter));

	IDXGIFactory* dxgiFactory = 0;
	HR(dxgiAdapter->GetParent(__uuidof(IDXGIFactory), (void**)&dxgiFactory));

	HR(dxgiFactory->CreateSwapChain(m_d3dDevice, &swapChainDesc, &m_swapChain));

	ReleaseCOM(dxgiDevice);
	ReleaseCOM(dxgiAdapter);
	ReleaseCOM(dxgiFactory);

	OnResize();

	BuildGeometryBuffers();
	BuildShader();
	BuildVertexLayout();
}

//************************************************************
//************ EXECUTION *************************************
//************************************************************

bool CHEN_D3D_APP::Run() {
	m_timer.Signal();

	if (!m_appPaused) {
		ShowFPS();
		UpdateScene(m_timer.Delta());
		DrawScene();
	} else {
		Sleep(100);
	}

	return true;
}

void CHEN_D3D_APP::UpdateScene(double _dt) {

}

void CHEN_D3D_APP::DrawScene() {
	assert(m_d3dImmediateContext);
	assert(m_swapChain);

	m_d3dImmediateContext->ClearRenderTargetView(m_renderTargetView, reinterpret_cast<const float*>(&Colors::DarkBlue));
	m_d3dImmediateContext->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	//m_d3dImmediateContext->IASetInputLayout(m_inputLayout);
	m_d3dImmediateContext->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);

	UINT stride = sizeof(SIMPLE_VERTEX);
	UINT offset = 0;
	m_d3dImmediateContext->IASetVertexBuffers(0, 1, &m_circleVB, &stride, &offset);

	XMMATRIX world = XMLoadFloat4x4(&m_world);
	XMMATRIX view = XMLoadFloat4x4(&m_view);
	XMMATRIX proj = XMLoadFloat4x4(&m_proj);
	XMMATRIX worldViewProj = world*view*proj;


	HR(m_swapChain->Present(0, 0));
}

void CHEN_D3D_APP::BuildGeometryBuffers() {
	m_vertices = DrawCircle(0, 0, 100, 50);

	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = static_cast<UINT>(sizeof(SIMPLE_VERTEX) * m_vertices.size());
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;
	vbd.StructureByteStride = 0;
	D3D11_SUBRESOURCE_DATA vinitData;
	vinitData.pSysMem = &m_vertices;
	HR(m_d3dDevice->CreateBuffer(&vbd, &vinitData, &m_circleVB));
}


void CHEN_D3D_APP::BuildShader() {
	HR(m_d3dDevice->CreateVertexShader(Trivial_VS, sizeof(Trivial_VS), NULL, &m_vertexShader));
	HR(m_d3dDevice->CreatePixelShader(Trivial_PS, sizeof(Trivial_PS), NULL, &m_pixelShader));

	m_d3dImmediateContext->VSSetShader(m_vertexShader, NULL, 0);
	m_d3dImmediateContext->PSSetShader(m_pixelShader, NULL, 0);

}

void CHEN_D3D_APP::BuildVertexLayout() {
	// create input layout describing our geometry
	D3D11_INPUT_ELEMENT_DESC vertColorLayout[] = {
		{ "POSITION",	0, DXGI_FORMAT_R32G32B32_FLOAT,		0, 0,	D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR",		0, DXGI_FORMAT_R32G32B32A32_FLOAT,	0, 12,	D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	// Create the input layout
	//HR(m_d3dDevice->CreateInputLayout(vertColorLayout, 2, m_vertexBlob, m_vertexBlob->GetBufferSize(), &m_inputLayout));
}

void CHEN_D3D_APP::ShowFPS() {
	static int frameCnt = 0;
	static float timeElapsed = 0.0f;

	frameCnt++;

	// Compute averages over one second period.
	if ((m_timer.TotalTime() - timeElapsed) >= 1.0f) {
		float fps = (float)frameCnt; // fps = frameCnt / 1
		float mspf = 1000.0f / fps;

		std::wostringstream outs;
		outs.precision(6);
		outs << m_mainWindTitle << L" - FPS: " << fps << L", Time: " << mspf << L" (ms)";
		SetWindowText(window, outs.str().c_str());

		// Reset for next average.
		frameCnt = 0;
		timeElapsed += 1.0f;
	}
}

void CHEN_D3D_APP::OnResize() {
	assert(m_d3dImmediateContext);
	assert(m_d3dDevice);
	assert(m_swapChain);

	// Release the old views, as they hold references to the buffers we
	// will be destroying.  Also release the old depth/stencil buffer.
	ReleaseCOM(m_renderTargetView);
	ReleaseCOM(m_depthStencilView);
	ReleaseCOM(m_depthStencilBuffer);

	// Resize the swap chain and recreate the render target view.

	HR(m_swapChain->ResizeBuffers(1, m_clientWidth, m_clientHeight, DXGI_FORMAT_R8G8B8A8_UNORM, 0));
	ID3D11Texture2D* backBuffer;
	HR(m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&backBuffer)));
	HR(m_d3dDevice->CreateRenderTargetView(backBuffer, 0, &m_renderTargetView));
	ReleaseCOM(backBuffer);


	// Create the depth/stencil buffer and view.
	D3D11_TEXTURE2D_DESC depthStencilDesc;

	depthStencilDesc.Width = m_clientWidth;
	depthStencilDesc.Height = m_clientHeight;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.ArraySize = 1;
	depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;

	if (m_enable4xMsaa) {
		depthStencilDesc.SampleDesc.Count = 4;
		depthStencilDesc.SampleDesc.Quality = m_4xMsaaQuality - 1;
	} else {
		depthStencilDesc.SampleDesc.Count = 1;
		depthStencilDesc.SampleDesc.Quality = 0;
	}

	depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
	depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthStencilDesc.CPUAccessFlags = 0;
	depthStencilDesc.MiscFlags = 0;

	HR(m_d3dDevice->CreateTexture2D(&depthStencilDesc, 0, &m_depthStencilBuffer));
	HR(m_d3dDevice->CreateDepthStencilView(m_depthStencilBuffer, 0, &m_depthStencilView));

	// Bind the render target view and depth/stencil view to the pipeline.
	m_d3dImmediateContext->OMSetRenderTargets(1, &m_renderTargetView, m_depthStencilView);

	// Set the viewport transform.

	m_screenViewport.TopLeftX = 0;
	m_screenViewport.TopLeftY = 0;
	m_screenViewport.Width = static_cast<float>(m_clientWidth);
	m_screenViewport.Height = static_cast<float>(m_clientHeight);
	m_screenViewport.MinDepth = 0.0f;
	m_screenViewport.MaxDepth = 1.0f;

	m_d3dImmediateContext->RSSetViewports(1, &m_screenViewport);
}

//************************************************************
//************ DESTRUCTION ***********************************
//************************************************************

bool CHEN_D3D_APP::ShutDown() {
	// TODO: PART 1 STEP 6

	ReleaseCOM(m_renderTargetView);
	ReleaseCOM(m_depthStencilView);
	ReleaseCOM(m_swapChain);
	ReleaseCOM(m_depthStencilBuffer);

	// Restore all default settings.
	if (m_d3dImmediateContext)
		m_d3dImmediateContext->ClearState();

	ReleaseCOM(m_d3dImmediateContext);
	ReleaseCOM(m_d3dDevice);

	ReleaseCOM(m_circleVB);
	ReleaseCOM(m_vertexShader);
	ReleaseCOM(m_pixelShader);
	ReleaseCOM(m_inputLayout);

	UnregisterClass(L"DirectXApplication", application);
	return true;
}

//************************************************************
//************ WINDOWS RELATED *******************************
//************************************************************

// ****************** BEGIN WARNING ***********************// 
// WINDOWS CODE, I DON'T TEACH THIS YOU MUST KNOW IT ALREADY!

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow);
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wparam, LPARAM lparam);

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, LPTSTR, int) {
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif
	srand(unsigned int(time(0)));
	CHEN_D3D_APP myApp(hInstance, (WNDPROC)WndProc);
	MSG msg; ZeroMemory(&msg, sizeof(msg));
	while (msg.message != WM_QUIT && myApp.Run()) {
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	myApp.ShutDown();
	return 0;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	if (GetAsyncKeyState(VK_ESCAPE))
		message = WM_DESTROY;
	switch (message) {
		case (WM_DESTROY) : { 
			PostQuitMessage(0); 
	
			break;
		} 
	}
	return DefWindowProc(hWnd, message, wParam, lParam);
}
//********************* END WARNING ************************//