#include "cterrain.h"

/*
 *	\brief Class constructor
*/
CTerrain::CTerrain()
{
	m_renderer = nullptr;
	m_vertexBuffer = nullptr;
	m_indexBuffer = nullptr;

	m_size = D3DXVECTOR2(256, 256);
}

/*
 *	\brief Class destructor
*/
CTerrain::~CTerrain()
{

}

/*
 *	\brief Create a flat terrain
*/
bool CTerrain::Create( 
		CRenderer *renderer 
	)
{
	m_renderer = renderer;

	if (!InitializeBuffers())
		return false;

	return true;
}

/*
 *	\brief Setup the terrain buffers to a default state
*/
const bool CTerrain::InitializeBuffers()
{
	m_indexCount = m_vertexCount = (static_cast<unsigned int>(m_size.x) - 1) * (static_cast<unsigned int>(m_size.y) - 1) * 8;

	VertexType *const vertices = new VertexType[m_vertexCount];

	unsigned long *const indices = new unsigned long[m_indexCount];

	unsigned int index = 0;

	for (int z = 0; z < (m_size.y - 1); ++z)
	{
		for(int x = 0; x < (m_size.x - 1); ++x)
		{
			AddVertex(vertices, indices, x, z + 1, index);
			AddVertex(vertices, indices, x + 1, z + 1, index);

			AddVertex(vertices, indices, x + 1, z + 1, index);
			AddVertex(vertices, indices, x + 1, z, index);

			AddVertex(vertices, indices, x + 1, z, index);
			AddVertex(vertices, indices, x, z, index);

			AddVertex(vertices, indices, x, z, index);
			AddVertex(vertices, indices, x, z + 1, index);
		}
	}

	// Set up the description of the static vertex buffer.
	D3D11_BUFFER_DESC vertexBufferDesc;
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the vertex data.
	D3D11_SUBRESOURCE_DATA vertexData;
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// Now create the vertex buffer.
	if (FAILED(m_renderer->GetDevice()->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer)))
		return false;

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
	if (FAILED(m_renderer->GetDevice()->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer)))
		return false;

	// Release the arrays now that the buffers have been created and loaded.
	delete vertices;
	delete indices;

	return true;
}

/*
 *	\brief Add a vertex at a given x,z coordinate
*/
void CTerrain::AddVertex( 
		VertexType *vertices,
		unsigned long *indices,
		const int x, 
		const int z, 
		unsigned int &index 
	)
{
	vertices[index].position = D3DXVECTOR3(static_cast<float>(x), 0.0f, static_cast<float>(z));
	vertices[index].color = D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f);
	indices[index] = index;
	index++;
}

/*
 *	\brief Release any resources allocated by the terrain class
*/
void CTerrain::Release()
{
	SafeRelease(m_indexBuffer);
	SafeRelease(m_vertexBuffer);
}

/*
 *	\brief Update the terrain for the D3D11 renderer
*/
void CTerrain::Update()
{
	// Set vertex buffer stride and offset.
	unsigned int stride = sizeof(VertexType); 
	unsigned int offset = 0;

	// Set the vertex buffer to active in the input assembler so it can be rendered.
	m_renderer->GetDeviceContext()->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

	// Set the index buffer to active in the input assembler so it can be rendered.
	m_renderer->GetDeviceContext()->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// Set the type of primitive that should be rendered from this vertex buffer, in this case a line list.
	m_renderer->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
}
