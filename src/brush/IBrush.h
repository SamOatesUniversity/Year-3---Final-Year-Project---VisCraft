#pragma once

#include "../cgizmo.h"

class IBrush {

friend class CGizmo;

protected:

	float											m_size;											//!< The size of the brush
	float											m_strength;										//!< The strength of the brush

public:
													//! Class constructor
													IBrush() : m_size(1.0f), m_strength(1.0f)
													{

													}

													//! Class destructor
	virtual											~IBrush() {};

													//! Apply the brush to the terrain
	virtual void									Apply(
														CGizmo *gizmo,								//!< The gizmo controlling the brush
														CInput *input,								//!< The input device being used for the brush
														CTerrain *terrain							//!< The terrain object we want to apply the brush too
													) = 0;

													//! Gets whether the brush is lockable or not
	virtual bool									IsLockable() = 0;
};