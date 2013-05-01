#include "cmesh.h"

CMesh::CMesh()
{
	m_indexBuffer = nullptr;
	m_vertexBuffer = nullptr;
	m_indexCount = 0;
	m_vertexCount = 0;
}

CMesh::~CMesh()
{
	SafeRelease(m_indexBuffer);
	SafeRelease(m_vertexBuffer);
}


bool CMesh::LoadMesh(
		CRenderer *renderer, 
		char *meshLocation
	)
{
	std::ifstream objFile;
	objFile.open(meshLocation, std::ios_base::in);

	struct Face {
		float position[3];
		float normal[3];
		float texcoord[3];
	};
	std::vector<Face> faces;
	std::vector<D3DXVECTOR3> verts, vertNorms;
	std::vector<D3DXVECTOR2> vertTexCoords;

	std::string line;
	while (std::getline(objFile, line))
	{
		std::vector<std::string> tokens;
		std::istringstream iss(line);
		std::copy(std::istream_iterator<std::string>(iss), std::istream_iterator<std::string>(), std::back_inserter<std::vector<std::string> >(tokens));

		if (tokens.empty())
		{
			continue;
		}

		if (tokens[0] == "v")
		{
			D3DXVECTOR3 newVert;
			newVert.x = static_cast<float>(::atof(tokens[1].c_str()));
			newVert.y = static_cast<float>(::atof(tokens[2].c_str()));
			newVert.z = static_cast<float>(::atof(tokens[3].c_str()));
			verts.push_back(newVert);
		}
		else if (tokens[0] == "vn")
		{
			D3DXVECTOR3 newVertNormal;
			newVertNormal.x = static_cast<float>(::atof(tokens[1].c_str()));
			newVertNormal.y = static_cast<float>(::atof(tokens[2].c_str()));
			newVertNormal.z = static_cast<float>(::atof(tokens[3].c_str()));
			vertNorms.push_back(newVertNormal);
		}
		else if (tokens[0] == "vt")
		{
			D3DXVECTOR2 newVertTexCoord;
			newVertTexCoord.x = static_cast<float>(::atof(tokens[1].c_str()));
			newVertTexCoord.y = static_cast<float>(::atof(tokens[2].c_str()));
			vertTexCoords.push_back(newVertTexCoord);
		}
		else if (tokens[0] == "f")
		{
			Face newFace;

			for (int tokenIndex = 1; tokenIndex <= 3; ++tokenIndex)
			{
				std::string part = tokens[tokenIndex];
				std::string posPart, normPart, texcoordPart;

				int strbreak = part.find('/');

				if (strbreak == -1) 
				{
					posPart = part;
					normPart = "";
					texcoordPart = "";
				}
				else
				{
					posPart = part.substr(0, strbreak);
					int strbreakr = part.rfind('/');

					if (strbreakr == strbreak)
					{
						normPart = part.substr(strbreakr + 1);
						texcoordPart = "";
					}
					else
					{
						normPart = part.substr(strbreak + 1, strbreakr - strbreak - 1);
						texcoordPart = part.substr(strbreakr + 1);
					}
				}

				newFace.position[tokenIndex - 1] = static_cast<float>(::atof(posPart.c_str())) - 1;
				newFace.normal[tokenIndex - 1] = static_cast<float>(::atof(normPart.c_str())) - 1;
				newFace.texcoord[tokenIndex - 1] = static_cast<float>(::atof(normPart.c_str())) - 1;
			}

			faces.push_back(newFace);
		}
	}

	objFile.close();

	m_vertexCount = verts.size();
	m_indexCount = faces.size() * 3;

	// Create the vertex array.
	CMesh::Vertex *const vertices = new CMesh::Vertex[m_vertexCount];

	// Create the index array.
	unsigned long *const indices = new unsigned long[m_indexCount];

	for (unsigned int vertIndex = 0; vertIndex < m_vertexCount; ++vertIndex)
	{
		vertices[vertIndex].position = verts[vertIndex];
		if (vertNorms.size() > vertIndex) {
			vertices[vertIndex].normal = vertNorms[vertIndex];
		}
		if (vertTexCoords.size() > vertIndex) {
			vertices[vertIndex].texcoord = vertTexCoords[vertIndex];
		}
	}

	// Load the index array with data.
	int indIndex = 0;
	for (unsigned int faceIndex = 0; faceIndex < faces.size(); ++faceIndex)
	{
		indices[indIndex]		= static_cast<unsigned int>(faces[faceIndex].position[0]);  
		indices[indIndex + 1]	= static_cast<unsigned int>(faces[faceIndex].position[1]);  
		indices[indIndex + 2]	= static_cast<unsigned int>(faces[faceIndex].position[2]); 
		indIndex += 3;
	}

	// Set up the description of the static vertex buffer.
	D3D11_BUFFER_DESC vertexBufferDesc;
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(CMesh::Vertex) * m_vertexCount;
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
	if (FAILED(renderer->GetDevice()->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer)))
	{
		delete[] vertices;
		delete[] indices;
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
	if (FAILED(renderer->GetDevice()->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer)))
	{
		delete[] vertices;
		delete[] indices;
		return false;
	}

	// Release the arrays now that the vertex and index buffers have been created and loaded.
	delete[] vertices;
	delete[] indices;

	return true;
}

void CMesh::PrepareRender(
		CRenderer *renderer
	)
{
	// Set vertex buffer stride and offset.
	unsigned int stride = sizeof(CMesh::Vertex); 
	unsigned int offset = 0;

	// Set the vertex buffer to active in the input assembler so it can be rendered.
	renderer->GetDeviceContext()->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

	// Set the index buffer to active in the input assembler so it can be rendered.
	renderer->GetDeviceContext()->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
	renderer->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void CMesh::Draw( 
		CRenderer *renderer
	)
{
	renderer->GetDeviceContext()->DrawIndexed(m_indexCount, 0, 0);
}
