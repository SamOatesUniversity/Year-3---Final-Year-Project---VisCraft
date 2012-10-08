#include "cshader.h"

CShader::CShader()
{
	m_renderer = nullptr;
}

CShader::~CShader()
{

}

const bool CShader::Create( 
		CRenderer *renderer					//!< 
	)
{
	m_renderer = renderer;

	const char *const vsFilename = "terrain.vs";
	const char *const psFilename = "terrain.ps";

	ID3D10Blob *errorMessage = nullptr;
	ID3D10Blob *vertexShaderBuffer = nullptr;

	// Compile the vertex shader code.
	if (FAILED(D3DX11CompileFromFile(
		vsFilename, NULL, NULL, "ColorVertexShader", "vs_5_0", 
		D3D10_SHADER_ENABLE_STRICTNESS, 0, NULL, 
		&vertexShaderBuffer, &errorMessage, NULL)))
	{
		return false;
	}

	// Compile the pixel shader code.
	ID3D10Blob *pixelShaderBuffer = nullptr;
	if (FAILED(D3DX11CompileFromFile(
		psFilename, NULL, NULL, "ColorPixelShader", "ps_5_0", 
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
	// This setup needs to match the VertexType stucture in the ModelClass and in the shader.
	D3D11_INPUT_ELEMENT_DESC polygonLayout[2];
	polygonLayout[0].SemanticName = "POSITION";
	polygonLayout[0].SemanticIndex = 0;
	polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[0].InputSlot = 0;
	polygonLayout[0].AlignedByteOffset = 0;
	polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[0].InstanceDataStepRate = 0;

	polygonLayout[1].SemanticName = "COLOR";
	polygonLayout[1].SemanticIndex = 0;
	polygonLayout[1].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
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

	// Set the vertex input layout.
	m_renderer->GetDeviceContext()->IASetInputLayout(m_layout);

	// Set the vertex and pixel shaders that will be used to render this triangle.
	m_renderer->GetDeviceContext()->VSSetShader(m_vertexShader, NULL, 0);
	m_renderer->GetDeviceContext()->PSSetShader(m_pixelShader, NULL, 0);

	// Render the triangle.
	m_renderer->GetDeviceContext()->DrawIndexed(indexCount, 0, 0);

	return true;
}
