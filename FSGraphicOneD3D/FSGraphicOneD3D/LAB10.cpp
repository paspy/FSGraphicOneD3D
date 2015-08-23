#include "LAB10.h"
#include "Mathlib.h"
// shader byte code
#include "Trivial_VS.csh"
#include "Trivial_PS.csh"

#include <iostream>

LAB10::LAB10(HINSTANCE hinst) : D3DApp(hinst),
	m_circleVertexBuffer(nullptr),
	m_gridVertexBuffer(nullptr),
	m_inputLayout(nullptr),
	m_vertexShader(nullptr),
	m_pixelShader(nullptr) {

	World = XMMatrixIdentity();
	camView = XMMatrixIdentity();
	camProjection = XMMatrixIdentity();

	camUp = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

}

LAB10::~LAB10() {
	ReleaseCOM(m_circleVertexBuffer);
	ReleaseCOM(m_gridVertexBuffer);
	ReleaseCOM(m_vertexShader);
	ReleaseCOM(m_pixelShader);
	ReleaseCOM(m_inputLayout);

	ReleaseCOM(cbPerObjectBuffer);

	ReleaseCOM(m_cubeIndexBuffer);

	ReleaseCOM(m_wireFrame);

	ReleaseCOM(m_cubesTexture);
	ReleaseCOM(m_cubesTexSamplerState);


	ReleaseCOM(m_blendTransparency);
	ReleaseCOM(m_cwCullingMode);
	ReleaseCOM(m_ccwCullingMode);
}

