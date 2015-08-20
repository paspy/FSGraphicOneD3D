#pragma once

#include <DirectXMath.h>

//#if defined(DEBUG) || defined(_DEBUG)
//#define _CRTDBG_MAP_ALLOC
//#include <crtdbg.h>
//#endif

// Convenience macro for releasing COM objects.
#define ReleaseCOM(x) { if(x){ x->Release(); x = nullptr; } }

// Convenience macro for deleting objects.
#define SafeDelete(x) { delete x; x = nullptr; }

// d3d error checker
#if defined(DEBUG) | defined(_DEBUG)
#ifndef HR
#define HR(x) {												\
		HRESULT hr = (x);									\
		if(FAILED(hr)) {									\
			LPWSTR output;									\
			FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM |		\
				FORMAT_MESSAGE_IGNORE_INSERTS 	 |			\
				FORMAT_MESSAGE_ALLOCATE_BUFFER,				\
				NULL,										\
				hr,											\
				MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),	\
				(LPTSTR) &output,							\
				0,											\
				NULL);										\
			MessageBox(NULL, output, L"Error", MB_OK);		\
		}													\
	}
#endif
#else
#ifndef HR
#define HR(x) (x)
#endif
#endif 

#define BACKBUFFER_WIDTH	500
#define BACKBUFFER_HEIGHT	500

#define VK_LW 0x57
#define VK_LS 0x53
#define VK_LA 0x41
#define VK_LD 0x44
