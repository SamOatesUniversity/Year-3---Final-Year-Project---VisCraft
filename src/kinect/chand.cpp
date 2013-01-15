#include "chand.h"


CHand::CHand()
{
	m_frameWidth = 0;
	m_frameHeight = 0;
}

CHand::~CHand()
{

}

bool CHand::Create( 
		int frameWidth, 
		int frameHeight 
	)
{
	m_frameWidth = frameWidth;
	m_frameHeight = frameHeight;

	m_handStateDTM[HandState::OpenHand] = new CDeformableTemplateModel();

	m_handStateDTM[HandState::ClosedFist] = new CDeformableTemplateModel();


	SAM::TVector<float, 2> position;
	SAM::TVector<BYTE, 3> color;
	
	position.Set(16, 16);
	color.Set(255, 0, 0);
	m_handStateDTM[HandState::ClosedFist]->AddPoint(position, color, 0); // center of the hand

	position.Set(10, 16);
	color.Set(255, 0, 0);
	m_handStateDTM[HandState::ClosedFist]->AddPoint(position, color, 0); // left of the hand (in hand)

	position.Set(22, 16);
	color.Set(255, 0, 0);
	m_handStateDTM[HandState::ClosedFist]->AddPoint(position, color, 0); // right of the hand (in hand)

	position.Set(16, 10);
	color.Set(255, 0, 0);
	m_handStateDTM[HandState::ClosedFist]->AddPoint(position, color, 0); // above of the hand (in hand)

	position.Set(4, 16);
	color.Set(0, 0, 0);
	m_handStateDTM[HandState::ClosedFist]->AddPoint(position, color, 0); // left of the hand (out of hand)

	position.Set(28, 16);
	color.Set(0, 0, 0);
	m_handStateDTM[HandState::ClosedFist]->AddPoint(position, color, 0); // right of the hand (out of hand)

	position.Set(16, 4);
	color.Set(0, 0, 0);
	m_handStateDTM[HandState::ClosedFist]->AddPoint(position, color, 0); // above of the hand (out of hand)

	return true;
}

RGBQUAD* CHand::FindFromDepth( 
		RGBQUAD* depthData 
	)
{
	// start with a simple depth cull.
	// we can presume the user will be between two given points
	// The two given points should be controlled by an options value

	static const int FAR_POINT = 95;
	static const int NEAR_POINT = 127;
	static const float SCALER = NEAR_POINT / (NEAR_POINT - FAR_POINT);

	for (unsigned int i = 0; i < m_frameWidth * m_frameHeight; ++i)
	{
		if (depthData[i].rgbRed > FAR_POINT && depthData[i].rgbRed < NEAR_POINT)
		{
			depthData[i].rgbBlue = 0;
			depthData[i].rgbGreen = 0;
			depthData[i].rgbRed = 255;
		}
	}

	// from the clamped data, try and find a bounding box for the hand
	if (!SampleToHandArea(depthData))
		return depthData;

#ifdef _DEBUG
	// Draw the bounds if we are in debug mode
	DrawHandAreaBounds(depthData);
#endif

	// From the hand bounding box, try and find the center of the palm of the hand




	return depthData;
}

bool CHand::SampleToHandArea(
		RGBQUAD *depthData
	)
{
	// Start at the opposite extremities
	unsigned int right = 0;
	unsigned int bottom = 0;
	unsigned int left = m_frameWidth;
	unsigned int top = m_frameHeight;

	// spin through all pixel points, moving the bounds accordingly
	for (unsigned int yPos = 0; yPos < m_frameHeight; ++yPos)
	{
		for (unsigned int xPos = 0; xPos < m_frameWidth; ++xPos)
		{
			const unsigned int pixel = (yPos * m_frameWidth) + xPos;
			if (depthData[pixel].rgbRed == depthData[pixel].rgbBlue)
				continue;

			if (xPos > right) right = xPos;
			if (xPos < left) left = xPos;
			if (yPos < top) top = yPos;
		}
	}

	// Set the bottom to be the same distance away from the top, as the box is wide
	bottom = top + (right - left);

	// Make sure we have valid extremities
	if (left >= right) return false;
	if (top >= bottom) return false;
	if (bottom > m_frameHeight) return false;
	if (top < 0) return false;
	if (right > m_frameWidth) return false;
	if (left < 0) return false;

	// Update our stored sample location
	m_handArea.Set(0, 0, 0, 0);
	m_handArea[HandAreaSamplePoint::Left] = left;
	m_handArea[HandAreaSamplePoint::Right] = right;
	m_handArea[HandAreaSamplePoint::Top] = top;
	m_handArea[HandAreaSamplePoint::Bottom] = bottom;

	return true;
}

void CHand::DrawHandAreaBounds(
		RGBQUAD *depthData
	)
{
	static const int thickness = 1;

	const unsigned int left = m_handArea[HandAreaSamplePoint::Left];
	const unsigned int right = m_handArea[HandAreaSamplePoint::Right];
	const unsigned int top = m_handArea[HandAreaSamplePoint::Top];
	const unsigned int bottom = m_handArea[HandAreaSamplePoint::Bottom];

	for (unsigned int yPos = top; yPos < bottom; ++yPos)
	{
		if (yPos >= m_frameHeight) continue;
		if (yPos <= 0) continue;

		for (unsigned int xPos = left - thickness; xPos < left + thickness; ++xPos)
		{
			if (xPos >= m_frameWidth) continue;
			if (xPos <= 0) continue;
			
			const unsigned int pixel = (yPos * m_frameWidth) + xPos;
			depthData[pixel].rgbGreen = 255;
		}
	}

	for (unsigned int yPos = top; yPos < bottom; ++yPos)
	{
		if (yPos >= m_frameHeight) continue;
		if (yPos <= 0) continue;

		for (unsigned int xPos = right - thickness; xPos < right + thickness; ++xPos)
		{
			if (xPos >= m_frameWidth) continue;
			if (xPos <= 0) continue;

			const unsigned int pixel = (yPos * m_frameWidth) + xPos;
			depthData[pixel].rgbGreen = 255;
		}
	}

	for (unsigned int xPos = left; xPos < right; ++xPos)
	{
		if (xPos >= m_frameWidth) continue;
		if (xPos <= 0) continue;

		for (unsigned int yPos = top - thickness; yPos < top + thickness; ++yPos)
		{
			if (yPos >= m_frameHeight) continue;
			if (yPos <= 0) continue;

			const unsigned int pixel = (yPos * m_frameWidth) + xPos;
			depthData[pixel].rgbGreen = 255;
		}
	}

	for (unsigned int xPos = left; xPos < right; ++xPos)
	{
		if (xPos >= m_frameWidth) continue;
		if (xPos <= 0) continue;

		for (unsigned int yPos = bottom - thickness; yPos < bottom + thickness; ++yPos)
		{
			if (yPos >= m_frameHeight) continue;
			if (yPos <= 0) continue;

			const unsigned int pixel = (yPos * m_frameWidth) + xPos;
			depthData[pixel].rgbGreen = 255;
		}
	}
}