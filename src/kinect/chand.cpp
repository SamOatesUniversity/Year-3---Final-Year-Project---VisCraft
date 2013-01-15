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
		else
		{
			depthData[i].rgbBlue = 0;
			depthData[i].rgbGreen = 0;
			depthData[i].rgbRed = 0;
		}
	}

	// from the clamped data, try and find a bounding box for the hand

	unsigned int left = m_frameWidth;
	unsigned int right = 0;

	unsigned int bottom = 0;
	unsigned int top = m_frameHeight;

	for (unsigned int yPos = 0; yPos < m_frameHeight; ++yPos)
	{
		for (unsigned int xPos = 0; xPos < m_frameWidth; ++xPos)
		{
			const unsigned int pixel = (yPos * m_frameWidth) + xPos;
			if (depthData[pixel].rgbRed == 0)
				continue;

			if (xPos > right) right = xPos;
			if (xPos < left) left = xPos;
			if (yPos < top) top = yPos;
		}
	}

	bottom = top + (right - left);

	if (left >= right) return depthData;
	if (top >= bottom) return depthData;

	if (bottom > m_frameHeight) return depthData;
	if (top < 0) return depthData;
	if (right > m_frameWidth) return depthData;
	if (left < 0) return depthData;

	// draw the bounds
	static const int thickness = 2;

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

	// From the hand bounding box, try and find the center of the palm of the hand


	return depthData;
}

