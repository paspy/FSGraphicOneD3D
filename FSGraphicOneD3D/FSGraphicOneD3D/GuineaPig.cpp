#include "GuineaPig.h"
#include "Mathlib.h"
// shader byte code
#include "Trivial_VS.csh"
#include "Trivial_PS.csh"

// texture header file
#include "Resource/numbers_test.h"
#include "Resource/grass_diffuse.h"
//#include "Resource/grass_normal.h"

#include <iostream>

GuineaPig::GuineaPig(HINSTANCE hinst) : D3DApp(hinst),
	m_cubeVertexBuffer(nullptr),
	m_groundVertexBuffer(nullptr),
	m_inputLayout(nullptr),
	m_vertexShader(nullptr),
	m_pixelShader(nullptr) {

}

GuineaPig::~GuineaPig() {
	// release geometries ptr
	ReleaseCOM(m_cubeVertexBuffer);
	ReleaseCOM(m_cubeIndexBuffer);
	ReleaseCOM(m_groundVertexBuffer);

	// release shader ptr
	ReleaseCOM(m_vertexShader);
	ReleaseCOM(m_pixelShader);

	// release layout ptr
	ReleaseCOM(m_inputLayout);

	// release constant buffer ptr
	ReleaseCOM(m_cbCubeBuffer);
	ReleaseCOM(m_cbGroundBuffer);

	// release texture ptr
	ReleaseCOM(m_cubeShaderResView);
	ReleaseCOM(m_cubeTexture2D);
	ReleaseCOM(m_cubeTexSamplerState);

	ReleaseCOM(m_grassShaderResView);
	ReleaseCOM(m_grassTexture2D);
	ReleaseCOM(m_grassTexSamplerState);

	// release render state ptr
	ReleaseCOM(m_antialiasedLine);
	ReleaseCOM(m_blendTransparency);
	ReleaseCOM(m_cwCullingMode);
	ReleaseCOM(m_ccwCullingMode);
}

bool GuineaPig::Init() {
	if (!D3DApp::Init())
		return false;

	BuildObjConstBuffer();
	BuildGeometryBuffers(); 
	BuildTextureAndState();
	BuildVertexLayout();
	BuildShader();
	BuildRenderStates();

	return true;
}

void GuineaPig::OnResize() {
	D3DApp::OnResize();

}

void GuineaPig::BuildObjConstBuffer() {
	D3D11_BUFFER_DESC cbbd;
	ZeroMemory(&cbbd, sizeof(D3D11_BUFFER_DESC));

	cbbd.Usage = D3D11_USAGE_DEFAULT;
	cbbd.ByteWidth = sizeof(ConstPerObject);
	cbbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbbd.CPUAccessFlags = 0;
	cbbd.MiscFlags = 0;

	HR(m_d3dDevice->CreateBuffer(&cbbd, NULL, &m_cbCubeBuffer));

	HR(m_d3dDevice->CreateBuffer(&cbbd, NULL, &m_cbGroundBuffer));
	m_cbGroundObject.texIndex = 0;
}

