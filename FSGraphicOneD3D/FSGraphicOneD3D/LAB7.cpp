#include "LAB7.h"
#include "Mathlib.h"
// shader byte code
#include "Trivial_VS.csh"
#include "Trivial_PS.csh"

LAB7::LAB7(HINSTANCE hinst) : D3DApp(hinst),
	m_circleVertexBuffer(nullptr),
	m_constantBuffer(nullptr),
	m_inputLayout(nullptr),
	m_vertexShader(nullptr),
	m_pixelShader(nullptr) {

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
}

bool LAB7::Init() {
	if (!D3DApp::Init())
		return false;

	BuildCameraBuffer();
	BuildGeometryBuffers(); 
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
	camPosition = XMVectorSet(0.0f, 3.0f, -8.0f, 0.0f);
	camTarget = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	camUp = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	camView = XMMatrixLookAtLH(camPosition, camTarget, camUp);

	camProjection = XMMatrixPerspectiveFovLH(0.4f*3.14f, AspectRatio(), 1.0f, 1000.0f);

}

void LAB7::BuildGeometryBuffers() {
	m_vertices.clear();

	// build circle
	for (int i = 0; i < 361; i++) {
		SIMPLE_VERTEX vert;
		vert.pos.x = 0.99f*sinf(DegreeToRadians((float)i));
		vert.pos.y = 0.99f*cosf(DegreeToRadians((float)i));
		vert.pos.z = 0.5f;
		XMStoreFloat4(&vert.color, Colors::Yellow);
		m_vertices.push_back(vert);
	}

	SIMPLE_VERTEX cubeVerteces[] = {
		SIMPLE_VERTEX(XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f)), // 0
		SIMPLE_VERTEX(XMFLOAT3(-1.0f, +1.0f, -1.0f), XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f)), // 1
		SIMPLE_VERTEX(XMFLOAT3(+1.0f, +1.0f, -1.0f), XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f)), // 2
		SIMPLE_VERTEX(XMFLOAT3(+1.0f, -1.0f, -1.0f), XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f)), // 3
		SIMPLE_VERTEX(XMFLOAT3(-1.0f, -1.0f, +1.0f), XMFLOAT4(0.0f, 1.0f, 1.0f, 1.0f)), // 4
		SIMPLE_VERTEX(XMFLOAT3(-1.0f, +1.0f, +1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f)), // 5
		SIMPLE_VERTEX(XMFLOAT3(+1.0f, +1.0f, +1.0f), XMFLOAT4(1.0f, 0.0f, 1.0f, 1.0f)), // 6
		SIMPLE_VERTEX(XMFLOAT3(+1.0f, -1.0f, +1.0f), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f)), // 7
	};

	DWORD cubeIndices[] = {
		// front face
		0, 1, 2,
		0, 2, 3,

		// back face
		4, 6, 5,
		4, 7, 6,

		// left face
		4, 5, 1,
		4, 1, 0,

		// right face
		3, 2, 6,
		3, 6, 7,

		// top face
		1, 5, 6,
		1, 6, 2,

		// bottom face
		4, 0, 3,
		4, 3, 7
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
	vertexBufferDesc.ByteWidth = static_cast<UINT>(sizeof(SIMPLE_VERTEX) * 8);

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

void LAB7::BuildShader() {
	HR(m_d3dDevice->CreateVertexShader(Trivial_VS, sizeof(Trivial_VS), NULL, &m_vertexShader));
	m_d3dImmediateContext->VSSetShader(m_vertexShader, NULL, 0);

	HR(m_d3dDevice->CreatePixelShader(Trivial_PS, sizeof(Trivial_PS), NULL, &m_pixelShader));
	m_d3dImmediateContext->PSSetShader(m_pixelShader, NULL, 0);

}

void LAB7::BuildVertexLayout() {
	// create input layout describing our geometry
	D3D11_INPUT_ELEMENT_DESC vertLayout[] = {
		{ "POSITION",	0, DXGI_FORMAT_R32G32B32_FLOAT,		0, D3D11_APPEND_ALIGNED_ELEMENT,	D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR",		0, DXGI_FORMAT_R32G32B32A32_FLOAT,	0, D3D11_APPEND_ALIGNED_ELEMENT,	D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	// Create the input layout
	HR(m_d3dDevice->CreateInputLayout(vertLayout, 2, Trivial_VS, sizeof(Trivial_VS), &m_inputLayout));
	m_d3dImmediateContext->IASetInputLayout(m_inputLayout);

}

void LAB7::BuildRenderStates() {
	D3D11_RASTERIZER_DESC rasterDesc;
	ZeroMemory(&rasterDesc, sizeof(D3D11_RASTERIZER_DESC));

	rasterDesc.FillMode = D3D11_FILL_WIREFRAME;
	rasterDesc.CullMode = D3D11_CULL_NONE;

	HR(m_d3dDevice->CreateRasterizerState(&rasterDesc, &m_wireFrame));

	// Apply change to render state. Default: NULL
	m_d3dImmediateContext->RSSetState(m_wireFrame);
}


void LAB7::UpdateScene(double _dt) {
	rot += (float)_dt;

	if (rot > 6.26f) rot = 0.0f;

	cubeWorldMat = XMMatrixIdentity();

	XMVECTOR rotAxis_X = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
	XMVECTOR rotAxis_Y = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	XMVECTOR rotAxis_Z = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);

	XMMATRIX rotationMat =  XMMatrixRotationAxis(rotAxis_Y, rot) * XMMatrixRotationAxis(rotAxis_X, rot);
	XMMATRIX translationMat = XMMatrixTranslation(0.0f, 0.0f, -2.0f);

	cubeWorldMat = rotationMat * translationMat;
}

void LAB7::DrawScene() {
	assert(m_d3dImmediateContext);
	assert(m_swapChain);

	m_d3dImmediateContext->ClearRenderTargetView(m_renderTargetView, reinterpret_cast<const float*>(&Colors::Black));
	m_d3dImmediateContext->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	// Caculate World Mat
	World = XMMatrixIdentity();
	WVP = World * camView * camProjection;
	cbPerObj.WVP = XMMatrixTranspose(WVP);
	//D3D11_MAPPED_SUBRESOURCE camSubres;
	//m_d3dImmediateContext->Map(cbPerObjectBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &camSubres);
	//memcpy(camSubres.pData, &cbPerObj, sizeof(cbPerObj));
	//m_d3dImmediateContext->Unmap(cbPerObjectBuffer, 0);
	m_d3dImmediateContext->UpdateSubresource(cbPerObjectBuffer, 0, NULL, &cbPerObj, 0, 0);
	m_d3dImmediateContext->VSSetConstantBuffers(0, 1, &cbPerObjectBuffer);

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



