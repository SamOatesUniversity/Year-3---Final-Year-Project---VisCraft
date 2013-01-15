#include "CDeformableTemplateModel.h"

CDeformableTemplateModel::CDeformableTemplateModel()
{

}

CDeformableTemplateModel::~CDeformableTemplateModel()
{

}

void CDeformableTemplateModel::AddPoint(
		SAM::TVector<float, 2> position,
		SAM::TVector<BYTE, 3> color,
		unsigned int tolerance
	)
{
	SamplePoint newSamplePoint;
	newSamplePoint.position = position;
	newSamplePoint.color = color;
	newSamplePoint.tolerance = tolerance;
	m_points.push_back(newSamplePoint);
}