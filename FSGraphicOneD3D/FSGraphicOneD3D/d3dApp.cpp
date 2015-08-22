#include "d3dApp.h"
#include "LAB9.h"

#include <Windowsx.h>
#include <sstream>

namespace {
	D3DApp *g_d3dApp = 0;
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
	//return DefWindowProc(hWnd, message, wParam, lParam);
	return g_d3dApp->MsgProc(hWnd, message, wParam, lParam);
}

D3DApp::D3DApp(HINSTANCE hinst/*, WNDPROC proc*/) :
	application(hinst),
	//appWndProc(proc),
	m_mainWindTitle(L"Graphic I - Chen Lu"),
	m_d3dDriverType(D3D_DRIVER_TYPE_HARDWARE),
	m_clientWidth(BACKBUFFER_WIDTH),
	m_clientHeight(BACKBUFFER_HEIGHT),
	m_enable4xMsaa(false),
	m_appPaused(false),
	m_minimized(false),
	m_maximized(false),
	m_resizing(false),
	m_timerStop(false),
	m_4xMsaaQuality(0),

	m_d3dDevice(nullptr),
	m_d3dImmediateContext(nullptr),
	m_swapChain(nullptr),

	m_depthStencilBuffer(nullptr),
	m_renderTargetView(nullptr),
	m_depthStencilView(nullptr),

	m_mouseAplha(XM_PI),
	m_mouseBeta(XM_PI)
{
	ZeroMemory(&m_screenViewport, sizeof(D3D11_VIEWPORT));
	m_lastMousePos.x = 0;
	m_lastMousePos.y = 0;
	g_d3dApp = this;
}

D3DApp::~D3DApp() {
	ReleaseCOM(m_renderTargetView);
	ReleaseCOM(m_swapChain);
	ReleaseCOM(m_depthStencilView);
	ReleaseCOM(m_depthStencilBuffer);

	if (m_d3dImmediateContext)
		m_d3dImmediateContext->ClearState();

	ReleaseCOM(m_d3dImmediateContext);
	ReleaseCOM(m_d3dDevice);

	UnregisterClass(L"DirectXApplication", application);
}

bool D3DApp::Init() {
	if (!InitMainWindow()) return false;
	if (!InitDirect3D()) return false;
	return true;
}

bool D3DApp::InitMainWindow() {
	// ****************** BEGIN WARNING ***********************// 
	// WINDOWS CODE, I DON'T TEACH THIS YOU MUST KNOW IT ALREADY! 
	//application = hinst;
	//appWndProc = proc;
	WNDCLASSEX  wndClass;
	ZeroMemory(&wndClass, sizeof(wndClass));
	wndClass.cbSize = sizeof(WNDCLASSEX);
	wndClass.lpfnWndProc = WndProc;	// changed
	wndClass.lpszClassName = L"DirectXApplication";
	wndClass.hInstance = application;
	wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndClass.hbrBackground = (HBRUSH)(COLOR_WINDOWFRAME);
	wndClass.hIcon = LoadIcon(0, IDI_APPLICATION);
	//wndClass.hIcon			= LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_FSICON));

	if (!RegisterClassEx(&wndClass)) {
		MessageBox(0, L"RegisterClass Failed.", 0, 0);
		return false;
	}
	//RegisterClassEx(&wndClass);

	RECT window_size = { 0, 0, m_clientWidth, m_clientHeight };

	AdjustWindowRect(&window_size, WS_OVERLAPPEDWINDOW, false);

	window = CreateWindow(L"DirectXApplication", m_mainWindTitle.c_str(),
		WS_OVERLAPPEDWINDOW & ~(WS_THICKFRAME | WS_MAXIMIZEBOX),
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		window_size.right - window_size.left,
		window_size.bottom - window_size.top,
		NULL, NULL, application, 0);	// this

	ShowWindow(window, SW_SHOW);
	UpdateWindow(window);	// added
//********************* END WARNING ************************//
	return true;
}