void GuineaPig::BuildGeometryBuffers() {
	
	// Clockwise
	Vertex3D cubeVerteces[] = {
		// Front Face
		Vertex3D(XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT2(0.0f,  1.0f)),	// A
		Vertex3D(XMFLOAT3(-1.0f,  1.0f, -1.0f), XMFLOAT2(0.0f,  0.0f)),	// B
		Vertex3D(XMFLOAT3(+1.0f,  1.0f, -1.0f), XMFLOAT2(0.25f, 0.0f)),	// C
		Vertex3D(XMFLOAT3(+1.0f, -1.0f, -1.0f), XMFLOAT2(0.25f, 1.0f)),	// D

		// Back Face
		Vertex3D(XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT2(0.25f, 1.0f)),		
		Vertex3D(XMFLOAT3(+1.0f, -1.0f, 1.0f), XMFLOAT2(0.0f,  1.0f)),
		Vertex3D(XMFLOAT3(+1.0f,  1.0f, 1.0f), XMFLOAT2(0.0f,  0.0f)),
		Vertex3D(XMFLOAT3(-1.0f,  1.0f, 1.0f), XMFLOAT2(0.25f, 0.0f)),

		// Top Face
		Vertex3D(XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT2(0.0f,  1.0f)),
		Vertex3D(XMFLOAT3(-1.0f, 1.0f,  1.0f), XMFLOAT2(0.0f,  0.0f)),
		Vertex3D(XMFLOAT3(+1.0f, 1.0f,  1.0f), XMFLOAT2(0.25f, 0.0f)),
		Vertex3D(XMFLOAT3(+1.0f, 1.0f, -1.0f), XMFLOAT2(0.25f, 1.0f)),

		// Bottom Face
		Vertex3D(XMFLOAT3(-1.0f, -1.0f, -1.0f),XMFLOAT2(0.25f, 1.0f)),
		Vertex3D(XMFLOAT3(+1.0f, -1.0f, -1.0f),XMFLOAT2(0.0f,  1.0f)),
		Vertex3D(XMFLOAT3(+1.0f, -1.0f,  1.0f),XMFLOAT2(0.0f,  0.0f)),
		Vertex3D(XMFLOAT3(-1.0f, -1.0f,  1.0f),XMFLOAT2(0.25f, 0.0f)),

		// Left Face
		Vertex3D(XMFLOAT3(-1.0f, -1.0f,  1.0f), XMFLOAT2(0.0f,  1.0f)),
		Vertex3D(XMFLOAT3(-1.0f,  1.0f,  1.0f), XMFLOAT2(0.0f,  0.0f)),
		Vertex3D(XMFLOAT3(-1.0f,  1.0f, -1.0f), XMFLOAT2(0.25f, 0.0f)),
		Vertex3D(XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT2(0.25f, 1.0f)),

		// Right Face
		Vertex3D(XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT2(0.0f,  1.0f)),
		Vertex3D(XMFLOAT3(1.0f,  1.0f, -1.0f), XMFLOAT2(0.0f,  0.0f)),
		Vertex3D(XMFLOAT3(1.0f,  1.0f,  1.0f), XMFLOAT2(0.25f, 0.0f)),
		Vertex3D(XMFLOAT3(1.0f, -1.0f,  1.0f), XMFLOAT2(0.25f, 1.0f)),
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
	ZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc));

	vertexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = NULL;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.ByteWidth = static_cast<UINT>(sizeof(Vertex3D) * 24);

	D3D11_SUBRESOURCE_DATA vinitData;
	//vinitData.pSysMem = m_vertices.data();
	vinitData.pSysMem = cubeVerteces;

	// Create Verteces Buffer
	HR(m_d3dDevice->CreateBuffer(&vertexBufferDesc, &vinitData, &m_cubeVertexBuffer));
	// Set verteces buffer	- if changed over time, need to recall every frame
	//UINT stride = sizeof(Vertex3D), offset = 0;
	//m_d3dImmediateContext->IASetVertexBuffers(0, 1, &m_cubeVertexBuffer, &stride, &offset);
	
	BuildGroundBuffers();
}

