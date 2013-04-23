#include "CBitmap.h"

CBitmap::CBitmap()
{
	m_vertexBuffer = nullptr;
	m_indexBuffer = nullptr;
	m_texture = nullptr;
}

CBitmap::~CBitmap()
{

}

const bool CBitmap::Create( 
		ID3D11Device *device, 
		int screenWidth, 
		int screenHeight, 
		char *filePath, 
		int width, 
		int height
	)
{
	// Store the screen size.
	m_screenWidth = screenWidth;
	m_screenHeight = screenHeight;

	// Store the size in pixels that this bitmap should be rendered at.
	m_bitmapWidth = width;
	m_bitmapHeight = height;

	// Initialize the previous rendering position to negative one.
	m_previousPosX = -1;
	m_previousPosY = -1;

	// Initialize the vertex and index buffers.
	const bool createResult = CreateBuffers(device);
	if(!createResult)
	{
		return false;
	}

	// Load the texture for this bitmap.
	const bool loadResult = LoadTexture(device, filePath);
	if(!loadResult)
	{
		return false;
	}

	return true;
}

void CBitmap::Destroy()
{
	// Release the bitmap texture.
	ReleaseTexture();

	// Shutdown the vertex and index buffers.
	DestroyBuffers();
}

const bool CBitmap::Render( 
	ID3D11DeviceContext *deviceContext, 
	int x, 
	int y,
	int w,
	int h
	)
{
	if (w == -1) w = m_bitmapWidth;
	if (h == -1) h = m_bitmapHeight;

	// Re-build the dynamic vertex buffer for rendering to possibly a different location on the screen.
	const bool updateResult = UpdateBuffers(deviceContext, x, y, w, h);
	if(!updateResult)
	{
		return false;
	}

	// Put the vertex and index buffers on the graphics pipeline to prepare them for drawing.
	RenderBuffers(deviceContext);

	return true;
}

const int CBitmap::GetIndexCount() const
{
	return m_indexCount;
}

ID3D11ShaderResourceView *CBitmap::GetTexture() const
{
	return m_texture->GetTexture();
}

const bool CBitmap::CreateBuffers( 
		ID3D11Device *device
	)
{
	// Set the number of vertices in the vertex array.
	m_vertexCount = 6;

	// Set the number of indices in the index array.
	m_indexCount = m_vertexCount;

	// Create the vertex array.
	VertexType *const vertices = new VertexType[m_vertexCount];
	if(!vertices)
	{
		return false;
	}

	// Create the index array.
	unsigned long *const indices = new unsigned long[m_indexCount];
	if(!indices)
	{
		return false;
	}

	// Initialize vertex array to zeros at first.
	memset(vertices, 0, (sizeof(VertexType) * m_vertexCount));

	// Load the index array with data.
	for(int index = 0; index < m_indexCount; ++index)
	{
		indices[index] = index;
	}

	// Set up the description of the static vertex buffer.
	D3D11_BUFFER_DESC vertexBufferDesc;
	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the vertex data.
	D3D11_SUBRESOURCE_DATA vertexData;
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// Now create the vertex buffer.
	const HRESULT createVertexBufferResult = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
	if (FAILED(createVertexBufferResult))
	{
		return false;
	}

	// Set up the description of the static index buffer.
	D3D11_BUFFER_DESC indexBufferDesc;
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the index data.
	D3D11_SUBRESOURCE_DATA indexData;
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// Create the index buffer.
	const HRESULT createIndexBufferResult = device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer);
	if (FAILED(createIndexBufferResult))
	{
		return false;
	}

	// Release the arrays now that the vertex and index buffers have been created and loaded.
	delete [] vertices;
	delete [] indices;

	return true;
}

void CBitmap::DestroyBuffers()
{
	// Release the index buffer.
	if(m_indexBuffer)
	{
		m_indexBuffer->Release();
		m_indexBuffer = nullptr;
	}

	// Release the vertex buffer.
	if(m_vertexBuffer)
	{
		m_vertexBuffer->Release();
		m_vertexBuffer = nullptr;
	}
}