bool D3DApp::InitDirect3D() {

	// Begin to create device and swap chains
	UINT createDeviceFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)  
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	D3D_FEATURE_LEVEL  FeatureLevelsRequested = D3D_FEATURE_LEVEL_11_0;
	UINT               numLevelsRequested = 1;
	D3D_FEATURE_LEVEL  FeatureLevelsSupported;

	// create a struct to hold information about the swap chain
	DXGI_SWAP_CHAIN_DESC swapChainDesc;

	// clear out the struct for use
	ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));

	// fill the swap chain description struct
	swapChainDesc.BufferDesc.Width						= m_clientWidth;
	swapChainDesc.BufferDesc.Height						= m_clientHeight;
	swapChainDesc.BufferDesc.RefreshRate.Numerator		= 60;
	swapChainDesc.BufferDesc.RefreshRate.Denominator	= 1;
	swapChainDesc.BufferDesc.Format						= DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferDesc.ScanlineOrdering			= DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChainDesc.BufferDesc.Scaling					= DXGI_MODE_SCALING_UNSPECIFIED;
	if (m_enable4xMsaa) {
		swapChainDesc.SampleDesc.Count					= 4;
		swapChainDesc.SampleDesc.Quality				= m_4xMsaaQuality - 1;
	} else {
		swapChainDesc.SampleDesc.Count					= 1;
		swapChainDesc.SampleDesc.Quality				= 0;
	}
	swapChainDesc.BufferUsage							= DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferCount							= 1;
	swapChainDesc.OutputWindow							= window;
	swapChainDesc.Windowed								= true;
	swapChainDesc.SwapEffect							= DXGI_SWAP_EFFECT_DISCARD;
	swapChainDesc.Flags									= DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	IDXGIFactory *factoryPtr = nullptr;
	IDXGIAdapter *adapterPtr = nullptr;
	std::vector<IDXGIAdapter *> adapters;

	CreateDXGIFactory(__uuidof(IDXGIFactory), (void **)&factoryPtr);

	UINT bestAdapterIndex = 0;
	size_t bestMemSize = 0;

	for (UINT i = 0;
	factoryPtr->EnumAdapters(i, &adapterPtr) != DXGI_ERROR_NOT_FOUND;
		++i) {
		adapters.push_back(adapterPtr);
		DXGI_ADAPTER_DESC desc;
		adapterPtr->GetDesc(&desc);

		if (desc.DedicatedVideoMemory > bestMemSize) {
			bestAdapterIndex = i;
			bestMemSize = desc.DedicatedVideoMemory;
		}
	}

	ReleaseCOM(factoryPtr);

	HRESULT hr = D3D11CreateDeviceAndSwapChain(
		0,							// default adapters
		D3D_DRIVER_TYPE_HARDWARE,	// Driver Type If you specify the adapter, you cannot specify the driver type
		NULL,						// Software
		createDeviceFlags,			// Flags
		&FeatureLevelsRequested,	// Feature Levels Requested Pointer
		1,							// Number of Feature Levels
		D3D11_SDK_VERSION,			// D3D11_SDK_VERSION
		&swapChainDesc,				// Swap Chain Desciptions
		&m_swapChain,				// Swap Chain Pointer
		&m_d3dDevice,				// D3D Device
		&FeatureLevelsSupported,	// Return supported levels
		&m_d3dImmediateContext		// Device Context Pointer
	);	

	if ( FAILED(hr) ) {
		MessageBox(0, L"D3D11CreateDevice Failed.", 0, 0);
		return false;
	}

	if (FeatureLevelsSupported != D3D_FEATURE_LEVEL_11_0) {
		MessageBox(0, L"Direct3D Feature Level 11 unsupported.", 0, 0);
		return false;
	}

	HR(m_d3dDevice->CheckMultisampleQualityLevels(DXGI_FORMAT_R8G8B8A8_UNORM, 4, &m_4xMsaaQuality));
	assert(m_4xMsaaQuality > 0);

	OnResize();

	return true;
}

