#pragma once

/**
	Header file includes
*/
#include "crenderer.h"
#include <D3DX11.h>

class CGizmo {
private:

	CRenderer				*m_renderer;						//!< Pointer to the renderer object

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
	void					Render();
};