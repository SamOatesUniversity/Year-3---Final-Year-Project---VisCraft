#pragma once

#include <windows.h>
#include <NuiApi.h>
#include <SAM.h>
#include <d3dx9.h>

#include "../helper.h"
#include "CDeformableTemplateModel.h"

class CHand {
private:

	struct HandAreaSamplePoint {
		enum Enum {
			Left,
			Right,
			Top,
			Bottom
		};
	};

	struct HandState {
		enum Enum {
			OpenHand,
			ClosedFist,
			Noof
		};
	};

private:

	unsigned int									m_frameWidth;									//!< 
	unsigned int									m_frameHeight;									//!<

	SAM::TVector<float, 2>							m_palm;											//!<
	SAM::TVector<unsigned int, 4>					m_handArea;										//!< 

	CDeformableTemplateModel						*m_handStateDTM[HandState::Noof];				//!< 	

	RGBQUAD											*m_edgeTempBuffer;								//!< 

private:

													//! Try to sample the data down to a smaller area,
													//! Where the hand could be located
	bool											SampleToHandArea(
														RGBQUAD *depthData
													);

													//! Draw a green box around the sampled hand area for debugging
	void											DrawHandAreaBounds(
														RGBQUAD *depthData
													);

													//! Detect the edges of the hands and display them in white
	void											DetectHandEdges(
														RGBQUAD *depthData
													);

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