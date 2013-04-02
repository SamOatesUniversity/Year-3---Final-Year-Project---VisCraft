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
		unsigned int width,
		unsigned int height,
		SAM::TVector<unsigned int, 4> &area
	)
{
	if (m_points.empty())
		return false;

	int handWidth = 0; //area[HandAreaSamplePoint::Right] - area[HandAreaSamplePoint::Left];
	int handHeight = 0; //area[HandAreaSamplePoint::Bottom] - area[HandAreaSamplePoint::Top];

	for (int xoffset = 0; xoffset < handWidth; ++xoffset)
	{
		for (int yoffset = 0; yoffset < handHeight; ++yoffset)
		{
			// Draw the DTM to see if the data is somewhat nice
			SAM::TVector<float, 2> position = m_center.position;

			const int left = static_cast<int>(area[HandAreaSamplePoint::Left] + position.X()) + xoffset;
			const int top = static_cast<int>(area[HandAreaSamplePoint::Top] + position.Y()) + yoffset;

			DrawBox(data, width, height, left, top, 10);

			for (unsigned int pointIndex = 0; pointIndex < m_points.size(); ++ pointIndex)
			{
				SAM::TVector<float, 2> p = m_points[pointIndex].position;
				DrawBox(data, width, height, static_cast<int>(left + p.X()), static_cast<int>(top + p.Y()), 6);
			}
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
	return false;
}

void CDeformableTemplateModel::DrawBox( 
		RGBQUAD *data,
		unsigned int w,
		unsigned int h,
		unsigned int x, 
		unsigned int y, 
		unsigned int t 
	)
{
	int halfSize = static_cast<unsigned int>(t * 0.5f);

	for (int xo = -halfSize; xo <= halfSize; ++xo)
	{
		for (int yo = -halfSize; yo <= halfSize; ++yo)
		{
			const unsigned int pixel = ((y + yo) * w) + (x + xo);
			if (pixel >= (w * h)) continue;
			data[pixel].rgbGreen = 255;
			data[pixel].rgbBlue = 255;
		}
	}
	
}