void D3DApp::OnResize() {
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

	// Backbuffer and Render Terget View Creation
	ID3D11Texture2D *backBuffer;
	HR(m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&backBuffer)));
	HR(m_d3dDevice->CreateRenderTargetView(backBuffer, 0, &m_renderTargetView));
	ReleaseCOM(backBuffer);

	// Create the depth/stencil buffer and view.
	D3D11_TEXTURE2D_DESC depthStencilDesc;

	depthStencilDesc.Width = m_clientWidth;
	depthStencilDesc.Height = m_clientHeight;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.ArraySize = 1;
	//depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;	//	A 32-bit z-buffer format that supports 24 bits for depth and 8 bits for stencil.
	depthStencilDesc.Format = DXGI_FORMAT_D32_FLOAT;	//	A single-component, 32-bit floating-point format that supports 32 bits for depth.

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
	LAB9 *abc = nullptr;
	abc = dynamic_cast<LAB9*> (g_d3dApp);
	if ( abc ) {
		abc->SetCamProj(XMMatrixPerspectiveFovLH(0.4f*3.14f, AspectRatio(), 1.0f, 1000.0f));
	}
}

void D3DApp::ShowFPS() {
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

bool D3DApp::Run() {
	if (!m_timerStop) m_timer.Signal();

	if (!m_appPaused) {
		ShowFPS();
		UpdateKeyboardInput(m_timer.Delta());
		UpdateCamera();
		UpdateScene(m_timer.Delta());
		DrawScene();
	} else {
		Sleep(100);
	}

	return true;
}

LRESULT D3DApp::MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	switch (msg) {
	case WM_ACTIVATE:

		return 0;
		// WM_SIZE is sent when the user resizes the window.  
	case WM_SIZE:
		// Save the new client area dimensions.
		m_clientWidth = LOWORD(lParam);
		m_clientHeight = HIWORD(lParam);
		if (m_d3dDevice) {
			if (wParam == SIZE_MINIMIZED) {
				m_appPaused = true;
				m_minimized = true;
				m_maximized = false;
			} else if (wParam == SIZE_MAXIMIZED) {
				m_appPaused = false;
				m_minimized = false;
				m_maximized = true;
				OnResize();
			} else if (wParam == SIZE_RESTORED) {
				// Restoring from minimized state?
				if (m_minimized) {
					m_appPaused = false;
					m_minimized = false;
					OnResize();
				}
				// Restoring from maximized state?
				else if (m_maximized) {
					m_appPaused = false;
					m_maximized = false;
					OnResize();
				} else if (m_resizing){
				} else {	
					// API call such as SetWindowPos or mSwapChain->SetFullscreenState.
					OnResize();
				}
			}
		}
		return 0;

		// WM_EXITSIZEMOVE is sent when the user grabs the resize bars.
	case WM_ENTERSIZEMOVE:
		m_appPaused = true;
		m_resizing = true;
		m_timerStop = true;
		return 0;

	case WM_EXITSIZEMOVE:
		m_appPaused = false;
		m_resizing = false;
		m_timerStop = false;
		OnResize();
		return 0;

		// WM_DESTROY is sent when the window is being destroyed.
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

		// The WM_MENUCHAR message is sent when a menu is active and the user presses 
		// a key that does not correspond to any mnemonic or accelerator key. 
	case WM_MENUCHAR:
		// Don't beep when we alt-enter.
		return MAKELRESULT(0, MNC_CLOSE);

		// Catch this message so to prevent the window from becoming too small.
	case WM_GETMINMAXINFO:
		((MINMAXINFO*)lParam)->ptMinTrackSize.x = 200;
		((MINMAXINFO*)lParam)->ptMinTrackSize.y = 200;
		return 0;

	case WM_LBUTTONDOWN:
	case WM_MBUTTONDOWN:
	case WM_RBUTTONDOWN:
		OnMouseDown(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		return 0;
	case WM_LBUTTONUP:
	case WM_MBUTTONUP:
	case WM_RBUTTONUP:
		OnMouseUp(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		return 0;
	case WM_MOUSEMOVE:
		OnMouseMove(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		return 0;
	}

	return DefWindowProc(hwnd, msg, wParam, lParam);
}

