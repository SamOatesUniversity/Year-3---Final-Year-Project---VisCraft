#include "cwater.h"

CWater::CWater()
{
	m_mesh = nullptr;

	m_vertexShader = nullptr;
	m_pixelShader = nullptr;
	m_layout = nullptr;
	m_matrixBuffer = nullptr;

	m_texture = nullptr;
	m_sampleState = nullptr;
}

CWater::~CWater()
{
	SafeDelete(m_mesh);
}

bool CWater::Create(
		CRenderer *renderer
	)
{
	m_mesh = new CMesh();
	if (!m_mesh->LoadMesh(renderer, "models/water/water.obj"))
	{
		SafeDelete(m_mesh);
		return false;
	}

	// Compile the vertex shader code.
	HRESULT result;
	ID3D10Blob* errorMessage = nullptr;
	ID3D10Blob* vertexShaderBuffer = nullptr;
	result = D3DX11CompileFromFile("shaders/water/water.vs", NULL, NULL, "WaterVertexShader", "vs_4_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, NULL, &vertexShaderBuffer, &errorMessage, NULL);
	if (FAILED(result))
		return false;

	// Compile the pixel shader code.
	ID3D10Blob* pixelShaderBuffer = nullptr;
	result = D3DX11CompileFromFile("shaders/water/water.ps", NULL, NULL, "WaterPixelShader", "ps_4_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, NULL, &pixelShaderBuffer, &errorMessage, NULL);
	if (FAILED(result))
		return false;

	// Create the vertex shader from the buffer.
	result = renderer->GetDevice()->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &m_vertexShader);
	if (FAILED(result))
		return false;

	// Create the pixel shader from the buffer.
	result = renderer->GetDevice()->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &m_pixelShader);
	if (FAILED(result))
		return false;

	// Now setup the layout of the data that goes into the shader.
	// This setup needs to match the VertexType stucture in the ModelClass and in the shader.
	D3D11_INPUT_ELEMENT_DESC polygonLayout[3];
	polygonLayout[0].SemanticName = "POSITION";
	polygonLayout[0].SemanticIndex = 0;
	polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[0].InputSlot = 0;
	polygonLayout[0].AlignedByteOffset = 0;
	polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[0].InstanceDataStepRate = 0;

	polygonLayout[1].SemanticName = "TEXCOORD";
	polygonLayout[1].SemanticIndex = 0;
	polygonLayout[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	polygonLayout[1].InputSlot = 0;
	polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[1].InstanceDataStepRate = 0;

	polygonLayout[2].SemanticName = "NORMAL";
	polygonLayout[2].SemanticIndex = 0;
	polygonLayout[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[2].InputSlot = 0;
	polygonLayout[2].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[2].InstanceDataStepRate = 0;

	// Get a count of the elements in the layout.
	unsigned int numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	// Create the vertex input layout.
	result = renderer->GetDevice()->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), &m_layout);
	if (FAILED(result))
		return false;

	// Release the vertex shader buffer and pixel shader buffer since they are no longer needed.
	SafeRelease(vertexShaderBuffer);
	SafeRelease(pixelShaderBuffer);

	// Setup the description of the dynamic matrix constant buffer that is in the vertex shader.
	D3D11_BUFFER_DESC matrixBufferDesc;
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(CWater::MatrixBuffer);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	// Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	result = renderer->GetDevice()->CreateBuffer(&matrixBufferDesc, NULL, &m_matrixBuffer);
	if (FAILED(result))
		return false;

	//// Load the terrain texture
	//if (FAILED(D3DX11CreateShaderResourceViewFromFile(
	//	renderer->GetDevice(),
	//	"skybox/texture.dds",
	//	NULL, NULL,
	//	&m_texture,
	//	NULL
	//)))
	//	return false;
	
	//// Create a texture sampler state description.
	//D3D11_SAMPLER_DESC samplerDesc;
	//samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	//samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	//samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	//samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	//samplerDesc.MipLODBias = 0.0f;
	//samplerDesc.MaxAnisotropy = 1;
	//samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	//samplerDesc.BorderColor[0] = 0;
	//samplerDesc.BorderColor[1] = 0;
	//samplerDesc.BorderColor[2] = 0;
	//samplerDesc.BorderColor[3] = 0;
	//samplerDesc.MinLOD = 0;
	//samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	//// Create the texture sampler state.
	//if (FAILED(renderer->GetDevice()->CreateSamplerState(&samplerDesc, &m_sampleState)))
	//	return false;

	return true;
}

void CWater::Render(
		CRenderer *renderer,
		D3DXMATRIX world,		//!< 
		D3DXMATRIX view,		//!< 
		D3DXMATRIX projection	//!< 
	)
{
	m_mesh->PrepareRender(renderer);

	D3DXMATRIX xformmat, scalemat;
	D3DXMatrixTranslation(&xformmat, 64, -1.0f, 64); 
	D3DXMatrixScaling(&scalemat, 0.9f, 1.0f, 0.9f);

	D3DXMatrixMultiply (&world, &scalemat, &xformmat);

	// Transpose the matrices to prepare them for the shader.
	D3DXMatrixTranspose(&world, &world);
	D3DXMatrixTranspose(&view, &view);
	D3DXMatrixTranspose(&projection, &projection);	

	// Lock the constant buffer so it can be written to.
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	result = renderer->GetDeviceContext()->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
		return;

	// Get a pointer to the data in the constant buffer.
	CWater::MatrixBuffer *const matrixDataPtr = (CWater::MatrixBuffer*)mappedResource.pData;

	// Copy the matrices into the constant buffer.
	matrixDataPtr->world = world;
	matrixDataPtr->view = view;
	matrixDataPtr->projection = projection;

	D3DXMATRIX invView;
	D3DXMatrixInverse(&invView, NULL, &view);
	matrixDataPtr->invViewMarix = invView;

	// Unlock the constant buffer.
	renderer->GetDeviceContext()->Unmap(m_matrixBuffer, 0);

	// Finally set the constant buffer in the vertex shader with the updated values.
	renderer->GetDeviceContext()->VSSetConstantBuffers(0, 1, &m_matrixBuffer);
	
	// Send the texture across
	//renderer->GetDeviceContext()->PSSetShaderResources(0, 1, &m_texture);

	// Set the vertex input layout.
	renderer->GetDeviceContext()->IASetInputLayout(m_layout);

	// Set the vertex and pixel shaders that will be used to render this triangle.
	renderer->GetDeviceContext()->VSSetShader(m_vertexShader, NULL, 0);
	renderer->GetDeviceContext()->PSSetShader(m_pixelShader, NULL, 0);

	// set the sampler state
	//renderer->GetDeviceContext()->PSSetSamplers(0, 1, &m_sampleState);

	renderer->EnableAlphaBlending(true);

	m_mesh->Draw(renderer);

	renderer->EnableAlphaBlending(false);
}