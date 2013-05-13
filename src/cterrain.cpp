#include "cterrain.h"
#include <fstream>

/*
 *	\brief Class constructor
*/
CTerrain::CTerrain()
{
	m_flags.allflags = 0;
	m_vertexCount = 0;
	m_indexCount = 0;
	
	m_renderer = nullptr;
	m_vertexBuffer = nullptr;
	m_indexBuffer = nullptr;
	m_heightMap = nullptr;

	m_normalsBuffer = nullptr;

	m_size = D3DXVECTOR2(128, 128);
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

	SafeDelete(m_heightMap);
	m_heightMap = heightMap;

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

	CalculateTextureCoordinates(heightMap);

	m_indexCount = m_vertexCount = (static_cast<unsigned int>(m_size.x) - 1) * (static_cast<unsigned int>(m_size.y) - 1) * 6;

	VertexType *const vertices = new VertexType[m_vertexCount];

	unsigned long *const indices = new unsigned long[m_indexCount];

	unsigned int index = 0;

	for (int z = 0; z < (m_size.y - 1); ++z)
	{
		for(int x = 0; x < (m_size.x - 1); ++x)
		{
			const int bottomLeft	= static_cast<int>(m_size.y * z) + x;				
			const int bottomRight	= static_cast<int>(m_size.y * z) + (x + 1);			
			const int topLeft		= static_cast<int>(m_size.y * (z + 1)) + x;			
			const int topRight		= static_cast<int>(m_size.y * (z + 1)) + (x + 1);	

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
	if (FAILED(m_renderer->GetDevice()->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer)))
	{
		delete[] vertices;
		delete[] indices;
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
		delete[] vertices;
		delete[] indices;
		delete heightMap;
		return false;
	}

	// Release the arrays now that the buffers have been created and loaded.
	delete[] vertices;
	delete[] indices;

	return true;
}

/*
 *	\brief Add a vertex at a given x,z coordinate
*/
void CTerrain::AddVertex( 
		VertexType *vertices,
		unsigned long *indices,
		const HeightMap &hm, 
		unsigned int &index 
	)
{
	vertices[index].position = hm.position;
	vertices[index].texture = hm.texture;
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
	// If our normal buffer doesn't exist, create it
	if (m_normalsBuffer == nullptr) {
		m_normalsBuffer = new VectorType[static_cast<int>((m_size.x - 1) * (m_size.y - 1))];
	}

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
			m_normalsBuffer[index].position.x = (vector1[1] * vector2[2]) - (vector1[2] * vector2[1]);
			m_normalsBuffer[index].position.y = (vector1[2] * vector2[0]) - (vector1[0] * vector2[2]);
			m_normalsBuffer[index].position.z = (vector1[0] * vector2[1]) - (vector1[1] * vector2[0]);
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

				sum[0] += m_normalsBuffer[index].position.x;
				sum[1] += m_normalsBuffer[index].position.y;
				sum[2] += m_normalsBuffer[index].position.z;
				count++;
			}

			// Bottom right face.
			if ((x < (m_size.x - 1)) && ((z - 1) >= 0))
			{
				int index = static_cast<int>(((z - 1) * (m_size.y - 1)) + x);

				sum[0] += m_normalsBuffer[index].position.x;
				sum[1] += m_normalsBuffer[index].position.y;
				sum[2] += m_normalsBuffer[index].position.z;
				count++;
			}

			// Upper left face.
			if (((x - 1) >= 0) && (z < (m_size.y - 1)))
			{
				int index = static_cast<int>((z * (m_size.y - 1)) + (x - 1));

				sum[0] += m_normalsBuffer[index].position.x;
				sum[1] += m_normalsBuffer[index].position.y;
				sum[2] += m_normalsBuffer[index].position.z;
				count++;
			}

			// Upper right face.
			if ((x < (m_size.x - 1)) && (z < (m_size.y - 1)))
			{
				int index = static_cast<int>((z * (m_size.y - 1)) + x);

				sum[0] += m_normalsBuffer[index].position.x;
				sum[1] += m_normalsBuffer[index].position.y;
				sum[2] += m_normalsBuffer[index].position.z;
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
	//delete [] normals;

	return true;
}

/*
 *	\brief Calculate the texture coordinates
*/
void CTerrain::CalculateTextureCoordinates(
		HeightMap *heightMap							//!< The heightmap to calculate the normals of
	)
{
	static const int TEXTURE_REPEAT = static_cast<int>(m_size.x * 0.5f);

	// Calculate how much to increment the texture coordinates by.
	float incrementValue = static_cast<float>(TEXTURE_REPEAT) / static_cast<float>(m_size.x);

	// Calculate how many times to repeat the texture.
	float temp = static_cast<float>(m_size.x) / static_cast<float>(TEXTURE_REPEAT);
	int incrementCount = static_cast<int>(temp);

	// Initialize the tu and tv coordinate values.
	float tuCoordinate = 0.0f;
	float tvCoordinate = 1.0f;

	// Initialize the tu and tv coordinate indexes.
	int tuCount = 0;
	int tvCount = 0;

	// Loop through the entire height map and calculate the tu and tv texture coordinates for each vertex.
	for(int j = 0; j < m_size.y; ++j)
	{
		for(int i = 0; i < m_size.x; ++i)
		{
			// Store the texture coordinate in the height map.
			int vert = static_cast<int>((m_size.y * j) + i);
			heightMap[vert].texture.x = tuCoordinate;
			heightMap[vert].texture.y = tvCoordinate;

			// Increment the tu texture coordinate by the increment value and increment the index by one.
			tuCoordinate += incrementValue;
			tuCount++;

			// Check if at the far right end of the texture and if so then start at the beginning again.
			if (tuCoordinate >= 1.0f)
			{
				tuCoordinate = 0.0f;
				tuCount = 0;
			}
		}

		// Increment the tv texture coordinate by the increment value and increment the index by one.
		tvCoordinate -= incrementValue;
		tvCount++;

		// Check if at the top of the texture and if so then start at the bottom again.
		if (tvCount == incrementCount)
		{
			tvCoordinate = 1.0f;
			tvCount = 0;
		}
	}
}

/*
 *	\brief Release any resources allocated by the terrain class
*/
void CTerrain::Release()
{
	SafeRelease(m_indexBuffer);
	SafeRelease(m_vertexBuffer);
	SafeDelete(m_heightMap);
	SafeDelete(m_normalsBuffer);
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

	// Set the type of primitive that should be rendered from this vertex buffer
	// if we are drawing in wireframe use a line list
	// else use a triangle list
	if (m_flags.wireframe)
		m_renderer->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST); 
	else
		m_renderer->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

/*
 *	\brief Update the buffers from the current height map
*/
void CTerrain::UpdateHeightMap()
{
	if (!CalculateNormals(m_heightMap))
		return;

	D3D11_MAPPED_SUBRESOURCE resource;
	const HRESULT result = m_renderer->GetDeviceContext()->Map(m_vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);
	if (FAILED(result))	
		return;

	VertexType *const vertices = static_cast<VertexType*>(resource.pData);

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

			AddVertex(vertices, indices, m_heightMap[topLeft],		index);
			AddVertex(vertices, indices, m_heightMap[topRight],		index);

			AddVertex(vertices, indices, m_heightMap[bottomLeft],	index);
			AddVertex(vertices, indices, m_heightMap[bottomLeft],	index);

			AddVertex(vertices, indices, m_heightMap[topRight],		index);
			AddVertex(vertices, indices, m_heightMap[bottomRight],	index);
		}
	}

	m_renderer->GetDeviceContext()->Unmap(m_vertexBuffer, 0);

	delete[] indices;
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
		VISASSERT(false, "Failed to open the heightmap file");
		return false;
	}

	BITMAPFILEHEADER fileHeader;
	if(fread(&fileHeader, sizeof(BITMAPFILEHEADER), 1, file) != 1)
	{
		VISASSERT(false, "Failed to read heightmap header");
		return false;
	}

	BITMAPINFOHEADER fileInfo;
	if(fread(&fileInfo, sizeof(BITMAPINFOHEADER), 1, file) != 1)
	{
		VISASSERT(false, "Failed to read heightmap info header");
		return false;
	}

	if (fileInfo.biWidth != fileInfo.biHeight)
	{
		VISASSERT(false, "Heightmap image is not square");
		return false;
	}

	if (fileInfo.biBitCount / 8 != 3)
	{
		VISASSERT(false, "The height map is not a 24 bit image");
		return false;
	}

	m_size = D3DXVECTOR2(static_cast<float>(fileInfo.biWidth), static_cast<float>(fileInfo.biHeight));

	const unsigned int imageSize = static_cast<const unsigned int>(m_size.x * m_size.y * 3);

	unsigned char *const image = new unsigned char[imageSize];

	fseek(file, fileHeader.bfOffBits, SEEK_SET);

	if (fread(image, 1, imageSize, file) != imageSize)
	{
		VISASSERT(false, "Failed to read image data");
		delete[] image;
		return false;
	}

	fclose(file);

	// free the old buffers
	Release();

	// get the image data into a height map array
	m_heightMap = new HeightMap[static_cast<int>(m_size.x * m_size.y)];
	
	unsigned int y = 0;

	// Read the image data into the height map.
	for (int z = 0; z < m_size.y; ++z)
	{
		for (int x = 0; x < m_size.x; ++x)
		{
			const float height = image[y];

			const int index = static_cast<int>(m_size.y * z) + x;
			m_heightMap[index].position.x = static_cast<float>(x);
			m_heightMap[index].position.y = height;
			m_heightMap[index].position.z = static_cast<float>(z);

			y += 3;
		}
	}

	// load in the height map data to our buffers
	if (!InitializeBuffers(m_heightMap))
	{
		delete[] image;
		return false;
	}

	delete[] image;

	return true;
}