bool LAB10::Init() {
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

void LAB10::OnResize() {
	D3DApp::OnResize();

}

void LAB10::BuildCameraBuffer() {
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
	//camView = XMMatrixLookAtLH(camPosition, camTarget, camUp);

	camPosition = XMVectorSet(0.0f, 0.0f, -0.5f, 1.0f);
	camTarget = XMVectorSet(0.0f, 0.0f, 1.0f, 1.0f);
	camProjection = XMMatrixPerspectiveFovLH(0.4f*3.14f, AspectRatio(), 1.0f, 1000.0f);
	camView = XMMatrixLookAtLH(camPosition, camTarget, camUp);

}

void LAB10::BuildGeometryBuffers() {
	
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
	//UINT stride = sizeof(SIMPLE_VERTEX), offset = 0;
	//m_d3dImmediateContext->IASetVertexBuffers(0, 1, &m_circleVertexBuffer, &stride, &offset);

	
	BuildGridBuffers();
}

void LAB10::BuildGridBuffers() {

	for (int i = -10; i <= 10; i++) {
		XMFLOAT4 color;
		XMStoreFloat4(&color, Colors::LightGreen);
		SIMPLE_VERTEX vert1(XMFLOAT3((float)i, -1.0f, +10.0f), color);
		SIMPLE_VERTEX vert2(XMFLOAT3((float)i, -1.0f, -10.0f), color);
		SIMPLE_VERTEX vert3(XMFLOAT3(+10.0f, -1.0f, (float)i), color);
		SIMPLE_VERTEX vert4(XMFLOAT3(-10.0f, -1.0f, (float)i), color);
		m_gridVerts.push_back(vert1);
		m_gridVerts.push_back(vert2);
		m_gridVerts.push_back(vert3);
		m_gridVerts.push_back(vert4);
	}

	D3D11_BUFFER_DESC vertexBufferDesc;
	vertexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = NULL;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.ByteWidth = static_cast<UINT>(sizeof(SIMPLE_VERTEX) * m_gridVerts.size());

	D3D11_SUBRESOURCE_DATA vinitData;
	vinitData.pSysMem = m_gridVerts.data();

	// Create Verteces Buffer
	HR(m_d3dDevice->CreateBuffer(&vertexBufferDesc, &vinitData, &m_gridVertexBuffer));
}

void LAB10::BuildTextureAndState() {

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

void LAB10::BuildShader() {
	HR(m_d3dDevice->CreateVertexShader(Trivial_VS, sizeof(Trivial_VS), NULL, &m_vertexShader));
	m_d3dImmediateContext->VSSetShader(m_vertexShader, NULL, 0);

	HR(m_d3dDevice->CreatePixelShader(Trivial_PS, sizeof(Trivial_PS), NULL, &m_pixelShader));
	m_d3dImmediateContext->PSSetShader(m_pixelShader, NULL, 0);

}

void LAB10::BuildVertexLayout() {

	D3D11_INPUT_ELEMENT_DESC vertLayout[] = {
		{ "POSITION",	0, DXGI_FORMAT_R32G32B32_FLOAT,	0, D3D11_APPEND_ALIGNED_ELEMENT,	D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR",		0, DXGI_FORMAT_R32G32B32A32_FLOAT,	0, D3D11_APPEND_ALIGNED_ELEMENT,	D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD",	0, DXGI_FORMAT_R32G32_FLOAT,	0, D3D11_APPEND_ALIGNED_ELEMENT,	D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	// Create the input layout
	HR(m_d3dDevice->CreateInputLayout(vertLayout, 3, Trivial_VS, sizeof(Trivial_VS), &m_inputLayout));
	m_d3dImmediateContext->IASetInputLayout(m_inputLayout);

}

void LAB10::BuildRenderStates() {

	// Raster Description	
	D3D11_RASTERIZER_DESC rasterDesc;
	ZeroMemory(&rasterDesc, sizeof(D3D11_RASTERIZER_DESC));

	rasterDesc.FillMode = D3D11_FILL_SOLID;
	rasterDesc.CullMode = D3D11_CULL_NONE;
	rasterDesc.AntialiasedLineEnable = true;

	HR(m_d3dDevice->CreateRasterizerState(&rasterDesc, &m_wireFrame));

	// Apply change to render state. Default: NULL
	//m_d3dImmediateContext->RSSetState(m_wireFrame);		- disabled when blending
	//m_d3dImmediateContext->RSSetState(NULL);

	// create blending description
	D3D11_BLEND_DESC blendDesc;
	ZeroMemory(&blendDesc, sizeof(blendDesc));

	D3D11_RENDER_TARGET_BLEND_DESC renderTargetBlendDesc;
	ZeroMemory(&renderTargetBlendDesc, sizeof(renderTargetBlendDesc));

	renderTargetBlendDesc.BlendEnable			= true;
	renderTargetBlendDesc.SrcBlend				= D3D11_BLEND_SRC_COLOR;
	renderTargetBlendDesc.DestBlend				= D3D11_BLEND_BLEND_FACTOR;
	renderTargetBlendDesc.BlendOp				= D3D11_BLEND_OP_ADD;
	renderTargetBlendDesc.SrcBlendAlpha			= D3D11_BLEND_ONE;
	renderTargetBlendDesc.DestBlendAlpha		= D3D11_BLEND_ZERO;
	renderTargetBlendDesc.BlendOpAlpha			= D3D11_BLEND_OP_ADD;
	renderTargetBlendDesc.RenderTargetWriteMask = D3D10_COLOR_WRITE_ENABLE_ALL;

	blendDesc.AlphaToCoverageEnable = false;
	blendDesc.RenderTarget[0] = renderTargetBlendDesc;

	HR(m_d3dDevice->CreateBlendState(&blendDesc, &m_blendTransparency));

	// create counter-clockwise and clockwise description
	D3D11_RASTERIZER_DESC cmdesc;
	ZeroMemory(&cmdesc, sizeof(D3D11_RASTERIZER_DESC));

	cmdesc.FillMode = D3D11_FILL_SOLID;
	cmdesc.CullMode = D3D11_CULL_BACK;

	cmdesc.FrontCounterClockwise = true;
	HR(m_d3dDevice->CreateRasterizerState(&cmdesc, &m_ccwCullingMode));

	cmdesc.FrontCounterClockwise = false;
	HR(m_d3dDevice->CreateRasterizerState(&cmdesc, &m_cwCullingMode));
}

void LAB10::UpdateKeyboardInput(double _dt) {

	if (GetAsyncKeyState(VK_LW)) {
		moveBackForward += (float)_dt * 10.0f;
	}

	if (GetAsyncKeyState(VK_LS)) {
		moveBackForward -= (float)_dt * 10.0f;
	}

	if (GetAsyncKeyState(VK_LA)) {
		moveLeftRight -= (float)_dt * 10.0f;
	}

	if (GetAsyncKeyState(VK_LD)) {
		moveLeftRight += (float)_dt * 10.0f;
	}

}

void LAB10::UpdateCamera() {

	camRotationMatrix = XMMatrixRotationRollPitchYaw(camPitch, camYaw, 0);
	camTarget = XMVector3TransformCoord(DefaultForward, camRotationMatrix);

	camTarget = XMVector3Normalize(camTarget);

	XMMATRIX RotateYTempMatrix;

	RotateYTempMatrix = XMMatrixRotationY(camYaw);

	//camForward = XMVector3TransformCoord(DefaultForward, RotateYTempMatrix);	// walk on ground

	camForward = camTarget;

	camRight = XMVector3TransformCoord(DefaultRight, RotateYTempMatrix);

	camUp = XMVector3TransformCoord(camUp, RotateYTempMatrix);
	camPosition += moveLeftRight*camRight + moveBackForward*camForward;
	camTarget += camPosition;

	camView = XMMatrixLookAtLH(camPosition, camTarget, camUp);

	moveLeftRight = 0.0f;
	moveBackForward = 0.0f;
}

void LAB10::UpdateScene(double _dt) {

	// Update objects
	static double texIdx = 0;
	static float rot = 0.00f;

	rot += (float)_dt;

	if (rot > 6.26f) rot = 0.0f;

	texIdx += _dt;

	cbPerObj.texIndex = (int)texIdx;

	if ((int)texIdx > 3) texIdx = 0;

	cubeWorldMat = XMMatrixIdentity();
	gridWorldMat = XMMatrixIdentity();

	XMVECTOR rotAxis_X = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
	XMVECTOR rotAxis_Y = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	XMVECTOR rotAxis_Z = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);

	XMMATRIX rotationMat =  XMMatrixRotationAxis(rotAxis_X, rot) * XMMatrixRotationAxis(rotAxis_Y, rot);
	XMMATRIX translationMat = XMMatrixTranslation(0.0f, 0.0f, -2.0f);

	cubeWorldMat = rotationMat * translationMat;


}

void LAB10::DrawScene() {
	assert(m_d3dImmediateContext);
	assert(m_swapChain);

	//Refresh the render target view
	m_d3dImmediateContext->ClearRenderTargetView(m_renderTargetView, reinterpret_cast<const float*>(&Colors::Black));
	
	//Refresh the Depth/Stencil view
	m_d3dImmediateContext->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	// Set Shader Resources and Samplers
	m_d3dImmediateContext->PSSetShaderResources(0, 1, &m_cubesTexture);
	m_d3dImmediateContext->PSSetSamplers(0, 1, &m_cubesTexSamplerState);

	// opaque objects drawing

	//"fine-tune" the blending equation
	float blendFactor[] = { 0.75f, 0.75f, 0.75f, 1.0f };

	//Set the default blend state (no blending) for opaque objects
	m_d3dImmediateContext->OMSetBlendState(0, 0, 0xffffffff);

	//Render opaque objects//

	//Set the blend state for transparent objects
	m_d3dImmediateContext->OMSetBlendState(m_blendTransparency, blendFactor, 0xffffffff);

	//*****Transparency Depth Ordering*****//
	// Find which transparent object is further from the camera
	// So we can render the objects in depth order to the render target
	// Puting the objects into a vector to organize the order of distance
	// Find distance from cube to camera
	//XMVECTOR objPos = XMVectorZero();

	//objPos = XMVector3TransformCoord(objPos, cubeWorldMat);

	//float distX = XMVectorGetX(objPos) - XMVectorGetX(camPosition);
	//float distY = XMVectorGetY(objPos) - XMVectorGetY(camPosition);
	//float distZ = XMVectorGetZ(objPos) - XMVectorGetZ(camPosition);

	//float cubeDist = distX*distX + distY*distY + distZ*distZ;

	//objPos = XMVectorZero();

	//objPos = XMVector3TransformCoord(objPos, gridWorldMat);

	//distX = XMVectorGetX(objPos) - XMVectorGetX(camPosition);
	//distY = XMVectorGetY(objPos) - XMVectorGetY(camPosition);
	//distZ = XMVectorGetZ(objPos) - XMVectorGetZ(camPosition);

	//float gridDist = distX*distX + distY*distY + distZ*distZ;

	UINT stride = sizeof(SIMPLE_VERTEX), offset = 0;
	// Draw Grid
	WVP = gridWorldMat * camView * camProjection;
	cbPerObj.WVP = XMMatrixTranspose(WVP);
	m_d3dImmediateContext->UpdateSubresource(cbPerObjectBuffer, 0, NULL, &cbPerObj, 0, 0);
	m_d3dImmediateContext->VSSetConstantBuffers(0, 1, &cbPerObjectBuffer);
	m_d3dImmediateContext->RSSetState(m_wireFrame);	// Set raster setting
	m_d3dImmediateContext->IASetVertexBuffers(0, 1, &m_gridVertexBuffer, &stride, &offset);
	m_d3dImmediateContext->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);

	m_d3dImmediateContext->Draw((UINT)m_gridVerts.size(), 0);

	// draw two cubes
	WVP = cubeWorldMat * camView * camProjection;
	cbPerObj.WVP = XMMatrixTranspose(WVP);
	m_d3dImmediateContext->UpdateSubresource(cbPerObjectBuffer, 0, NULL, &cbPerObj, 0, 0);
	m_d3dImmediateContext->VSSetConstantBuffers(0, 1, &cbPerObjectBuffer);

	m_d3dImmediateContext->IASetVertexBuffers(0, 1, &m_circleVertexBuffer, &stride, &offset);
	m_d3dImmediateContext->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Send conterclockwise culling cube first!
	m_d3dImmediateContext->RSSetState(m_ccwCullingMode);
	m_d3dImmediateContext->DrawIndexed(36, 0, 0);

	// Send clockwise culling cube following the conter-colockwise culling cube!
	m_d3dImmediateContext->RSSetState(m_cwCullingMode);
	m_d3dImmediateContext->DrawIndexed(36, 0, 0);




	//Present the backbuffer to the screen
	HR(m_swapChain->Present(0, 0));
}

void LAB10::OnMouseDown(WPARAM _btnState, int _x, int _y) {
	m_lastMousePos.x = _x;
	m_lastMousePos.y = _y;

	SetCapture(window);
}

void LAB10::OnMouseUp(WPARAM _btnState, int _x, int _y) {
	ReleaseCapture();
}

void LAB10::OnMouseMove(WPARAM _btnState, int _x, int _y) {
	if ((MK_RBUTTON & _btnState) != 0) {

		camYaw +=  0.01f*(_x - m_lastMousePos.x);
		camPitch += 0.01f*(_y - m_lastMousePos.y);

		camPitch = Mathlib::Clamp(camPitch, -XM_PIDIV2 + 0.01f, XM_PIDIV2 - 0.01f);
	} 

	m_lastMousePos.x = _x;
	m_lastMousePos.y = _y;
}