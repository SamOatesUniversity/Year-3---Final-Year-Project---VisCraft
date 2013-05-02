#pragma once

#include "crenderer.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <fstream>
#include <vector>

class CMesh {

public:

	struct Vertex
	{
		D3DXVECTOR3 position;
		D3DXVECTOR2 texcoord;
		D3DXVECTOR3 normal;
	};

private:

	ID3D11Buffer						*m_vertexBuffer;					//!< 
	ID3D11Buffer						*m_indexBuffer;						//!< 
	unsigned int						m_vertexCount;						//!< 
	unsigned int						m_indexCount;						//!< 

	std::vector<CMesh::Vertex>			m_vertex;
	std::vector<unsigned int>			m_index;

public:
										//!
										CMesh();

										//! 
										~CMesh();

										//! 
	bool								LoadMesh(
											CRenderer *renderer, 
											char *meshLocation
										);

										//! 
	void								PrepareRender(
											CRenderer *renderer
										);

										//!
	void								Draw( 
											CRenderer * renderer
										);


	void								parseNode( 
											const aiScene* scene, 
											aiNode* node 
										);
};