#pragma once

#include "../cgizmo.h"

class IBrush {

friend class CGizmo;

protected:

	int												m_size;											//!< The size of the brush
	float											m_strength;										//!< The strength of the brush

public:
													//! Class constructor
													IBrush() : m_size(10), m_strength(1.0f)
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
	virtual bool									IsLockable() const = 0;
};

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
	virtual bool									IsLockable() const
													{
														return true;
													}
};

class CBrushLower : public IBrush {

private:

public:
													//! Class constructor
													CBrushLower();

													//! Class destructor
	virtual											~CBrushLower();

													//! Apply the brush to the terrain
	virtual void									Apply(
														CGizmo *gizmo,								//!< The gizmo controlling this brush
														CInput *input,								//!< The input device being used for the brush
														CTerrain *terrain							//!< The terrain object we want to apply the brush too
													);

													//! Gets whether the brush is lockable or not
	virtual bool									IsLockable() const
													{
														return false;
													}
};

class CBrushRaise : public IBrush
{
private:

public:
													//! Class constructor
													CBrushRaise();

													//! Class destructor
	virtual											~CBrushRaise();

													//! Apply the brush to the terrain
	virtual void									Apply(
														CGizmo *gizmo,								//!< The gizmo controlling this brush
														CInput *input,								//!< The input device being used for the brush
														CTerrain *terrain							//!< The terrain object we want to apply the brush too
													);

													//! Gets whether the brush is lockable or not
	virtual bool									IsLockable() const
													{
														return false;
													}
};