void GuineaPig::BuildGroundBuffers() {

	//Create the vertex buffer
	Vertex3D groundVerts[] = {
		Vertex3D(XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT2( 100.0f, 100.0f)),
		Vertex3D(XMFLOAT3(+1.0f, -1.0f, -1.0f), XMFLOAT2(   0.0f, 100.0f)),
		Vertex3D(XMFLOAT3(+1.0f, -1.0f, +1.0f), XMFLOAT2(   0.0f,   0.0f)),
		Vertex3D(XMFLOAT3(-1.0f, -1.0f, +1.0f), XMFLOAT2( 100.0f,   0.0f)),
	};

	DWORD gourndIndices[] = {
		0,  1,  2,
		0,  2,  3,
	};

	D3D11_BUFFER_DESC indexBufferDesc;
	ZeroMemory(&indexBufferDesc, sizeof(indexBufferDesc));

	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.ByteWidth = sizeof(DWORD) * 2 * 3;

	D3D11_SUBRESOURCE_DATA iinitData;
	iinitData.pSysMem = gourndIndices;
	// Create Index Buffer
	HR(m_d3dDevice->CreateBuffer(&indexBufferDesc, &iinitData, &m_groundIndexBuffer));
	

	D3D11_BUFFER_DESC vertexBufferDesc;
	ZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc));

	vertexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = NULL;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.ByteWidth = static_cast<UINT>(sizeof(Vertex3D) * 4);

	D3D11_SUBRESOURCE_DATA vinitData;
	vinitData.pSysMem = groundVerts;

	// Create Verteces Buffer
	HR(m_d3dDevice->CreateBuffer(&vertexBufferDesc, &vinitData, &m_groundVertexBuffer));

}

void GuineaPig::BuildTextureAndState() {

	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResViewDesc;
	ZeroMemory(&shaderResViewDesc, sizeof(shaderResViewDesc));
	shaderResViewDesc.ViewDimension = D3D_SRV_DIMENSION_TEXTURE2D;
	shaderResViewDesc.Texture2D.MipLevels = numbers_test_numlevels;

	D3D11_TEXTURE2D_DESC textureBufferDesc;
	ZeroMemory(&textureBufferDesc, sizeof(textureBufferDesc));
	textureBufferDesc.Width = numbers_test_width;
	textureBufferDesc.Height = numbers_test_height;
	textureBufferDesc.MipLevels = numbers_test_numlevels;
	textureBufferDesc.ArraySize = 1;
	textureBufferDesc.SampleDesc.Count = 1;
	textureBufferDesc.SampleDesc.Quality = 0;
	textureBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	textureBufferDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	textureBufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	textureBufferDesc.CPUAccessFlags = 0;
	textureBufferDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA texInitData[numbers_test_numlevels];
	for (int i = 0; i < numbers_test_numlevels; i++) {
		texInitData[i].pSysMem = &numbers_test_pixels[numbers_test_leveloffsets[i]];
		texInitData[i].SysMemPitch = (numbers_test_width >> i)*sizeof(unsigned int);
	}

	// create texture
	HR(m_d3dDevice->CreateTexture2D(&textureBufferDesc, texInitData, &m_cubeTexture2D));

	// create shader resource view from texture
	HR(m_d3dDevice->CreateShaderResourceView(m_cubeTexture2D, &shaderResViewDesc, &m_cubeShaderResView));

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
	HR(m_d3dDevice->CreateSamplerState(&sampDesc, &m_cubeTexSamplerState));


	// create grass texture

	ZeroMemory(&shaderResViewDesc, sizeof(shaderResViewDesc));
	shaderResViewDesc.ViewDimension = D3D_SRV_DIMENSION_TEXTURE2D;
	shaderResViewDesc.Texture2D.MipLevels = grass_diffuse_numlevels;

	ZeroMemory(&textureBufferDesc, sizeof(textureBufferDesc));
	textureBufferDesc.Width = grass_diffuse_width;
	textureBufferDesc.Height = grass_diffuse_height;
	textureBufferDesc.MipLevels = grass_diffuse_numlevels;
	textureBufferDesc.ArraySize = 1;
	textureBufferDesc.SampleDesc.Count = 1;
	textureBufferDesc.SampleDesc.Quality = 0;
	textureBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	textureBufferDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	textureBufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	textureBufferDesc.CPUAccessFlags = 0;
	textureBufferDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA grassInitData[grass_diffuse_numlevels];
	for (int i = 0; i < grass_diffuse_numlevels; i++) {
		grassInitData[i].pSysMem = &grass_diffuse_pixels[grass_diffuse_leveloffsets[i]];
		grassInitData[i].SysMemPitch = (grass_diffuse_width >> i)*sizeof(unsigned int);
	}

	// create texture
	HR(m_d3dDevice->CreateTexture2D(&textureBufferDesc, grassInitData, &m_grassTexture2D));

	// create shader resource view from texture
	HR(m_d3dDevice->CreateShaderResourceView(m_grassTexture2D, &shaderResViewDesc, &m_grassShaderResView));

	// Describe the Sample State
	sampDesc;
	ZeroMemory(&sampDesc, sizeof(sampDesc));

	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;

	//Create the Sample State
	HR(m_d3dDevice->CreateSamplerState(&sampDesc, &m_grassTexSamplerState));

}