/*
 *	\brief Gets the y height of the terrain at a given x and z location
*/
HeightMap *CTerrain::GetTerrainVertexAt( 
		const float x,															//!< The x coord to look up the vertex from 
		const float z															//!< The z coord to look up the vertex from 
	) const
{
	D3DXVECTOR2 lookupVec = D3DXVECTOR2(x, z);

	int closestVertIndex = -1;
	float lastDistance = m_size.x * m_size.y;

	const int heightMapSize = static_cast<int>(m_size.x * m_size.y);
	for (int heightMapIndex = 0; heightMapIndex < heightMapSize; ++heightMapIndex)
	{
		if (GetFlag(TERRAIN_FLAG_LOCK)) {
			return nullptr;
		}

		const HeightMap hmap = m_heightMap[heightMapIndex];

		if (hmap.position.x - x > 1 || hmap.position.x - x < -1)
			continue;

		if (hmap.position.z - z > 1 || hmap.position.z - z < -1)
			continue;

		const D3DXVECTOR2 currentVec = D3DXVECTOR2(hmap.position.x, hmap.position.z);
		const D3DXVECTOR2 distanceVec = currentVec - lookupVec;
		const float currentDistance = sqrt((distanceVec.x * distanceVec.x) + (distanceVec.y * distanceVec.y));
		if (currentDistance < lastDistance)
		{
			lastDistance = currentDistance;
			closestVertIndex = heightMapIndex;
		}
	}

	if (closestVertIndex == -1) 
	{
		return nullptr;
	}

	return &m_heightMap[closestVertIndex];
}

