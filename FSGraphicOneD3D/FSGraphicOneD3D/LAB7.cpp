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
}

bool LAB7::Init() {
	if (!D3DApp::Init())
		return false;

	BuildGeometryBuffers();
	BuildShader();
	BuildVertexLayout();

	return true;
}

void LAB7::OnResize() {
	D3DApp::OnResize();

}

void LAB7::UpdateScene(double _dt) {

}

void LAB7::DrawScene() {
	assert(m_d3dImmediateContext);
	assert(m_swapChain);

	m_d3dImmediateContext->ClearRenderTargetView(m_renderTargetView, reinterpret_cast<const float*>(&Colors::DarkBlue));
	m_d3dImmediateContext->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	//UINT stride = sizeof(Vertex4);
	UINT stride = sizeof(SIMPLE_VERTEX);
	UINT offset = 0;

	// set buffer
	m_d3dImmediateContext->IASetVertexBuffers(0, 1, &m_circleVertexBuffer, &stride, &offset);

	// Map & Unmap
	D3D11_MAPPED_SUBRESOURCE mapSubres;
	m_d3dImmediateContext->Map(m_constantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapSubres);
	memcpy(mapSubres.pData, &m_vertConstData, sizeof(m_vertConstData));
	m_d3dImmediateContext->Unmap(m_constantBuffer, 0);

	// set constant buffer
	m_d3dImmediateContext->VSSetConstantBuffers(0, 1, &m_constantBuffer);

	m_d3dImmediateContext->IASetInputLayout(m_inputLayout);
	m_d3dImmediateContext->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINESTRIP);

	m_d3dImmediateContext->VSSetShader(m_vertexShader, NULL, 0);
	m_d3dImmediateContext->PSSetShader(m_pixelShader, NULL, 0);

	m_d3dImmediateContext->Draw((UINT)m_vertices.size(), 0);

	HR(m_swapChain->Present(0, 0));
}

void LAB7::BuildGeometryBuffers() {
	m_vertices.clear();

	// build circle
	for (int i = 0; i < 361; i++) {
		SIMPLE_VERTEX vert;
		vert.pos.x = 0.99f*sinf(DegreeToRadians((float)i));
		vert.pos.y = 0.99f*cosf(DegreeToRadians((float)i));
		m_vertices.push_back(vert);
	}

	m_vertConstData.constantColor = { 1.0f, 1.0f, 0.0f, 1.0f };
	m_vertConstData.constantOffset = { 0, 0 };

	D3D11_BUFFER_DESC consBufferDesc;
	consBufferDesc.ByteWidth = sizeof(SEND_TO_VRAM);
	consBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	consBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	consBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	consBufferDesc.MiscFlags = 0;
	consBufferDesc.StructureByteStride = 0;

	// Create Constant Buffer
	HR(m_d3dDevice->CreateBuffer(&consBufferDesc, NULL, &m_constantBuffer));

	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = NULL;
	vbd.ByteWidth = static_cast<UINT>(sizeof(SIMPLE_VERTEX) * m_vertices.size());
	vbd.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA vinitData;
	vinitData.pSysMem = m_vertices.data();

	// Create Buffer
	HR(m_d3dDevice->CreateBuffer(&vbd, &vinitData, &m_circleVertexBuffer));
}

void LAB7::BuildShader() {
	HR(m_d3dDevice->CreateVertexShader(Trivial_VS, sizeof(Trivial_VS), NULL, &m_vertexShader));
	HR(m_d3dDevice->CreatePixelShader(Trivial_PS, sizeof(Trivial_PS), NULL, &m_pixelShader));
}

void LAB7::BuildVertexLayout() {
	// create input layout describing our geometry
	D3D11_INPUT_ELEMENT_DESC vertLayout[] = {
		{ "POSITION",	0, DXGI_FORMAT_R32G32B32_FLOAT,		0, D3D11_APPEND_ALIGNED_ELEMENT,	D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR",		0, DXGI_FORMAT_R32G32B32A32_FLOAT,	0, D3D11_APPEND_ALIGNED_ELEMENT,	D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	// Create the input layout
	HR(m_d3dDevice->CreateInputLayout(vertLayout, 2, Trivial_VS, sizeof(Trivial_VS), &m_inputLayout));
}
