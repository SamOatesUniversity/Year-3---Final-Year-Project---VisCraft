#pragma once

/**
	Header file includes
*/
#include "crenderer.h"

class CTerrain {
private:
	//! The vertex type deceleration
	struct VertexType
	{
		D3DXVECTOR3 position;									//!< The position of the vertex
		D3DXVECTOR4 color;										//!< The color of the vertex
	};

private:
	CRenderer				*m_renderer;						//!< Pointer to the renderer object

	D3DXVECTOR2				m_size;								//!< The size of the terrain
	unsigned int			m_vertexCount;						//!< The number of verticies
	unsigned int			m_indexCount;						//!< The number of indecies

	ID3D11Buffer			*m_vertexBuffer;					//!< Terrain D3D11 vertex buffer
	ID3D11Buffer			*m_indexBuffer;						//!< Terrain D3D11 index buffer

private:
							//! Initialize the terrains vertex buffers
	const bool				InitializeBuffers();

							//! Add a vertex at a given x,z coordinate
	void					AddVertex(
								VertexType *vertices,			//!< A pointer to the vertex array
								unsigned long *indices,			//!< A pointer to the index array
								const int x,					//!< The x coordinate of the vertex
								const int z,					//!< The z coordinate of the vertex
								unsigned int &index				//!< The index into the arrays this vertex represents
							);

public:
							//! Class constructor
							CTerrain();

							//! Class destructor
							~CTerrain();

							//! Create a flat terrain
	bool					Create( 
								CRenderer *renderer				//!< Pointer to the render
							);

							//! Release any resources allocated by the terrain class
	void					Release();

							//! Update the terrain for the D3D11 renderer
	void					Update();

							//! Returns the index count
	inline unsigned int		GetIndexCount() const
							{
								return m_indexCount;
							}
};