#pragma once

#include "crenderer.h"
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

	ID3D11Buffer			*m_vertexBuffer;					//!< 
	ID3D11Buffer			*m_indexBuffer;						//!< 
	unsigned int			m_vertexCount;						//!< 
	unsigned int			m_indexCount;						//!< 

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
};