bool CBitmap::UpdateBuffers( 
		ID3D11DeviceContext *deviceContext, 
		int x, 
		int y,
		int w,
		int h
	)
{
	// If the position we are rendering this bitmap to has not changed then don't update the vertex buffer since it
	// currently has the correct parameters.
	if ((x == m_previousPosX) && (y == m_previousPosY))
	{
		return true;
	}

	// If it has changed then update the position it is being rendered to.
	m_previousPosX = x;
	m_previousPosY = y;

	// Calculate the screen coordinates of the left side of the bitmap.
	float left = static_cast<float>((m_screenWidth / 2) * -1) + static_cast<float>(x);

	// Calculate the screen coordinates of the right side of the bitmap.
	float right = left + static_cast<float>(w);

	// Calculate the screen coordinates of the top of the bitmap.
	float top = static_cast<float>(m_screenHeight / 2) - static_cast<float>(y);

	// Calculate the screen coordinates of the bottom of the bitmap.
	float bottom = top - static_cast<float>(h);

	// Create the vertex array.
	VertexType *const vertices = new VertexType[m_vertexCount];
	if (!vertices)
	{
		return false;
	}

	// Load the vertex array with data.
	// First triangle.
	vertices[0].position = D3DXVECTOR3(left, top, 0.0f);  // Top left.
	vertices[0].texture = D3DXVECTOR2(0.0f, 0.0f);

	vertices[1].position = D3DXVECTOR3(right, bottom, 0.0f);  // Bottom right.
	vertices[1].texture = D3DXVECTOR2(1.0f, 1.0f);

	vertices[2].position = D3DXVECTOR3(left, bottom, 0.0f);  // Bottom left.
	vertices[2].texture = D3DXVECTOR2(0.0f, 1.0f);

	// Second triangle.
	vertices[3].position = D3DXVECTOR3(left, top, 0.0f);  // Top left.
	vertices[3].texture = D3DXVECTOR2(0.0f, 0.0f);

	vertices[4].position = D3DXVECTOR3(right, top, 0.0f);  // Top right.
	vertices[4].texture = D3DXVECTOR2(1.0f, 0.0f);

	vertices[5].position = D3DXVECTOR3(right, bottom, 0.0f);  // Bottom right.
	vertices[5].texture = D3DXVECTOR2(1.0f, 1.0f);

	// Lock the vertex buffer so it can be written to.
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	const HRESULT mapResult = deviceContext->Map(m_vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(mapResult))
	{
		return false;
	}

	// Get a pointer to the data in the vertex buffer.
	VertexType *const verticesPtr = static_cast<VertexType*>(mappedResource.pData);

	// Copy the data into the vertex buffer.
	memcpy(verticesPtr, (void*)vertices, (sizeof(VertexType) * m_vertexCount));

	// Unlock the vertex buffer.
	deviceContext->Unmap(m_vertexBuffer, 0);

	// Release the vertex array as it is no longer needed.
	delete [] vertices;

	return true;
}

void CBitmap::RenderBuffers( 
		ID3D11DeviceContext *deviceContext 
	)
{
	// Set vertex buffer stride and offset.
	unsigned int stride = sizeof(VertexType); 
	unsigned int offset = 0;

	// Set the vertex buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

	// Set the index buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

bool CBitmap::LoadTexture( 
		ID3D11Device *device, 
		char *fileLocation 
	)
{
	// Create the texture object.
	m_texture = new CTexture();
	if (!m_texture)
	{
		return false;
	}

	// Initialize the texture object.
	const bool textureCreateResult = m_texture->Create(device, fileLocation);
	if (!textureCreateResult)
	{
		return false;
	}

	return true;
}

void CBitmap::ReleaseTexture()
{
	// Release the texture object.
	if (m_texture)
	{
		m_texture->Destroy();
		delete m_texture;
		m_texture = 0;
	}
}

const int CBitmap::GetWidth() const
{
	return m_bitmapWidth;
}

const int CBitmap::GetHeight() const
{
	return m_bitmapHeight;
}

D3DXVECTOR2 CBitmap::GetPosition() const
{
	return D3DXVECTOR2(static_cast<FLOAT>(m_previousPosX), static_cast<FLOAT>(m_previousPosY));
}
