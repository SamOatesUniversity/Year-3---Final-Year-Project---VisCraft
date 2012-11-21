#include "cterrain.h"

/*
 *	\brief Class constructor
*/
CTerrain::CTerrain()
{
	m_renderer = nullptr;
	m_vertexBuffer = nullptr;
	m_indexBuffer = nullptr;
	m_heightMap = nullptr;

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
	HeightMap *const heightMap = new HeightMap[static_cast<int>(m_size.x * m_size.y)];
	// Read the image data into the height map.
	for (int z = 0; z < m_size.y; ++z)
	{
		for (int x = 0; x < m_size.x; ++x)
		{
			const int index = static_cast<int>(m_size.y * z) + x;
			heightMap[index].position.x = static_cast<float>(x);
			heightMap[index].position.y = 0.0f;
			heightMap[index].position.z = static_cast<float>(z);
		}
	}

	if (!InitializeBuffers(heightMap)) 
		return false;

	return true;
}

/*
 *	\brief Setup the terrain buffers to a default state
*/
const bool CTerrain::InitializeBuffers(
		HeightMap *heightMap			//!< Heightmap to initalize the buffers too
	)
{
	if (heightMap == nullptr)
		return false;

	if (!CalculateNormals(heightMap))
		return false;

	m_indexCount = m_vertexCount = (static_cast<unsigned int>(m_size.x) - 1) * (static_cast<unsigned int>(m_size.y) - 1) * 6;

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

			AddVertex(vertices, indices, heightMap[topLeft],		index);
			AddVertex(vertices, indices, heightMap[topRight],		index);

			AddVertex(vertices, indices, heightMap[bottomLeft],		index);
			AddVertex(vertices, indices, heightMap[bottomLeft],		index);

			AddVertex(vertices, indices, heightMap[topRight],		index);
			AddVertex(vertices, indices, heightMap[bottomRight],	index);
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
	{
		delete vertices;
		delete indices;
		delete heightMap;
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
	if (FAILED(m_renderer->GetDevice()->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer)))
	{
		delete vertices;
		delete indices;
		delete heightMap;
		return false;
	}

	// Release the arrays now that the buffers have been created and loaded.
	delete vertices;
	delete indices;

	SafeDelete(m_heightMap);
	m_heightMap = heightMap;

	return true;
}

/*
 *	\brief Add a vertex at a given x,z coordinate
*/
void CTerrain::AddVertex( 
		VertexType *vertices,
		unsigned long *indices,
		const HeightMap hm, 
		unsigned int &index 
	)
{
	vertices[index].position = hm.position;
	vertices[index].normal = hm.normal;
	indices[index] = index;
	index++;
}

