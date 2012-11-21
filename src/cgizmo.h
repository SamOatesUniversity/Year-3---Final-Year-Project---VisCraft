#pragma once

/**
	Header file includes
*/
#include "crenderer.h"
#include <d3dx11async.h>

class CGizmo {
private:

	struct Vertex
	{
		D3DXVECTOR3 position;
		D3DXVECTOR4 color;
	};

	struct MatrixBuffer
	{
		D3DXMATRIX world;
		D3DXMATRIX view;
		D3DXMATRIX projection;
	};

private:

	CRenderer				*m_renderer;						//!< Pointer to the renderer object

	ID3D11Buffer			*m_vertexBuffer;					//!< 
	ID3D11Buffer			*m_indexBuffer;						//!< 
	int						m_vertexCount;						//!< 
	int						m_indexCount;						//!< 

	ID3D11VertexShader		*m_vertexShader;					//!< 
	ID3D11PixelShader		*m_pixelShader;						//!< 
	ID3D11InputLayout		*m_layout;							//!< 
	ID3D11Buffer			*m_matrixBuffer;					//!< 

	D3DXVECTOR3				m_position;							//! The position of the gizmo

private:

public:
							//! Class constructor
							CGizmo();

							//! Class destructor
							~CGizmo();

							//! Create the gizmo
	bool					Create(
								CRenderer *renderer				//! Pointer to the renderer
							);

							//! Render the gizmo
	void					Render(
								D3DXMATRIX world,					//!< 
								D3DXMATRIX view,					//!< 
								D3DXMATRIX projection				//!< 
							);
};