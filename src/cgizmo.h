#pragma once

/**
	Header file includes
*/
#include "crenderer.h"
#include "cinput.h"
#include "cterrain.h"
#include "ccamera.h"
#include "brush/IBrush.h"
#include "kinect/CKinect.h"

#include <d3dx11async.h>
#include <vector>

struct GizmoState {
	enum Enum {
		Free,
		Locked
	};
};

struct BrushType {
	enum Enum {
		Deform,
		Raise,
		Lower,
		Smooth,
		Noise,
		Noof
	};
};

struct InputType {
	enum Enum {
		Mouse,
		Kinect,
		Noof
	};
};

class CGizmo {

friend class IBrush;

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

	struct DragData {
		D3DXVECTOR2 startMousePosition;
		float lastY;
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

	D3DXVECTOR3				m_position;							//!< The position of the gizmo
	GizmoState::Enum		m_gizmoState;						//!< The current state of the gizmo
	DragData				m_dragData;							//!<

	BrushType::Enum			m_currentBrush;						//!< The currently active brush
	std::vector<IBrush*>	m_brush;							//!< A list of all the avaluble brushes

	InputType::Enum			m_inputType;						//!< 

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
								D3DXMATRIX projection,				//!< 
								CCamera *camera						//!< 
							);

							//! 
	void					Control( 
								CInput *input,						//!< 
								CTerrain *terrain,					//!< 
								CCamera *camera,					//!< 
								CKinect *kinect						//!< 
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

							//! Grab a reference of the gizmos drag data
	DragData				&DragData()
							{
								return m_dragData;
							}

							//!
	GizmoState::Enum		GetGizmoState() const
							{
								return m_gizmoState;
							}

							//! 
	void					SetCurrentBrush(
								const BrushType::Enum brushType
							);

							//! 
	void					SetInputType(
								InputType::Enum inputType
							)
							{
								m_inputType = inputType;
							}
};