#pragma once

/**
	Header file includes
*/
#include "crenderer.h"
#include <stdio.h>

struct HightMapType {
	enum Enum {
		IMAGE,
		RAW
	};
};

struct HeightMap
{ 
	D3DXVECTOR3 position;
	D3DXVECTOR3 normal;
};

struct VectorType 
{ 
	D3DXVECTOR3 position;
};

union TerrainFlags 
{
	struct 
	{
		unsigned int wireframe : 1;								//!< Should the terrain render in wireframe mode
		#define TERRAIN_FLAG_WIREFRAME		0x01
	};

	unsigned int allflags;
};

class CTerrain {
private:
	//! The vertex type deceleration
	struct VertexType
	{
		D3DXVECTOR3 position;									//!< The position of the vertex
		D3DXVECTOR3 normal;										//!< The normal of the vertex
	};

private:
	TerrainFlags			m_flags;							//!< Flags representing the terrain

	CRenderer				*m_renderer;						//!< Pointer to the renderer object

	D3DXVECTOR2				m_size;								//!< The size of the terrain
	unsigned int			m_vertexCount;						//!< The number of verticies
	unsigned int			m_indexCount;						//!< The number of indecies

	HeightMap				*m_heightMap;						//!< The heightmap of ther terrain, used for modifying the terrain buffers		

	ID3D11Buffer			*m_vertexBuffer;					//!< Terrain D3D11 vertex buffer
	ID3D11Buffer			*m_indexBuffer;						//!< Terrain D3D11 index buffer

private:
							//! Initialize the terrains vertex buffers
	const bool				InitializeBuffers(
								HeightMap *heightMap			//!< Heightmap to initalize the buffers too
							);

							//! Add a vertex at a given x,z coordinate
	void					AddVertex(
								VertexType *vertices,			//!< A pointer to the vertex array
								unsigned long *indices,			//!< A pointer to the index array
								const HeightMap hm,
								unsigned int &index				//!< The index into the arrays this vertex represents
							);

							//! Calculate the normals of the terrain
	bool					CalculateNormals(
								HeightMap *heightMap			//!< The heightmap to calculate the normals of
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

							//! Load a height map into the terrain
	const bool				LoadHeightMap(
								const char *heightmapLocation,									//!< The location of the heightmap to load
								HightMapType::Enum heightmapType = HightMapType::IMAGE			//!< The type of heightmap the file conatins
							);

							//! 
	void					EnableFlag(
								unsigned int flag
							) 
							{
								m_flags.allflags |= flag;
							}

							//! 
	void					DisableFlag(
								unsigned int flag
							) 
							{
								m_flags.allflags &= ~flag;
							}

							//! 
	bool					GetFlag(
								unsigned int flag
							) 
							{
								return (m_flags.allflags & flag) == 1;
							}
};