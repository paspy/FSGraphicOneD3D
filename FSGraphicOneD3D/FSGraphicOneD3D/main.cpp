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
#include <ctime>
#include "XTime.h"
#include "Chen_GraphicTools.h"
#include "Chen_Geometries.h"

using namespace std;

// TODO: PART 2 STEP 6

#define BACKBUFFER_WIDTH	500
#define BACKBUFFER_HEIGHT	500

//************************************************************
//************ SIMPLE WINDOWS APP CLASS **********************
//************************************************************

class CHEN_APP {
	HINSTANCE						application;
	WNDPROC							appWndProc;
	HWND							window;

	ID3D11Device					*clDevice			= nullptr;			// the pointer to our Direct3D device interface
	IDXGISwapChain					*clSwapChain		= nullptr;			// the pointer to the swap chain interface
	ID3D11DeviceContext				*clDeviceContext	= nullptr;          // the pointer to our Direct3D device context
	ID3D11RenderTargetView			*clRenderTargetView = nullptr;			// This allows D3D to connect to your swap chain¡¯s ¡°BackBuffer¡± or any other draw-able surface.
	DXGI_SWAP_CHAIN_DESC			clSwapChainDesc;						// create a struct to hold information about the swap chain
	D3D11_VIEWPORT					clViewport;								// Tells D3D11 what portion of the screen/surface you want to draw to. (typically all of it)
	
																			
	// TODO: PART 2 STEP 2

	// BEGIN PART 5
	// TODO: PART 5 STEP 1

	// TODO: PART 2 STEP 4

	// BEGIN PART 3
	// TODO: PART 3 STEP 1

	// TODO: PART 3 STEP 2b

	// TODO: PART 3 STEP 4a

public:
	// BEGIN PART 2
	// TODO: PART 2 STEP 1

	CHEN_APP(HINSTANCE hinst, WNDPROC proc);
	bool Run();
	bool ShutDown();
};

//************************************************************
//************ CREATION OF OBJECTS & RESOURCES ***************
//************************************************************

CHEN_APP::CHEN_APP(HINSTANCE hinst, WNDPROC proc) {
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

	window = CreateWindow(L"DirectXApplication", L"FS Graphic I - Lab - Chen Lu", WS_OVERLAPPEDWINDOW & ~(WS_THICKFRAME | WS_MAXIMIZEBOX),
		CW_USEDEFAULT, CW_USEDEFAULT, window_size.right - window_size.left, window_size.bottom - window_size.top,
		NULL, NULL, application, this);

	ShowWindow(window, SW_SHOW);
	//********************* END WARNING ************************//
	

	// clear out the struct for use
	ZeroMemory(&clSwapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC));

	// fill the swap chain description struct
	clSwapChainDesc.BufferCount = 1;                                    // one back buffer
	clSwapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;     // use 32-bit color
	clSwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;      // how swap chain is to be used
	clSwapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;		// Special Flags

	// create a device, device context and swap chain using the information in the scd struct
	// TODO: PART 1 STEP 3a
#if defined(DEBUG) | defined(_DEBUG)
	D3D11CreateDeviceAndSwapChain(NULL,
								  D3D_DRIVER_TYPE_HARDWARE,
								  NULL,
								  D3D11_CREATE_DEVICE_DEBUG,
								  NULL,
								  NULL,
								  D3D11_SDK_VERSION,
								  &clSwapChainDesc,
								  &clSwapChain,
								  &clDevice,
								  NULL,
								  &clDeviceContext);
#else
 	// TODO: PART 1 STEP 3b
	D3D11CreateDeviceAndSwapChain(NULL,
								  D3D_DRIVER_TYPE_HARDWARE,
								  NULL,
								  NULL,
								  NULL,
								  NULL,
								  D3D11_SDK_VERSION,
								  &clSwapChainDesc,
								  &clSwapChain,
								  &clDevice,
								  NULL,
								  &clDeviceContext);
#endif

	//		TODO: PART 1 STEP 4

	// TODO: PART 1 STEP 5

	// TODO: PART 2 STEP 3a

	// BEGIN PART 4
	// TODO: PART 4 STEP 1

	// TODO: PART 2 STEP 3b

	// TODO: PART 2 STEP 3c

	// TODO: PART 2 STEP 3d

	// TODO: PART 5 STEP 2a

	// TODO: PART 5 STEP 2b

	// TODO: PART 5 STEP 3

	// TODO: PART 2 STEP 5
	// ADD SHADERS TO PROJECT, SET BUILD OPTIONS & COMPILE

	// TODO: PART 2 STEP 7

	// TODO: PART 2 STEP 8a

	// TODO: PART 2 STEP 8b

	// TODO: PART 3 STEP 3

	// TODO: PART 3 STEP 4b

}

//************************************************************
//************ EXECUTION *************************************
//************************************************************

bool CHEN_APP::Run() {
	// TODO: PART 4 STEP 2	

	// TODO: PART 4 STEP 3

	// TODO: PART 4 STEP 5

	// END PART 4

	// TODO: PART 1 STEP 7a

	// TODO: PART 1 STEP 7b

	// TODO: PART 1 STEP 7c

	// TODO: PART 5 STEP 4

	// TODO: PART 5 STEP 5

	// TODO: PART 5 STEP 6

	// TODO: PART 5 STEP 7

	// END PART 5

	// TODO: PART 3 STEP 5

	// TODO: PART 3 STEP 6

	// TODO: PART 2 STEP 9a

	// TODO: PART 2 STEP 9b

	// TODO: PART 2 STEP 9c

	// TODO: PART 2 STEP 9d

	// TODO: PART 2 STEP 10

	// END PART 2

	// TODO: PART 1 STEP 8

	// END OF PART 1
	return true;
}

//************************************************************
//************ DESTRUCTION ***********************************
//************************************************************

bool CHEN_APP::ShutDown() {
	// TODO: PART 1 STEP 6
	if (clSwapChain != nullptr)
		clSwapChain->Release();
	if (clDevice != nullptr)
		clDevice->Release();
	if (clDeviceContext != nullptr)
		clDeviceContext->Release();

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
	CHEN_APP myApp(hInstance, (WNDPROC)WndProc);
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