void GuineaPig::BuildShader() {
	HR(m_d3dDevice->CreateVertexShader(Trivial_VS, sizeof(Trivial_VS), NULL, &m_vertexShader));
	m_d3dImmediateContext->VSSetShader(m_vertexShader, NULL, 0);

	HR(m_d3dDevice->CreatePixelShader(Trivial_PS, sizeof(Trivial_PS), NULL, &m_pixelShader));
	m_d3dImmediateContext->PSSetShader(m_pixelShader, NULL, 0);

}

void GuineaPig::BuildVertexLayout() {

	D3D11_INPUT_ELEMENT_DESC vertLayout[] = {
		{ "POSITION",	0, DXGI_FORMAT_R32G32B32_FLOAT,	0, D3D11_APPEND_ALIGNED_ELEMENT,	D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR",		0, DXGI_FORMAT_R32G32B32A32_FLOAT,	0, D3D11_APPEND_ALIGNED_ELEMENT,	D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD",	0, DXGI_FORMAT_R32G32_FLOAT,	0, D3D11_APPEND_ALIGNED_ELEMENT,	D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	// Create the input layout
	HR(m_d3dDevice->CreateInputLayout(vertLayout, 3, Trivial_VS, sizeof(Trivial_VS), &m_inputLayout));
	m_d3dImmediateContext->IASetInputLayout(m_inputLayout);

}

void GuineaPig::BuildRenderStates() {

	// Raster Description	
	D3D11_RASTERIZER_DESC rasterDesc;
	ZeroMemory(&rasterDesc, sizeof(D3D11_RASTERIZER_DESC));

	rasterDesc.FillMode = D3D11_FILL_SOLID;
	rasterDesc.CullMode = D3D11_CULL_NONE;
	rasterDesc.AntialiasedLineEnable = true;

	HR(m_d3dDevice->CreateRasterizerState(&rasterDesc, &m_antialiasedLine));

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

void GuineaPig::UpdateScene(double _dt) {

	// Update objects
	static double texIdx = 0;
	static float rot = 0.00f;

	rot += (float)_dt;

	if (rot > 6.26f) rot = 0.0f;

	texIdx += _dt;

	m_cbCubeObject.texIndex = (int)texIdx;

	if ((int)texIdx > 3) texIdx = 0;

	groundWorldMat = XMMatrixIdentity();
	groundWorldMat = XMMatrixScaling(500.0f, 2.0f, 500.0f);

	cubeWorldMat = XMMatrixIdentity();
	XMVECTOR rotAxis_X = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
	XMVECTOR rotAxis_Y = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	XMVECTOR rotAxis_Z = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);

	XMMATRIX rotationMat =  XMMatrixRotationAxis(rotAxis_X, rot) * XMMatrixRotationAxis(rotAxis_Y, rot);
	XMMATRIX translationMat = XMMatrixTranslation(0.0f, 0.0f, 1.0f);

	cubeWorldMat = rotationMat * translationMat;
}

void GuineaPig::DrawScene() {
	assert(m_d3dImmediateContext);
	assert(m_swapChain);

	//Refresh the render target view
	m_d3dImmediateContext->ClearRenderTargetView(m_renderTargetView, reinterpret_cast<const float*>(&Colors::Black));
	
	//Refresh the Depth/Stencil view
	m_d3dImmediateContext->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

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

	//objPos = XMVector3TransformCoord(objPos, groundWorldMat);

	//distX = XMVectorGetX(objPos) - XMVectorGetX(camPosition);
	//distY = XMVectorGetY(objPos) - XMVectorGetY(camPosition);
	//distZ = XMVectorGetZ(objPos) - XMVectorGetZ(camPosition);

	//float gridDist = distX*distX + distY*distY + distZ*distZ;

	UINT stride = sizeof(Vertex3D), offset = 0;

	// Set Shader Resources and Samplers
	m_d3dImmediateContext->PSSetShaderResources(0, 1, &m_grassShaderResView);
	m_d3dImmediateContext->PSSetSamplers(0, 1, &m_grassTexSamplerState);

	// Draw Ground
	m_cbGroundObject.WVP = XMMatrixTranspose(groundWorldMat * m_camView * m_camProjection);
	m_d3dImmediateContext->UpdateSubresource(m_cbGroundBuffer, 0, NULL, &m_cbGroundObject, 0, 0);
	m_d3dImmediateContext->VSSetConstantBuffers(0, 1, &m_cbGroundBuffer);
	// Set verts buffer
	m_d3dImmediateContext->IASetVertexBuffers(0, 1, &m_groundVertexBuffer, &stride, &offset);
	// Set indeces buffer
	m_d3dImmediateContext->IASetIndexBuffer(m_groundIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
	m_d3dImmediateContext->RSSetState(m_antialiasedLine);
	m_d3dImmediateContext->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m_d3dImmediateContext->DrawIndexed(6, 0, 0);

	// Set Shader Resources and Samplers
	m_d3dImmediateContext->PSSetShaderResources(0, 1, &m_cubeShaderResView);
	m_d3dImmediateContext->PSSetSamplers(0, 1, &m_cubeTexSamplerState);

	// draw two cubes
	m_cbCubeObject.WVP = XMMatrixTranspose(cubeWorldMat * m_camView * m_camProjection);
	m_d3dImmediateContext->UpdateSubresource(m_cbCubeBuffer, 0, NULL, &m_cbCubeObject, 0, 0);
	m_d3dImmediateContext->VSSetConstantBuffers(0, 1, &m_cbCubeBuffer);
	// Set verts buffer
	m_d3dImmediateContext->IASetVertexBuffers(0, 1, &m_cubeVertexBuffer, &stride, &offset);
	// Set indeces buffer
	m_d3dImmediateContext->IASetIndexBuffer(m_cubeIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
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

void GuineaPig::UpdateKeyboardInput(double _dt) {

	if (GetAsyncKeyState(VK_LW)) {
		m_moveBackForward += (float)_dt * 10.0f;
	}

	if (GetAsyncKeyState(VK_LS)) {
		m_moveBackForward -= (float)_dt * 10.0f;
	}

	if (GetAsyncKeyState(VK_LA)) {
		m_moveLeftRight -= (float)_dt * 10.0f;
	}

	if (GetAsyncKeyState(VK_LD)) {
		m_moveLeftRight += (float)_dt * 10.0f;
	}

}

void GuineaPig::UpdateCamera() {
	D3DApp::UpdateCamera();
}

void GuineaPig::OnMouseDown(WPARAM _btnState, int _x, int _y) {
	m_lastMousePos.x = _x;
	m_lastMousePos.y = _y;

	SetCapture(window);
}

void GuineaPig::OnMouseUp(WPARAM _btnState, int _x, int _y) {
	ReleaseCapture();
}

void GuineaPig::OnMouseMove(WPARAM _btnState, int _x, int _y) {
	if ((MK_RBUTTON & _btnState) != 0) {

		m_camYaw +=  0.01f*(_x - m_lastMousePos.x);
		m_camPitch += 0.01f*(_y - m_lastMousePos.y);

		m_camPitch = Mathlib::Clamp(m_camPitch, -XM_PIDIV2 + 0.01f, XM_PIDIV2 - 0.01f);
	} 

	m_lastMousePos.x = _x;
	m_lastMousePos.y = _y;
}