#pragma once

/**
	Header file includes
*/
#include "crenderer.h"

class CTerrain {
private:
	struct VertexType
	{
		D3DXVECTOR3 position;
		D3DXVECTOR4 color;
	};

private:
	CRenderer				*m_renderer;						//!< Pointer to the renderer object

	D3DXVECTOR2				m_size;								//!< The size of the terrain
	unsigned int			m_vertexCount;						//!< The number of verticies
	unsigned int			m_indexCount;						//!< The number of indecies

	ID3D11Buffer			*m_vertexBuffer;					//!< 
	ID3D11Buffer			*m_indexBuffer;						//!< 

private:
							//! Initialize the terrains vertex buffers
	const bool				InitializeBuffers();

	void					AddVerticie(
								VertexType *vertices,
								unsigned long *indices,
								const int x,
								const int z,
								unsigned int &index
							);

public:
							//! Class constructor
							CTerrain();

							//! Class destructor
							~CTerrain();

							//! 
	bool					Create( 
								CRenderer *renderer				//!< Pointer to the render
							);

							//! 
	void					Release();

							//! 
	void					Update();

							//! 
	inline unsigned int		GetIndexCount()
	{
		return m_indexCount;
	}
};