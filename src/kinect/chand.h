#pragma once

#include <windows.h>
#include <NuiApi.h>
#include <SAM.h>

class CHand {
private:

	unsigned int									m_frameWidth;									//!< 
	unsigned int									m_frameHeight;									//!<

	SAM::TVector<float, 2>							m_palm;											//!<

public:
													//! Class constructor
													CHand();

													//! Class destructor
													~CHand();

													//! Create the hand instance
	bool											Create(
														int frameWidth,
														int frameHeight
													);

													//! Try and find a hand from the depth image
	RGBQUAD*										FindFromDepth(
														RGBQUAD *depthData	
													);

};