/*
 *	\brief Gets the y height of the terrain at a given x and z location
*/
const float CTerrain::GetTerrainHeightAt( 
		const float x,										//!< The x coord to look up the y from 
		const float z										//!< The z coord to look up the y from 
	) const
{
	HeightMap *point = GetTerrainVertexAt(x, z);
	if (point == nullptr) {
		return 0.0f;
	}
	return point->position.y;
}

void CTerrain::Reset()
{
	for (int z = 0; z < m_size.y; ++z)
	{
		for (int x = 0; x < m_size.x; ++x)
		{
			const int index = static_cast<int>(m_size.y * z) + x;
			m_heightMap[index].position.x = static_cast<float>(x);
			m_heightMap[index].position.y = 0.0f;
			m_heightMap[index].position.z = static_cast<float>(z);
		}
	}

	UpdateHeightMap();
}

const D3DXVECTOR2 CTerrain::GetSize()
{
	return m_size;
}

float CTerrain::CalculateAverageTerrainHeight(
		D3DXVECTOR2	position,
		int area
	)
{
	float count = 0.0f;
	float height = 0.0f;

	for (int xOffset = -area; xOffset <= area; ++xOffset)
	{
		for (int zOffset = -area; zOffset <= area; ++zOffset)
		{
			HeightMap *hmap = GetTerrainVertexAt(position.x + xOffset, position.y + zOffset);
			if (hmap == nullptr)
			{
				continue;
			}

			height += hmap->position.y;
			count++;
		}
	}

	return height / count;	
}

