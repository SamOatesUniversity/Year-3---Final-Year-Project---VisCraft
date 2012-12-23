#pragma once

#include "IBrush.h"

class CBrushDeform : public IBrush {

private:



public:
													//! Class constructor
													CBrushDeform();

													//! Class destructor
	virtual											~CBrushDeform();

													//! Apply the brush to the terrain
	virtual void									Apply(
														CGizmo *gizmo,								//!< The gizmo controlling this brush
														CInput *input,								//!< The input device being used for the brush
														CTerrain *terrain							//!< The terrain object we want to apply the brush too
													);

													//! Gets whether the brush is lockable or not
	virtual bool									IsLockable()
													{
														return true;
													}
};