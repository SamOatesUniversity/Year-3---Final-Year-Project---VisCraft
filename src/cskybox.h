#pragma once

#include "cmesh.h"

class CSkyBox {

private:

	CMesh				*m_mesh;

public:
						//! Class constructor
						CSkyBox();

						//! Class destructor
						~CSkyBox();

						//! 
	bool				Create(	
							CRenderer *renderer
						);

						//! 
	void				Render( 
							CRenderer *renderer 
						);
};