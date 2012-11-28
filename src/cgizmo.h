#pragma once

/**
	Header file includes
*/
#include "crenderer.h"
#include "cinput.h"
#include <d3dx11async.h>

struct GizmoState {
	enum Enum {
		Free,
		Locked
	};
};

class CGizmo {
private:

	struct Vertex
	{
		D3DXVECTOR3 position;
	};

	struct MatrixBuffer
	{
		D3DXMATRIX world;
		D3DXMATRIX view;
		D3DXMATRIX projection;
	};

	struct GizmoBuffer
	{
		D3DXVECTOR4 color;
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
	ID3D11Buffer			*m_gizmoBuffer;						//!< 

	D3DXVECTOR3				m_position;							//! The position of the gizmo

	GizmoState::Enum		m_gizmoState;						//! The current state of the gizmo

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

							//! 
	void					Control( 
								CInput *input						//!< 
							);

							//! 
	const D3DXVECTOR3		&Position() const
							{
								return m_position;
							}

							//! Set the Y coord of the gizmo
	void					SetYPosition(
								const float y						//!< THe new y coordinate
							)
							{
								m_position.y = y;
							}

};