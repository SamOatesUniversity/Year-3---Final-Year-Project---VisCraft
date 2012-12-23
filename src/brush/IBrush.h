#pragma once

#include "../cinput.h"
#include "../cterrain.h"

class CBrush {

protected:

	float											m_size;											//!< The size of the brush
	float											m_strength;										//!< The strength of the brush

public:
													//! Class constructor
													CBrush();

													//! Class destructor
	virtual											~CBrush();

													//! Apply the brush to the terrain
	virtual void									Apply(
														CInput *input,								//!< The input device being used for the brush
														CTerrain *terrain							//!< The terrain object we want to apply the brush too
													);


};