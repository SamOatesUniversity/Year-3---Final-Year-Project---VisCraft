#include "cshader.h"

CShader::CShader()
{
	m_renderer = nullptr;
	m_layout = nullptr;
	m_matrixBuffer = nullptr;
	m_pixelShader = nullptr;
	m_vertexShader = nullptr;
	m_lightBuffer = nullptr;
}

CShader::~CShader()
{

}

const bool CShader::Create( 
		CRenderer *renderer					//!< 
	)
{
	m_renderer = renderer;

	const char *const vsFilename = "shaders/terrain_diffuse.vs";
	const char *const psFilename = "shaders/terrain_diffuse.ps";

	ID3D10Blob *errorMessage = nullptr;
	ID3D10Blob *vertexShaderBuffer = nullptr;

	// Compile the vertex shader code.
	if (FAILED(D3DX11CompileFromFile(
		vsFilename, NULL, NULL, "TerrainVertexShader", "vs_4_0", 
		D3D10_SHADER_ENABLE_STRICTNESS, 0, NULL, 
		&vertexShaderBuffer, &errorMessage, NULL)))
	{
		return false;
	}

	// Compile the pixel shader code.
	ID3D10Blob *pixelShaderBuffer = nullptr;
	if (FAILED(D3DX11CompileFromFile(
		psFilename, NULL, NULL, "TerrainPixelShader", "ps_4_0", 
		D3D10_SHADER_ENABLE_STRICTNESS, 0, NULL, 
		&pixelShaderBuffer, &errorMessage, NULL)))
	{
		return false;
	}

	// Create the vertex shader from the buffer.
	if (FAILED(renderer->GetDevice()->CreateVertexShader(
			vertexShaderBuffer->GetBufferPointer(), 
			vertexShaderBuffer->GetBufferSize(), 
			NULL, &m_vertexShader)))
	{
		return false;
	}

	// Create the pixel shader from the buffer.
	if (FAILED(renderer->GetDevice()->CreatePixelShader(
		pixelShaderBuffer->GetBufferPointer(), 
		pixelShaderBuffer->GetBufferSize(), 
		NULL, &m_pixelShader)))
	{
		return false;
	}

	// Create the vertex input layout description.
	D3D11_INPUT_ELEMENT_DESC polygonLayout[2];
	polygonLayout[0].SemanticName = "POSITION";
	polygonLayout[0].SemanticIndex = 0;
	polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[0].InputSlot = 0;
	polygonLayout[0].AlignedByteOffset = 0;
	polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[0].InstanceDataStepRate = 0;

	polygonLayout[1].SemanticName = "NORMAL";
	polygonLayout[1].SemanticIndex = 0;
	polygonLayout[1].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[1].InputSlot = 0;
	polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[1].InstanceDataStepRate = 0;

	// Get a count of the elements in the layout.
	const unsigned int noofElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	// Create the vertex input layout.
	if (FAILED(renderer->GetDevice()->CreateInputLayout(
		polygonLayout, noofElements, 
		vertexShaderBuffer->GetBufferPointer(), 
		vertexShaderBuffer->GetBufferSize(), &m_layout)))
	{
		return false;
	}

	// Release the vertex shader buffer and pixel shader buffer since they are no longer needed.
	SafeRelease(vertexShaderBuffer);
	SafeRelease(pixelShaderBuffer);

	// Create a texture sampler state description.
	D3D11_SAMPLER_DESC samplerDesc;
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	// Create the texture sampler state.
	if (FAILED(renderer->GetDevice()->CreateSamplerState(&samplerDesc, &m_sampleState)))
		return false;

	// Setup the description of the dynamic matrix constant buffer that is in the vertex shader.
	D3D11_BUFFER_DESC matrixBufferDesc;
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBuffer);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	// Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	if (FAILED(renderer->GetDevice()->CreateBuffer(&matrixBufferDesc, NULL, &m_matrixBuffer)))
		return false;

	// Setup the description of the light dynamic constant buffer that is in the pixel shader.
	// Note that ByteWidth always needs to be a multiple of 16 if using D3D11_BIND_CONSTANT_BUFFER or CreateBuffer will fail.
	D3D11_BUFFER_DESC lightBufferDesc;
	lightBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	lightBufferDesc.ByteWidth = sizeof(LightBuffer);
	lightBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	lightBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	lightBufferDesc.MiscFlags = 0;
	lightBufferDesc.StructureByteStride = 0;

	// Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	if (FAILED(renderer->GetDevice()->CreateBuffer(&lightBufferDesc, NULL, &m_lightBuffer)))
		return false;

	return true;
}

