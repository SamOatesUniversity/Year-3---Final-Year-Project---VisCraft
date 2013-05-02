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
	Assimp::Importer Importer;
	const aiScene* scene = Importer.ReadFile(meshLocation, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenNormals | aiProcess_GenUVCoords);
	parseNode(scene, scene->mRootNode);
	
	m_vertexCount = m_vertex.size();
	m_indexCount = m_index.size();

	// Create the vertex array.
	CMesh::Vertex *const vertices = new CMesh::Vertex[m_vertexCount];

	// Create the index array.
	unsigned long *const indices = new unsigned long[m_indexCount];

	// copy shit from vectors
	for (unsigned int vertIndex = 0; vertIndex < m_vertexCount; ++vertIndex)
	{
		vertices[vertIndex].position = m_vertex[vertIndex].position;
		vertices[vertIndex].normal = m_vertex[vertIndex].normal;
		vertices[vertIndex].texcoord = m_vertex[vertIndex].texcoord;
	}

	for (unsigned int faceIndex = 0; faceIndex < m_index.size(); ++faceIndex)
	{
		indices[faceIndex] = m_index[faceIndex];
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

void CMesh::parseNode( 
		const aiScene* scene, 
		aiNode* node 
	)
{
	// load all the nodes meshes
	for (unsigned int meshIndex = 0; meshIndex < node->mNumMeshes; ++meshIndex)
	{
		const struct aiMesh *mesh = scene->mMeshes[node->mMeshes[meshIndex]];
		for (unsigned int vertexIndex = 0; vertexIndex < mesh->mNumVertices; ++vertexIndex)
		{
			CMesh::Vertex vert;
			vert.position = D3DXVECTOR3(mesh->mVertices[vertexIndex].x, mesh->mVertices[vertexIndex].y, mesh->mVertices[vertexIndex].z);
			vert.normal = D3DXVECTOR3(mesh->mNormals[vertexIndex].x, mesh->mNormals[vertexIndex].y, mesh->mNormals[vertexIndex].z);
			if (mesh->mTextureCoords[0] != NULL) vert.texcoord = D3DXVECTOR2(mesh->mTextureCoords[0][vertexIndex].x, mesh->mTextureCoords[0][vertexIndex].y);
			m_vertex.push_back(vert);
		}

		for (unsigned int faceIndex = 0; faceIndex < mesh->mNumFaces; ++faceIndex)
		{
			const struct aiFace* face = &mesh->mFaces[faceIndex];
			
			for (unsigned int faceIndex = 0; faceIndex < face->mNumIndices; ++faceIndex)
			{
				m_index.push_back(face->mIndices[faceIndex]);
			}

		}
	}

	// recurse through children
	for (unsigned int childIndex = 0; childIndex < node->mNumChildren; ++childIndex)
	{
		parseNode(scene, node->mChildren[childIndex]);
	}
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
