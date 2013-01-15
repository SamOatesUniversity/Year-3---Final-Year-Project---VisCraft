#pragma once

#include "SAM/TVector.h"
#include "SAM/TMatrix.h"

namespace SAM 
{

	static const float PI = 3.14159f;							//!< Global define of PI

	/*
	*	\Brief Convert a value given in radians into degrees
	*/
	template<class TYPE>
	TYPE RadToDegrees(TYPE val);

	/*
	*	\Brief Convert a value given in degrees into radians
	*/
	template<class TYPE>
	TYPE DegreesToRad(TYPE val);

}