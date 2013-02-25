#pragma once

#include <Windows.h>
#include <SAM.h>
#include <vector>

class CDeformableTemplateModel {
private:

	struct SamplePoint {
		SAM::TVector<float, 2>		position;
		SAM::TVector<BYTE, 3>		color;
		unsigned int				tolerance;
	};

private:

	SamplePoint															m_center;
	std::vector<SamplePoint>											m_points;

																		//! Test the DTM against a buffer of data
	bool																TestPoint(
																			RGBQUAD *data,
																			const unsigned int xOffset,
																			const unsigned int yOffset
																		);

public:
																		//! Class constructor
																		CDeformableTemplateModel();

																		//! Class destructor
																		~CDeformableTemplateModel();

																		//! Set the center point of the DTM
	void																SetCenterPoint(
																			SAM::TVector<float, 2> position,
																			SAM::TVector<BYTE, 3> color,
																			unsigned int tolerance
																		);

																		//! Add a sample point to the DTM
	void																AddPoint(
																			SAM::TVector<float, 2> position,
																			SAM::TVector<BYTE, 3> color,
																			unsigned int tolerance
																		);

																		//! Test the DTM against a buffer of data
	bool																Test(
																			RGBQUAD *data,
																			const unsigned int bufferWidth,
																			const unsigned int bufferHeight
																		);
};