#include "cshader.h"
#include "cviscraft.h"

CShader::CShader()
{
	m_renderer = nullptr;
	m_layout = nullptr;
	m_layoutShadow = nullptr;
	m_matrixBuffer = nullptr;
	m_pixelShader = nullptr;
	m_vertexShader = nullptr;
	m_pixelShadowShader = nullptr;
	m_vertexShadowShader = nullptr;
	m_lightBuffer = nullptr;
	m_sampleState = nullptr;

	for (int textureIndex = 0; textureIndex < TerrainTexture::Noof; ++textureIndex)
		m_texture[textureIndex] = nullptr;
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
	const char *const vsShadowFilename = "shaders/terrain_shadow.vs";
	const char *const psShadowFilename = "shaders/terrain_shadow.ps";

	ID3D10Blob *errorMessage = nullptr;
	
	// Compile the vertex shader code.
	ID3D10Blob *vertexShaderBuffer = nullptr;
	if (FAILED(D3DX11CompileFromFile(
		vsFilename, NULL, NULL, "TerrainVertexShader", "vs_4_0", 
		D3D10_SHADER_ENABLE_STRICTNESS, 0, NULL, 
		&vertexShaderBuffer, &errorMessage, NULL)))
	{
		char* compileErrors = (char*)(errorMessage->GetBufferPointer());
		return false;
	}

	// Compile the pixel shader code.
	ID3D10Blob *pixelShaderBuffer = nullptr;
	if (FAILED(D3DX11CompileFromFile(
		psFilename, NULL, NULL, "TerrainPixelShader", "ps_4_0", 
		D3D10_SHADER_ENABLE_STRICTNESS, 0, NULL, 
		&pixelShaderBuffer, &errorMessage, NULL)))
	{
		char* compileErrors = (char*)(errorMessage->GetBufferPointer());
		return false;
	}

	// Compile the vertex shader code.
	ID3D10Blob *vertexShadowShaderBuffer = nullptr;
	if (FAILED(D3DX11CompileFromFile(
		vsShadowFilename, NULL, NULL, "TerrainVertexShadowShader", "vs_4_0", 
		D3D10_SHADER_ENABLE_STRICTNESS, 0, NULL, 
		&vertexShadowShaderBuffer, &errorMessage, NULL)))
	{
		return false;
	}

	// Compile the pixel shader code.
	ID3D10Blob *pixelShadowShaderBuffer = nullptr;
	if (FAILED(D3DX11CompileFromFile(
		psShadowFilename, NULL, NULL, "TerrainPixelShadowShader", "ps_4_0", 
		D3D10_SHADER_ENABLE_STRICTNESS, 0, NULL, 
		&pixelShadowShaderBuffer, &errorMessage, NULL)))
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

	// Create the vertex shader from the buffer.
	if (FAILED(renderer->GetDevice()->CreateVertexShader(
		vertexShadowShaderBuffer->GetBufferPointer(), 
		vertexShadowShaderBuffer->GetBufferSize(), 
		NULL, &m_vertexShadowShader)))
	{
		return false;
	}

	// Create the pixel shader from the buffer.
	if (FAILED(renderer->GetDevice()->CreatePixelShader(
		pixelShadowShaderBuffer->GetBufferPointer(), 
		pixelShadowShaderBuffer->GetBufferSize(), 
		NULL, &m_pixelShadowShader)))
	{
		return false;
	}

	// Create the vertex input layout description.
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
	const unsigned int noofElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	// Create the vertex input layout.
	if (FAILED(renderer->GetDevice()->CreateInputLayout(
		polygonLayout, noofElements, 
		vertexShaderBuffer->GetBufferPointer(), 
		vertexShaderBuffer->GetBufferSize(), &m_layout)))
	{
		return false;
	}

	// Create the vertex input layout.
	if (FAILED(renderer->GetDevice()->CreateInputLayout(
		polygonLayout, noofElements, 
		vertexShadowShaderBuffer->GetBufferPointer(), 
		vertexShadowShaderBuffer->GetBufferSize(), &m_layoutShadow)))
	{
		return false;
	}

	// Release the vertex shader buffer and pixel shader buffer since they are no longer needed.
	SafeRelease(vertexShaderBuffer);
	SafeRelease(pixelShaderBuffer);
	SafeRelease(vertexShadowShaderBuffer);
	SafeRelease(pixelShadowShaderBuffer);

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

	// Load the terrain texture
	if (FAILED(D3DX11CreateShaderResourceViewFromFile(
			renderer->GetDevice(),
			"graphics/Terrain Textures/low.dds",
			NULL, NULL,
			&m_texture[TerrainTexture::Low],
			NULL
			)))
		return false;

	if (FAILED(D3DX11CreateShaderResourceViewFromFile(
		renderer->GetDevice(),
		"graphics/Terrain Textures/medium.dds",
		NULL, NULL,
		&m_texture[TerrainTexture::Medium],
		NULL
		)))
		return false;

	if (FAILED(D3DX11CreateShaderResourceViewFromFile(
		renderer->GetDevice(),
		"graphics/Terrain Textures/high.dds",
		NULL, NULL,
		&m_texture[TerrainTexture::High],
		NULL
		)))
		return false;

	m_light = new CLight();

	///////////////////////////////////////

	m_shadowbuffer = new CRenderTexture();
	m_shadowbuffer->Initialize(renderer->GetDevice(), 1024, 1024);

	return true;
}

