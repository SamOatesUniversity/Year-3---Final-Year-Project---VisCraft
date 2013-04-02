#pragma once

#include <Windows.h>
#include <SAM.h>
#include <vector>

struct HandAreaSamplePoint {
	enum Enum {
		Left,
		Right,
		Top,
		Bottom
	};
};

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
																			unsigned int width,
																			unsigned int height,
																			SAM::TVector<unsigned int, 4> &area
																		);
	
																		//! 
	void																DrawBox(
																			RGBQUAD *data,
																			unsigned int w,
																			unsigned int h,
																			unsigned int x, 
																			unsigned int y, 
																			unsigned int t  
																		);
};