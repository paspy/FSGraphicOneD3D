#include "LAB7.h"
#include "Mathlib.h"
// shader byte code
#include "Trivial_VS.csh"
#include "Trivial_PS.csh"

#include <iostream>

LAB7::LAB7(HINSTANCE hinst) : D3DApp(hinst),
	m_circleVertexBuffer(nullptr),
	m_constantBuffer(nullptr),
	m_inputLayout(nullptr),
	m_vertexShader(nullptr),
	m_pixelShader(nullptr) {

	World = XMMatrixIdentity();
	camView = XMMatrixIdentity();
	camProjection = XMMatrixIdentity();

	camUp = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

}

LAB7::~LAB7() {
	ReleaseCOM(m_circleVertexBuffer);
	ReleaseCOM(m_constantBuffer);
	ReleaseCOM(m_vertexShader);
	ReleaseCOM(m_pixelShader);
	ReleaseCOM(m_inputLayout);

	ReleaseCOM(cbPerObjectBuffer);

	ReleaseCOM(m_cubeIndexBuffer);

	ReleaseCOM(m_wireFrame);

	ReleaseCOM(m_cubesTexture);
	ReleaseCOM(m_cubesTexSamplerState);
}

bool LAB7::Init() {
	if (!D3DApp::Init())
		return false;

	BuildCameraBuffer();
	BuildGeometryBuffers(); 
	BuildTextureAndState();
	BuildVertexLayout();
	BuildShader();
	BuildRenderStates();

	return true;
}

void LAB7::OnResize() {
	D3DApp::OnResize();

}

void LAB7::BuildCameraBuffer() {
	D3D11_BUFFER_DESC cbbd;
	ZeroMemory(&cbbd, sizeof(D3D11_BUFFER_DESC));

	cbbd.Usage = D3D11_USAGE_DEFAULT;
	//cbbd.Usage = D3D11_USAGE_DYNAMIC;
	cbbd.ByteWidth = sizeof(cbPerObject);
	cbbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbbd.CPUAccessFlags = 0;
	cbbd.MiscFlags = 0;

	HR(m_d3dDevice->CreateBuffer(&cbbd, NULL, &cbPerObjectBuffer));

	// set up camera & projection mat
	//camPosition = XMVectorSet(0.0f, 0.0f, -2.5f, 0.0f);
	//camPosition = XMVectorSet(0.0f, 3.0f, -8.0f, 0.0f);
	//camTarget = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	//camView = XMMatrixLookAtLH(camPosition, camTarget, camUp);

	camPosition = XMVectorSet(0.0f, 0.0f, -0.5f, 1.0f);
	camProjection = XMMatrixPerspectiveFovLH(0.4f*3.14f, AspectRatio(), 1.0f, 1000.0f);
	camView = XMMatrixLookAtLH(camPosition, camTarget, camUp);

}

