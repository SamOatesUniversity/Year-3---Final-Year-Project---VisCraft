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

	std::vector<SamplePoint>											m_points;

public:
																		//! Class constructor
																		CDeformableTemplateModel();

																		//! Class destructor
																		~CDeformableTemplateModel();

																		//! Add a sample point to the DTM
	void																AddPoint(
																			SAM::TVector<float, 2> position,
																			SAM::TVector<BYTE, 3> color,
																			unsigned int tolerance
																		);
};