void CShader::Release()
{

}

const bool CShader::Render(
		int indexCount,						//!< 
		D3DXMATRIX world,					//!< 
		D3DXMATRIX view,					//!< 
		D3DXMATRIX projection				//!< 
	)
{
	// Transpose the matrices to prepare them for the shader.
	D3DXMatrixTranspose(&world, &world);
	D3DXMatrixTranspose(&view, &view);
	D3DXMatrixTranspose(&projection, &projection);

	// Lock the constant buffer so it can be written to.
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	if (FAILED(m_renderer->GetDeviceContext()->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))
		return false;

	// Get a pointer to the data in the constant buffer.
	MatrixBuffer *const dataPtr = static_cast<MatrixBuffer*>(mappedResource.pData);

	// Copy the matrices into the constant buffer.
	dataPtr->world = world;
	dataPtr->view = view;
	dataPtr->projection = projection;

	// Unlock the constant buffer.
	m_renderer->GetDeviceContext()->Unmap(m_matrixBuffer, 0);

	// Set the position of the constant buffer in the vertex shader.
	unsigned int bufferNumber = 0;

	// Finally set the constant buffer in the vertex shader with the updated values.
	m_renderer->GetDeviceContext()->VSSetConstantBuffers(bufferNumber, 1, &m_matrixBuffer);

	// Lock the light constant buffer so it can be written to.
	if (FAILED(m_renderer->GetDeviceContext()->Map(m_lightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))
		return false;

	// Get a pointer to the data in the constant buffer.
	LightBuffer *const lightDataPtr = static_cast<LightBuffer*>(mappedResource.pData);

	// Copy the lighting variables into the constant buffer.
	lightDataPtr->ambientColor = D3DXVECTOR4(0.05f, 0.05f, 0.05f, 0.05f);
	lightDataPtr->diffuseColor = D3DXVECTOR4(0.5f, 0.5f, 0.5f, 1.0f);
	lightDataPtr->lightDirection = D3DXVECTOR3(0.0f, -0.5f, 0.75f);
	lightDataPtr->__padding__ = 0.0f;

	// Unlock the constant buffer.
	m_renderer->GetDeviceContext()->Unmap(m_lightBuffer, 0);

	// Set the position of the light constant buffer in the pixel shader.
	bufferNumber = 0;

	// Finally set the light constant buffer in the pixel shader with the updated values.
	m_renderer->GetDeviceContext()->PSSetConstantBuffers(bufferNumber, 1, &m_lightBuffer);	

	// Set the vertex input layout.
	m_renderer->GetDeviceContext()->IASetInputLayout(m_layout);

	// Set the vertex and pixel shaders that will be used to render this triangle.
	m_renderer->GetDeviceContext()->VSSetShader(m_vertexShader, NULL, 0);
	m_renderer->GetDeviceContext()->PSSetShader(m_pixelShader, NULL, 0);

	// Set the sampler state in the pixel shader.
	m_renderer->GetDeviceContext()->PSSetSamplers(0, 1, &m_sampleState);

	// Render the triangle.
	m_renderer->GetDeviceContext()->DrawIndexed(indexCount, 0, 0);

	return true;
}