void CShader::Release()
{
	for (int textureIndex = 0; textureIndex < TerrainTexture::Noof; ++textureIndex)
		SafeRelease(m_texture[textureIndex]);

	SafeRelease(m_vertexShader);
	SafeRelease(m_pixelShader);
	SafeDelete(m_light);
	m_shadowbuffer->Shutdown();
	SafeDelete(m_shadowbuffer);
}

const bool CShader::Render(
		int indexCount,						//!< 
		D3DXMATRIX world,					//!< 
		D3DXMATRIX view,					//!< 
		D3DXMATRIX projection				//!< 
	)
{
	D3DXMATRIX lightView, lightProjection, lightViewProjection;
	D3DXMatrixOrthoLH(&lightProjection, static_cast<float>(128 + 32), static_cast<float>(128 + 32), 10.0f, 256.0f);

	D3DXMatrixLookAtLH(&lightView, &(m_light->GetDirection() * -64.0f), &D3DXVECTOR3(64, 0, 64), &D3DXVECTOR3(0, 1, 0));

	D3DXMatrixMultiply(&lightViewProjection, &lightView, &lightProjection);

	m_shadowbuffer->SetRenderTarget(m_renderer->GetDeviceContext());
	m_shadowbuffer->ClearRenderTarget(m_renderer->GetDeviceContext(), 0.0f, 0.0f, 0.0f, 1.0f);

	if (!RenderShadowPass(indexCount, world, lightView, lightProjection))
		return false;

	m_renderer->SetBackBufferRenderTarget();
	m_renderer->ResetViewport();

	if (!RenderLightPass(indexCount, world, view, projection, lightViewProjection))
		return false;

	return true;
}

bool CShader::RenderShadowPass(
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

	// Set the vertex input layout.
	m_renderer->GetDeviceContext()->IASetInputLayout(m_layoutShadow);

	// Set the vertex and pixel shaders that will be used to render this triangle.
	m_renderer->GetDeviceContext()->VSSetShader(m_vertexShadowShader, NULL, 0);
	m_renderer->GetDeviceContext()->PSSetShader(m_pixelShadowShader, NULL, 0);

	// Set the sampler state in the pixel shader.
	m_renderer->GetDeviceContext()->PSSetSamplers(0, 1, &m_sampleState);

	// Render the triangle.
	m_renderer->GetDeviceContext()->DrawIndexed(indexCount, 0, 0);

	return true;
}

bool CShader::RenderLightPass(
		int indexCount,						//!< 
		D3DXMATRIX world,					//!< 
		D3DXMATRIX view,					//!< 
		D3DXMATRIX projection,				//!< 
		D3DXMATRIX lightViewProjection		//!< 
	)
{
	// Transpose the matrices to prepare them for the shader.
	D3DXMatrixTranspose(&world, &world);
	D3DXMatrixTranspose(&view, &view);
	D3DXMatrixTranspose(&projection, &projection);
	D3DXMatrixTranspose(&lightViewProjection, &lightViewProjection);

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
	dataPtr->lightViewProjection = lightViewProjection;

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
	lightDataPtr->ambientColor = m_light->GetAmbiant();
	lightDataPtr->diffuseColor = m_light->GetDiffuse();
	lightDataPtr->lightDirection = m_light->GetDirection();
	lightDataPtr->colorRender = CVisCraft::GetInstance()->GetTerrain()->GetFlag(TERRAIN_FLAG_COLORRENDER);

	// Unlock the constant buffer.
	m_renderer->GetDeviceContext()->Unmap(m_lightBuffer, 0);

	// Set the position of the light constant buffer in the pixel shader.
	bufferNumber = 0;

	// Set the light constant buffer in the pixel shader with the updated values.
	m_renderer->GetDeviceContext()->PSSetConstantBuffers(bufferNumber, 1, &m_lightBuffer);	

	// Set shader texture resource in the pixel shader.
	ID3D11ShaderResourceView *depthTexture = m_shadowbuffer->GetShaderResourceView();

	m_renderer->GetDeviceContext()->PSSetShaderResources(0, 1, &m_texture[TerrainTexture::Low]);
	m_renderer->GetDeviceContext()->PSSetShaderResources(1, 1, &m_texture[TerrainTexture::Medium]);
	m_renderer->GetDeviceContext()->PSSetShaderResources(2, 1, &m_texture[TerrainTexture::High]);
	m_renderer->GetDeviceContext()->PSSetShaderResources(3, 1, &depthTexture);

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