/*
 *	\brief Calculate the normals of the terrain
*/
bool CTerrain::CalculateNormals(
		HeightMap *heightMap							//!< 
	)
{
	// Create a temporary array to hold the un-normalized normal vectors.
	VectorType *const normals = new VectorType[static_cast<int>((m_size.x - 1) * (m_size.y - 1))];
	
	float vertex1[3], vertex2[3], vertex3[3], vector1[3], vector2[3], sum[3], length;

	// Go through all the faces in the mesh and calculate their normals.
	for (int z = 0; z < (m_size.y - 1); ++z)
	{
		for (int x = 0; x < (m_size.x - 1); ++x)
		{
			int index1 = static_cast<int>((z * m_size.y) + x);
			int index2 = static_cast<int>((z * m_size.y) + (x + 1));
			int index3 = static_cast<int>(((z + 1) * m_size.y) + x);

			// Get three vertices from the face.
			vertex1[0] = heightMap[index1].position.x;
			vertex1[1] = heightMap[index1].position.y;
			vertex1[2] = heightMap[index1].position.z;

			vertex2[0] = heightMap[index2].position.x;
			vertex2[1] = heightMap[index2].position.y;
			vertex2[2] = heightMap[index2].position.z;

			vertex3[0] = heightMap[index3].position.x;
			vertex3[1] = heightMap[index3].position.y;
			vertex3[2] = heightMap[index3].position.z;

			// Calculate the two vectors for this face.
			vector1[0] = vertex1[0] - vertex3[0];
			vector1[1] = vertex1[1] - vertex3[1];
			vector1[2] = vertex1[2] - vertex3[2];
			vector2[0] = vertex3[0] - vertex2[0];
			vector2[1] = vertex3[1] - vertex2[1];
			vector2[2] = vertex3[2] - vertex2[2];

			int index = static_cast<int>((z * (m_size.y - 1)) + x);

			// Calculate the cross product of those two vectors to get the un-normalized value for this face normal.
			normals[index].position.x = (vector1[1] * vector2[2]) - (vector1[2] * vector2[1]);
			normals[index].position.y = (vector1[2] * vector2[0]) - (vector1[0] * vector2[2]);
			normals[index].position.z = (vector1[0] * vector2[1]) - (vector1[1] * vector2[0]);
		}
	}

	// Now go through all the vertices and take an average of each face normal 	
	// that the vertex touches to get the averaged normal for that vertex.
	for (int z = 0; z < m_size.y; ++z)
	{
		for (int x = 0; x < m_size.x; ++x)
		{
			// Initialize the sum.
			sum[0] = 0.0f;
			sum[1] = 0.0f;
			sum[2] = 0.0f;

			// Initialize the count.
			int count = 0;

			// Bottom left face.
			if (((x - 1) >= 0) && ((z - 1) >= 0))
			{
				int index = static_cast<int>(((z - 1) * (m_size.y - 1)) + (x - 1));

				sum[0] += normals[index].position.x;
				sum[1] += normals[index].position.y;
				sum[2] += normals[index].position.z;
				count++;
			}

			// Bottom right face.
			if ((x < (m_size.x - 1)) && ((z - 1) >= 0))
			{
				int index = static_cast<int>(((z - 1) * (m_size.y - 1)) + x);

				sum[0] += normals[index].position.x;
				sum[1] += normals[index].position.y;
				sum[2] += normals[index].position.z;
				count++;
			}

			// Upper left face.
			if (((x - 1) >= 0) && (z < (m_size.y - 1)))
			{
				int index = static_cast<int>((z * (m_size.y - 1)) + (x - 1));

				sum[0] += normals[index].position.x;
				sum[1] += normals[index].position.y;
				sum[2] += normals[index].position.z;
				count++;
			}

			// Upper right face.
			if ((x < (m_size.x - 1)) && (z < (m_size.y - 1)))
			{
				int index = static_cast<int>((z * (m_size.y - 1)) + x);

				sum[0] += normals[index].position.x;
				sum[1] += normals[index].position.y;
				sum[2] += normals[index].position.z;
				count++;
			}

			// Take the average of the faces touching this vertex.
			sum[0] = (sum[0] / static_cast<float>(count));
			sum[1] = (sum[1] / static_cast<float>(count));
			sum[2] = (sum[2] / static_cast<float>(count));

			// Calculate the length of this normal.
			length = sqrt((sum[0] * sum[0]) + (sum[1] * sum[1]) + (sum[2] * sum[2]));

			// Get an index to the vertex location in the height map array.
			int index = static_cast<int>((z * m_size.y) + x);

			// Normalize the final shared normal for this vertex and store it in the height map array.
			heightMap[index].normal.x = (sum[0] / length);
			heightMap[index].normal.y = (sum[1] / length);
			heightMap[index].normal.z = (sum[2] / length);
		}
	}

	// Release the temporary normals.
	delete [] normals;

	return true;
}

/*
 *	\brief Release any resources allocated by the terrain class
*/
void CTerrain::Release()
{
	SafeRelease(m_indexBuffer);
	SafeRelease(m_vertexBuffer);
	SafeDelete(m_heightMap);
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
	m_renderer->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST); //D3D11_PRIMITIVE_TOPOLOGY_LINELIST
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
	HeightMap *const heightMap = new HeightMap[static_cast<int>(m_size.x * m_size.y)];
	
	unsigned int y = 0;

	// Read the image data into the height map.
	for (int z = 0; z < m_size.y; ++z)
	{
		for (int x = 0; x < m_size.x; ++x)
		{
			const float height = image[y] * 0.1f;

			const int index = static_cast<int>(m_size.y * z) + x;
			heightMap[index].position.x = static_cast<float>(x);
			heightMap[index].position.y = height;
			heightMap[index].position.z = static_cast<float>(z);

			y += 3;
		}
	}

	// free the old buffers
	Release();

	// load in the height map data to our buffers
	if (!InitializeBuffers(heightMap))
		return false;

	delete image;

	return true;
}
