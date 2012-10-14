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

	// create a dummy height map
	D3DXVECTOR3 *const heightMap = new D3DXVECTOR3[static_cast<int>(m_size.x * m_size.y)];
	// Read the image data into the height map.
	for (int z = 0; z < m_size.y; ++z)
	{
		for (int x = 0; x < m_size.x; ++x)
		{
			const int index = static_cast<int>(m_size.y * z) + x;
			heightMap[index].x = static_cast<float>(x);
			heightMap[index].y = 0.0f;
			heightMap[index].z = static_cast<float>(z);
		}
	}

	if (!InitializeBuffers(heightMap)) 
	{
		delete heightMap;
		return false;
	}

	delete heightMap;

	return true;
}

/*
 *	\brief Setup the terrain buffers to a default state
*/
const bool CTerrain::InitializeBuffers(
		D3DXVECTOR3 *heightMap			//!< Heightmap to initalize the buffers too
	)
{
	if (heightMap == nullptr)
		return false;

	m_indexCount = m_vertexCount = (static_cast<unsigned int>(m_size.x) - 1) * (static_cast<unsigned int>(m_size.y) - 1) * 12;

	VertexType *const vertices = new VertexType[m_vertexCount];

	unsigned long *const indices = new unsigned long[m_indexCount];

	unsigned int index = 0;

	for (int z = 0; z < (m_size.y - 1); ++z)
	{
		for(int x = 0; x < (m_size.x - 1); ++x)
		{
			const int bottomLeft	= static_cast<int>(m_size.y * z) + x;				
			const int bottomRight	= static_cast<int>(m_size.y * z) + (x+1);			
			const int topLeft		= static_cast<int>(m_size.y * (z+1)) + x;			
			const int topRight		= static_cast<int>(m_size.y * (z+1)) + (x+1);	

			AddVertex(vertices, indices, heightMap[topLeft].x,		heightMap[topLeft].y,		heightMap[topLeft].z,		index);
			AddVertex(vertices, indices, heightMap[topRight].x,		heightMap[topRight].y,		heightMap[topRight].z,		index);

			AddVertex(vertices, indices, heightMap[topRight].x,		heightMap[topRight].y,		heightMap[topRight].z,		index);
			AddVertex(vertices, indices, heightMap[bottomLeft].x,	heightMap[bottomLeft].y,	heightMap[bottomLeft].z,	index);

			AddVertex(vertices, indices, heightMap[bottomLeft].x,	heightMap[bottomLeft].y,	heightMap[bottomLeft].z,	index);
			AddVertex(vertices, indices, heightMap[topLeft].x,		heightMap[topLeft].y,		heightMap[topLeft].z,		index);

			AddVertex(vertices, indices, heightMap[bottomLeft].x,	heightMap[bottomLeft].y,	heightMap[bottomLeft].z,	index);
			AddVertex(vertices, indices, heightMap[topRight].x,		heightMap[topRight].y,		heightMap[topRight].z,		index);

			AddVertex(vertices, indices, heightMap[topRight].x,		heightMap[topRight].y,		heightMap[topRight].z,		index);
			AddVertex(vertices, indices, heightMap[bottomRight].x,	heightMap[bottomRight].y,	heightMap[bottomRight].z,	index);

			AddVertex(vertices, indices, heightMap[bottomRight].x,	heightMap[bottomRight].y,	heightMap[bottomRight].z,	index);
			AddVertex(vertices, indices, heightMap[bottomLeft].x,	heightMap[bottomLeft].y,	heightMap[bottomLeft].z,	index);
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
		const float x, 
		const float y, 
		const float z, 
		unsigned int &index 
	)
{
	vertices[index].position = D3DXVECTOR3(x, y, z);
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

/*
 *	\brief Load a height map into the terrain
*/
const bool CTerrain::LoadHeightMap( 
		const char *heightmapLocation,				//!< The location of the heightmap to load
		HightMapType::Enum heightmapType			//!< The type of heightmap the file conatins
	)
{
	FILE *file = nullptr;
	if (fopen_s(&file, heightmapLocation, "rb") != 0)
	{
		ASSERT(false, "Failed to open the heightmap file");
		return false;
	}

	BITMAPFILEHEADER fileHeader;
	if(fread(&fileHeader, sizeof(BITMAPFILEHEADER), 1, file) != 1)
	{
		ASSERT(false, "Failed to read heightmap header");
		return false;
	}

	BITMAPINFOHEADER fileInfo;
	if(fread(&fileInfo, sizeof(BITMAPINFOHEADER), 1, file) != 1)
	{
		ASSERT(false, "Failed to read heightmap info header");
		return false;
	}

	m_size = D3DXVECTOR2(static_cast<float>(fileInfo.biWidth), static_cast<float>(fileInfo.biHeight));

	const unsigned int imageSize = static_cast<const unsigned int>(m_size.x * m_size.y * 3);

	unsigned char *const image = new unsigned char[imageSize];

	fseek(file, fileHeader.bfOffBits, SEEK_SET);

	if (fread(image, 1, imageSize, file) != imageSize)
	{
		ASSERT(false, "Failed to read image data");
		return false;
	}

	fclose(file);

	// get the image data into a height map array
	D3DXVECTOR3 *const heightMap = new D3DXVECTOR3[static_cast<int>(m_size.x * m_size.y)];
	
	unsigned int y = 0;

	// Read the image data into the height map.
	for (int z = 0; z < m_size.y; ++z)
	{
		for (int x = 0; x < m_size.x; ++x)
		{
			const float height = image[y] * 0.1f;

			const int index = static_cast<int>(m_size.y * z) + x;
			heightMap[index].x = static_cast<float>(x);
			heightMap[index].y = height;
			heightMap[index].z = static_cast<float>(z);

			y += 3;
		}
	}

	// free the old buffers
	Release();

	// load in the height map data to our buffers
	if (!InitializeBuffers(heightMap))
		return false;

	delete image;
	delete heightMap;

	return true;
}