void LAB7::BuildGeometryBuffers() {
	//m_vertices.clear();
	//// build circle
	//for (int i = 0; i < 361; i++) {
	//	SIMPLE_VERTEX vert;
	//	vert.pos.x = 0.99f*sinf(DegreeToRadians((float)i));
	//	vert.pos.y = 0.99f*cosf(DegreeToRadians((float)i));
	//	vert.pos.z = 0.5f;
	//	XMStoreFloat4(&vert.color, Colors::Yellow);
	//	m_vertices.push_back(vert);
	//}

	//SIMPLE_VERTEX cubeVerteces[] = {
	//	SIMPLE_VERTEX(XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f)), // 0
	//	SIMPLE_VERTEX(XMFLOAT3(-1.0f, +1.0f, -1.0f), XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f)), // 1
	//	SIMPLE_VERTEX(XMFLOAT3(+1.0f, +1.0f, -1.0f), XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f)), // 2
	//	SIMPLE_VERTEX(XMFLOAT3(+1.0f, -1.0f, -1.0f), XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f)), // 3
	//	SIMPLE_VERTEX(XMFLOAT3(-1.0f, -1.0f, +1.0f), XMFLOAT4(0.0f, 1.0f, 1.0f, 1.0f)), // 4
	//	SIMPLE_VERTEX(XMFLOAT3(-1.0f, +1.0f, +1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f)), // 5
	//	SIMPLE_VERTEX(XMFLOAT3(+1.0f, +1.0f, +1.0f), XMFLOAT4(1.0f, 0.0f, 1.0f, 1.0f)), // 6
	//	SIMPLE_VERTEX(XMFLOAT3(+1.0f, -1.0f, +1.0f), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f)), // 7
	//};

	//DWORD cubeIndices[] = {
	//	// front face
	//	0, 1, 2,
	//	0, 2, 3,
	//	// back face
	//	4, 6, 5,
	//	4, 7, 6,
	//	// left face
	//	4, 5, 1,
	//	4, 1, 0,
	//	// right face
	//	3, 2, 6,
	//	3, 6, 7,
	//	// top face
	//	1, 5, 6,
	//	1, 6, 2,
	//	// bottom face
	//	4, 0, 3,
	//	4, 3, 7
	//};

	// Clockwise
	SIMPLE_VERTEX cubeVerteces[] = {
		// Front Face
		SIMPLE_VERTEX(XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT2(0.0f,  1.0f)),	// A
		SIMPLE_VERTEX(XMFLOAT3(-1.0f,  1.0f, -1.0f), XMFLOAT2(0.0f,  0.0f)),	// B
		SIMPLE_VERTEX(XMFLOAT3(+1.0f,  1.0f, -1.0f), XMFLOAT2(0.25f, 0.0f)),	// C
		SIMPLE_VERTEX(XMFLOAT3(+1.0f, -1.0f, -1.0f), XMFLOAT2(0.25f, 1.0f)),	// D

		// Back Face
		SIMPLE_VERTEX(XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT2(0.25f, 1.0f)),		
		SIMPLE_VERTEX(XMFLOAT3(+1.0f, -1.0f, 1.0f), XMFLOAT2(0.0f,  1.0f)),
		SIMPLE_VERTEX(XMFLOAT3(+1.0f,  1.0f, 1.0f), XMFLOAT2(0.0f,  0.0f)),
		SIMPLE_VERTEX(XMFLOAT3(-1.0f,  1.0f, 1.0f), XMFLOAT2(0.25f, 0.0f)),

		// Top Face
		SIMPLE_VERTEX(XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT2(0.0f,  1.0f)),
		SIMPLE_VERTEX(XMFLOAT3(-1.0f, 1.0f,  1.0f), XMFLOAT2(0.0f,  0.0f)),
		SIMPLE_VERTEX(XMFLOAT3(+1.0f, 1.0f,  1.0f), XMFLOAT2(0.25f, 0.0f)),
		SIMPLE_VERTEX(XMFLOAT3(+1.0f, 1.0f, -1.0f), XMFLOAT2(0.25f, 1.0f)),

		// Bottom Face
		SIMPLE_VERTEX(XMFLOAT3(-1.0f, -1.0f, -1.0f),XMFLOAT2(0.25f, 1.0f)),
		SIMPLE_VERTEX(XMFLOAT3(+1.0f, -1.0f, -1.0f),XMFLOAT2(0.0f,  1.0f)),
		SIMPLE_VERTEX(XMFLOAT3(+1.0f, -1.0f,  1.0f),XMFLOAT2(0.0f,  0.0f)),
		SIMPLE_VERTEX(XMFLOAT3(-1.0f, -1.0f,  1.0f),XMFLOAT2(0.25f, 0.0f)),

		// Left Face
		SIMPLE_VERTEX(XMFLOAT3(-1.0f, -1.0f,  1.0f), XMFLOAT2(0.0f,  1.0f)),
		SIMPLE_VERTEX(XMFLOAT3(-1.0f,  1.0f,  1.0f), XMFLOAT2(0.0f,  0.0f)),
		SIMPLE_VERTEX(XMFLOAT3(-1.0f,  1.0f, -1.0f), XMFLOAT2(0.25f, 0.0f)),
		SIMPLE_VERTEX(XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT2(0.25f, 1.0f)),

		// Right Face
		SIMPLE_VERTEX(XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT2(0.0f,  1.0f)),
		SIMPLE_VERTEX(XMFLOAT3(1.0f,  1.0f, -1.0f), XMFLOAT2(0.0f,  0.0f)),
		SIMPLE_VERTEX(XMFLOAT3(1.0f,  1.0f,  1.0f), XMFLOAT2(0.25f, 0.0f)),
		SIMPLE_VERTEX(XMFLOAT3(1.0f, -1.0f,  1.0f), XMFLOAT2(0.25f, 1.0f)),
	};

	DWORD cubeIndices[] = {
		// Front Face
		0,  1,  2,
		0,  2,  3,

		// Back Face
		4,  5,  6,
		4,  6,  7,

		// Top Face
		8,  9, 10,
		8, 10, 11,

		// Bottom Face
		12, 13, 14,
		12, 14, 15,

		// Left Face
		16, 17, 18,
		16, 18, 19,

		// Right Face
		20, 21, 22,
		20, 22, 23
	};


	//XMStoreFloat4(&m_vertConstData.constantColor, Colors::Yellow);
	//m_vertConstData.constantOffset = { 0, 0 };
	//D3D11_BUFFER_DESC consBufferDesc;
	//consBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	//consBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	//consBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	//consBufferDesc.MiscFlags = 0;
	//consBufferDesc.StructureByteStride = 0;
	//consBufferDesc.ByteWidth = sizeof(SEND_TO_VRAM);
	//// Create Constant Buffer
	//HR(m_d3dDevice->CreateBuffer(&consBufferDesc, NULL, &m_constantBuffer));
	//// Set Constant buffer
	//m_d3dImmediateContext->VSSetConstantBuffers(0, 1, &m_constantBuffer);


	D3D11_BUFFER_DESC indexBufferDesc;
	ZeroMemory(&indexBufferDesc, sizeof(indexBufferDesc));

	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.ByteWidth = sizeof(DWORD) * 12 * 3;

	D3D11_SUBRESOURCE_DATA iinitData;
	iinitData.pSysMem = cubeIndices;
	// Create Index Buffer
	HR(m_d3dDevice->CreateBuffer(&indexBufferDesc, &iinitData, &m_cubeIndexBuffer));
	// Set indeces buffer
	m_d3dImmediateContext->IASetIndexBuffer(m_cubeIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

	D3D11_BUFFER_DESC vertexBufferDesc;
	vertexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = NULL;
	vertexBufferDesc.MiscFlags = 0;
	//vertexBufferDesc.ByteWidth = static_cast<UINT>(sizeof(SIMPLE_VERTEX) * m_vertices.size());
	vertexBufferDesc.ByteWidth = static_cast<UINT>(sizeof(SIMPLE_VERTEX) * 24);

	D3D11_SUBRESOURCE_DATA vinitData;
	//vinitData.pSysMem = m_vertices.data();
	vinitData.pSysMem = cubeVerteces;

	// Create Verteces Buffer
	HR(m_d3dDevice->CreateBuffer(&vertexBufferDesc, &vinitData, &m_circleVertexBuffer));
	// Set verteces buffer
	UINT stride = sizeof(SIMPLE_VERTEX), offset = 0;
	m_d3dImmediateContext->IASetVertexBuffers(0, 1, &m_circleVertexBuffer, &stride, &offset);

	m_d3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void LAB7::BuildTextureAndState() {

	// load texture from file
	HR(CreateDDSTextureFromFile(m_d3dDevice, L"Resource/numbers_test.dds", NULL, &m_cubesTexture));

	// Describe the Sample State
	D3D11_SAMPLER_DESC sampDesc;
	ZeroMemory(&sampDesc, sizeof(sampDesc));

	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;

	//Create the Sample State
	HR(m_d3dDevice->CreateSamplerState(&sampDesc, &m_cubesTexSamplerState));

}

void LAB7::BuildShader() {
	HR(m_d3dDevice->CreateVertexShader(Trivial_VS, sizeof(Trivial_VS), NULL, &m_vertexShader));
	m_d3dImmediateContext->VSSetShader(m_vertexShader, NULL, 0);

	HR(m_d3dDevice->CreatePixelShader(Trivial_PS, sizeof(Trivial_PS), NULL, &m_pixelShader));
	m_d3dImmediateContext->PSSetShader(m_pixelShader, NULL, 0);

}

void LAB7::BuildVertexLayout() {
	// create input layout describing our geometry
	//D3D11_INPUT_ELEMENT_DESC vertLayout[] = {
	//	{ "POSITION",	0, DXGI_FORMAT_R32G32B32_FLOAT,		0, D3D11_APPEND_ALIGNED_ELEMENT,	D3D11_INPUT_PER_VERTEX_DATA, 0 },
	//	{ "COLOR",		0, DXGI_FORMAT_R32G32B32A32_FLOAT,	0, D3D11_APPEND_ALIGNED_ELEMENT,	D3D11_INPUT_PER_VERTEX_DATA, 0 }
	//};
	D3D11_INPUT_ELEMENT_DESC vertLayout[] = {
		{ "POSITION",	0, DXGI_FORMAT_R32G32B32_FLOAT,	0, D3D11_APPEND_ALIGNED_ELEMENT,	D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD",	0, DXGI_FORMAT_R32G32_FLOAT,	0, D3D11_APPEND_ALIGNED_ELEMENT,	D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	// Create the input layout
	HR(m_d3dDevice->CreateInputLayout(vertLayout, 2, Trivial_VS, sizeof(Trivial_VS), &m_inputLayout));
	m_d3dImmediateContext->IASetInputLayout(m_inputLayout);

}

void LAB7::BuildRenderStates() {
	D3D11_RASTERIZER_DESC rasterDesc;
	ZeroMemory(&rasterDesc, sizeof(D3D11_RASTERIZER_DESC));

	rasterDesc.FillMode = D3D11_FILL_SOLID;
	rasterDesc.CullMode = D3D11_CULL_NONE;

	HR(m_d3dDevice->CreateRasterizerState(&rasterDesc, &m_wireFrame));

	// Apply change to render state. Default: NULL
	m_d3dImmediateContext->RSSetState(m_wireFrame);
	//m_d3dImmediateContext->RSSetState(NULL);
}


void LAB7::UpdateKeyboardInput(double _dt) {
	static float cam_x = 0.0f;
	static float cam_y = 0.0f;
	static float cam_z = -0.5f;
	if (GetAsyncKeyState(VK_LW)) {
		cam_y += (float)_dt * 10.0f;
		//m_mouseRadius -= (float)_dt * 10.0f;
		//m_mouseRadius = Mathlib::Clamp(m_mouseRadius, -20.0f, 20.0f);
	}
	if (GetAsyncKeyState(VK_LS)) {
		//m_mouseRadius += (float)_dt * 10.0f;
		//m_mouseRadius = Mathlib::Clamp(m_mouseRadius, -20.0f, 20.0f);
		cam_y -= (float)_dt * 10.0f;
	}

	if (GetAsyncKeyState(VK_LA)) {
		cam_x -= (float)_dt * 10.0f;
	}

	if (GetAsyncKeyState(VK_LD)) {
		cam_x += (float)_dt * 10.0f;
	}

	float x = cam_x*sinf(m_mouseTheta) - cam_y*cosf(m_mousePhi);
	float y = cam_x*sinf(m_mouseTheta) - cam_y*cosf(m_mousePhi);
	float z = cam_z;

	camPosition = XMVectorSet(x, y, z, 1.0f);

}

void LAB7::UpdateCamera() {
	// Update view matrix (camera)

	//float cam_x = m_mouseRadius*sinf(m_mousePhi)*cosf(m_mouseTheta);
	//float cam_z = m_mouseRadius*sinf(m_mousePhi)*sinf(m_mouseTheta);
	//float cam_y = m_mouseRadius*cosf(m_mousePhi);

	camTarget = XMVectorZero();

	camView = XMMatrixLookAtLH(camPosition, camTarget, camUp);
}

void LAB7::UpdateScene(double _dt) {

	// Update objects
	static double texIdx = 0;
	static float rot = 0.00f;

	rot += (float)_dt;

	if (rot > 6.26f) rot = 0.0f;

	texIdx += _dt;

	cbPerObj.texIndex = (int)texIdx;

	if ((int)texIdx > 3) texIdx = 0;

	cubeWorldMat = XMMatrixIdentity();

	XMVECTOR rotAxis_X = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
	XMVECTOR rotAxis_Y = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	XMVECTOR rotAxis_Z = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);

	XMMATRIX rotationMat =  XMMatrixRotationAxis(rotAxis_X, rot) * XMMatrixRotationAxis(rotAxis_Y, rot);
	XMMATRIX translationMat = XMMatrixTranslation(0.0f, 0.0f, -2.0f);

	cubeWorldMat = rotationMat * translationMat;


}

void LAB7::DrawScene() {
	assert(m_d3dImmediateContext);
	assert(m_swapChain);

	m_d3dImmediateContext->ClearRenderTargetView(m_renderTargetView, reinterpret_cast<const float*>(&Colors::Black));
	m_d3dImmediateContext->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	// Set Shader Resources and Samplers
	m_d3dImmediateContext->PSSetShaderResources(0, 1, &m_cubesTexture);
	m_d3dImmediateContext->PSSetSamplers(0, 1, &m_cubesTexSamplerState);

	// Map & Unmap Constant Buffer
	//D3D11_MAPPED_SUBRESOURCE mapSubres;
	//m_d3dImmediateContext->Map(m_constantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapSubres);
	//memcpy(mapSubres.pData, &m_vertConstData, sizeof(m_vertConstData));
	//m_d3dImmediateContext->Unmap(m_constantBuffer, 0);

	//Set the WVP matrix and send it to the constant buffe in shader
	WVP = cubeWorldMat * camView * camProjection;
	cbPerObj.WVP = XMMatrixTranspose(WVP);
	m_d3dImmediateContext->UpdateSubresource(cbPerObjectBuffer, 0, NULL, &cbPerObj, 0, 0);
	m_d3dImmediateContext->VSSetConstantBuffers(0, 1, &cbPerObjectBuffer);
	// Draw thing
	//m_d3dImmediateContext->Draw((UINT)m_vertices.size(), 0);
	m_d3dImmediateContext->DrawIndexed(36, 0, 0);

	//Present the backbuffer to the screen
	HR(m_swapChain->Present(0, 0));
}

void LAB7::OnMouseDown(WPARAM _btnState, int _x, int _y) {
	m_lastMousePos.x = _x;
	m_lastMousePos.y = _y;

	SetCapture(window);
}

void LAB7::OnMouseUp(WPARAM _btnState, int _x, int _y) {
	ReleaseCapture();
}

void LAB7::OnMouseMove(WPARAM _btnState, int _x, int _y) {
	if ((MK_RBUTTON & _btnState) != 0) {

		// Make each pixel correspond to a quarter of a degree.
		float dx = XMConvertToRadians(0.25f*static_cast<float>(_x - m_lastMousePos.x));
		float dy = XMConvertToRadians(0.25f*static_cast<float>(_y - m_lastMousePos.y));

		// Update angles based on input to orbit camera around box.
		m_mouseTheta -= dx;
		m_mousePhi -= dy;

		// Restrict the angle mPhi.
		m_mousePhi = Mathlib::Clamp(m_mousePhi, 0.1f, XM_PI - 0.1f);
	} 
	//else if ((MK_LBUTTON & _btnState) != 0) {
	//	// Make each pixel correspond to 0.005 unit in the scene.
	//	float dx = 0.005f*static_cast<float>(_x - m_lastMousePos.x);
	//	float dy = 0.005f*static_cast<float>(_y - m_lastMousePos.y);
	//	// Update the camera radius based on input.
	//	m_mouseRadius += dx - dy;
	//	// Restrict the radius.
	//	m_mouseRadius = Mathlib::Clamp(m_mouseRadius, 3.0f, 15.0f);
	//}
	m_lastMousePos.x = _x;
	m_lastMousePos.y = _y;
}