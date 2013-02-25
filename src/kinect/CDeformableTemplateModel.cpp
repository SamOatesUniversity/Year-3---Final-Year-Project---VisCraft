#include "CDeformableTemplateModel.h"

CDeformableTemplateModel::CDeformableTemplateModel()
{

}

CDeformableTemplateModel::~CDeformableTemplateModel()
{

}

void CDeformableTemplateModel::SetCenterPoint( 
		SAM::TVector<float, 2> position, 
		SAM::TVector<BYTE, 3> color, 
		unsigned int tolerance 
	)
{
	m_center.color = color;
	m_center.position = position;
	m_center.tolerance = tolerance;
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

bool CDeformableTemplateModel::Test( 
		RGBQUAD *data,
		const unsigned int bufferWidth,
		const unsigned int bufferHeight
	)
{
	if (m_points.empty())
		return false;

	// find a valid center point
	for (unsigned int yOffset = 0; yOffset < bufferHeight; ++yOffset)
	{
		for (unsigned int xOffset = 0; xOffset < bufferWidth; ++xOffset)
		{
			if (TestPoint(data, xOffset, yOffset))
				return true;
		}
	}

	return false;
}

bool CDeformableTemplateModel::TestPoint( 
		RGBQUAD *data, 
		const unsigned int xOffset, 
		const unsigned int yOffset 
	)
{
	SAM::TVector<float, 2> transformedCenter = m_center.position + SAM::TVector<float, 2>(static_cast<float>(xOffset), static_cast<float>(yOffset));
	//RGBQUAD *pixel = GetPixelAt(data, transformedCenter.X(), transformedCenter.Y());
	//if (!ComparePixelColor(pixel, m_center))
	//	return false;

	return false;
}