void CTerrain::SaveHeightMap( 
		char* fileName 
	)
{

	const int size = static_cast<int>(m_size.x * m_size.y) * 3;

	BITMAPFILEHEADER bmfh;
	BITMAPINFOHEADER info;
	memset ( &bmfh, 0, sizeof (BITMAPFILEHEADER ) );
	memset ( &info, 0, sizeof (BITMAPINFOHEADER ) );

	bmfh.bfType = 0x4d42;       // 0x4d42 = 'BM'
	bmfh.bfReserved1 = 0;
	bmfh.bfReserved2 = 0;
	bmfh.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + size;
	bmfh.bfOffBits = 0x36;

	info.biSize = sizeof(BITMAPINFOHEADER);
	info.biWidth = static_cast<LONG>(m_size.x);
	info.biHeight = static_cast<LONG>(m_size.y);
	info.biPlanes = 1;	
	info.biBitCount = 24;
	info.biCompression = BI_RGB;	
	info.biSizeImage = 0;
	info.biXPelsPerMeter = 0x0ec4;  
	info.biYPelsPerMeter = 0x0ec4;     
	info.biClrUsed = 0;	
	info.biClrImportant = 0; 	

	HANDLE file = CreateFile(fileName , GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (NULL == file)
	{
		CloseHandle (file);
		return;
	}

	unsigned long bwritten;
	if (WriteFile (file, &bmfh, sizeof ( BITMAPFILEHEADER ), &bwritten, NULL ) == false)
	{	
		CloseHandle(file);
		return;
	}

	if (WriteFile(file, &info, sizeof ( BITMAPINFOHEADER ), &bwritten, NULL) == false)
	{	
		CloseHandle(file);
		return;
	}

	const float min = GetLowestTerrainPoint(); 
	//const float max = GetHighestTerrainPoint(); 

	BYTE *fileBuffer = new BYTE[size];

	for (int terrainIndex = 0; terrainIndex < size; terrainIndex += 3)
	{
		HeightMap *hmap = &m_heightMap[terrainIndex / 3];
		const BYTE color = static_cast<BYTE>((hmap->position.y - min));
		fileBuffer[terrainIndex] = fileBuffer[terrainIndex+1] = fileBuffer[terrainIndex+2] = color;
	}
	
	if (WriteFile(file, fileBuffer, size, &bwritten, NULL ) == false)
	{	
		CloseHandle(file);
		return;
	}
	delete[] fileBuffer;

	CloseHandle(file);
}

const float CTerrain::GetLowestTerrainPoint()
{
	float lowest = 0;
	for (int terrainIndex = 0; terrainIndex < m_size.x * m_size.y; ++terrainIndex)
	{
		if (m_heightMap[terrainIndex].position.y < lowest)
		{
			lowest = m_heightMap[terrainIndex].position.y;
		}
	}
	return lowest;
}

const float CTerrain::GetHighestTerrainPoint()
{
	float highest = 0;
	for (int terrainIndex = 0; terrainIndex < m_size.x * m_size.y; ++terrainIndex)
	{
		if (m_heightMap[terrainIndex].position.y > highest)
		{
			highest = m_heightMap[terrainIndex].position.y;
		}
	}
	return highest;
}
