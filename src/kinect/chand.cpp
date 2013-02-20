#include "chand.h"


CHand::CHand() : m_edgeTempBuffer(nullptr)
{
	m_frameWidth = 0;
	m_frameHeight = 0;
}

CHand::~CHand()
{
	SafeArrayDelete(m_edgeTempBuffer);
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

	m_edgeTempBuffer = new RGBQUAD[frameWidth * frameHeight];

	return true;
}

RGBQUAD* CHand::FindFromDepth( 
		RGBQUAD* depthData 
	)
{
	// start with a simple depth cull.
	// we can presume the user will be between two given points
	// The two given points should be controlled by an options value

	static const int FAR_POINT = 85;
	static const int NEAR_POINT = 102;
	static const float SCALER = NEAR_POINT / (NEAR_POINT - FAR_POINT);
	static const int MID_POINT = FAR_POINT + static_cast<int>((NEAR_POINT - FAR_POINT) / 2);

	const unsigned int frameSize = m_frameWidth * m_frameHeight;
	for (unsigned int depthIndex = 0; depthIndex < frameSize; ++depthIndex)
	{
		const int depth = depthData[depthIndex].rgbRed;
		if (depth > FAR_POINT && depth < NEAR_POINT)
		{
			const int distanceFromMidPoint = depth - MID_POINT;

			const int blueColor = distanceFromMidPoint > 0 ? 0 : -distanceFromMidPoint;
			const int greenColor = distanceFromMidPoint < 0 ? 0 : distanceFromMidPoint;

			depthData[depthIndex].rgbBlue = static_cast<BYTE>(blueColor * SCALER);
			depthData[depthIndex].rgbGreen = static_cast<BYTE>(greenColor * SCALER) << 2;
			depthData[depthIndex].rgbRed = 255;
		}
		else
		{
			//depthData[depthIndex].rgbBlue = 0;
			//depthData[depthIndex].rgbGreen = 0;
			depthData[depthIndex].rgbRed = 0;
		}
	}

	// from the clamped data, try and find a bounding box for the hand
	if (!SampleToHandArea(depthData))
		return depthData;

	// From the hand bounding box, perform edge detection
	DetectHandEdges(depthData);

#ifdef _DEBUG
	// Draw the bounds if we are in debug mode
	DrawHandAreaBounds(depthData);
#endif

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
			if (depthData[pixel].rgbRed == 0)
				continue;

			if (xPos > right) right = xPos;
			if (xPos < left) left = xPos;
			if (yPos < top) top = yPos;
		}
	}

	// Set the bottom to be the same distance away from the top, as the box is wide
	bottom = top + static_cast<int>((right - left) * 1.3f);

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

void CHand::DetectHandEdges( 
		RGBQUAD *depthData 
	)
{
	memcpy(m_edgeTempBuffer, depthData, m_frameWidth * m_frameHeight * sizeof(RGBQUAD));

	const unsigned int left = m_handArea[HandAreaSamplePoint::Left];
	const unsigned int right = m_handArea[HandAreaSamplePoint::Right];
	const unsigned int top = m_handArea[HandAreaSamplePoint::Top];
	const unsigned int bottom = m_handArea[HandAreaSamplePoint::Bottom];

	for (unsigned int yPos = top + 1; yPos < bottom - 1; ++yPos)
	{
		for (unsigned int xPos = left + 1; xPos < right - 1; ++xPos)
		{
			const unsigned int pixel = (yPos * m_frameWidth) + xPos;

			SAM::TMatrix<float, 3, 3> G[2];

			G[0][0] = SAM::TVector<float, 3>(1.0f, 2.0f, 1.0f);
			G[0][1] = SAM::TVector<float, 3>(0.0f, 0.0f, 0.0f);
			G[0][2] = SAM::TVector<float, 3>(-1.0f, -2.0f, -1.0f);

			G[1][0] = SAM::TVector<float, 3>(1.0f, 0.0f, -1.0f);
			G[1][1] = SAM::TVector<float, 3>(2.0f, 0.0f, -2.0f);
			G[1][2] = SAM::TVector<float, 3>(1.0f, 0.0f, -1.0f);

			float cnv[2];
			SAM::TMatrix<float, 3, 3> I;
			SAM::TMatrix<float, 3, 3> sample;

			// fetch the 3x3 neighborhood and use the Red color intensity value
			for (int sampleOffsetX = 0; sampleOffsetX < 3; ++sampleOffsetX)
			{
				for (int sampleOffsetY = 0; sampleOffsetY < 3; ++sampleOffsetY)
				{
					unsigned int samplePoint = ((yPos + (sampleOffsetY - 1)) * m_frameWidth) + (xPos + (sampleOffsetX - 1));
					I[sampleOffsetX][sampleOffsetY] = depthData[samplePoint].rgbRed;
				}
			}

			// ignore solid color blocks, as they won't contain an edge
			bool allSame = true;
			float lastColor = I[0][0];
			for (int i=0; i < 3; i++)
			{
				for (int j=0; j < 3; j++)
				{
					if (I[i][j] != lastColor)
					{
						allSame = false;
						break;	
					}
					lastColor = I[i][j];
				}
			}

			if (allSame)
			{
				m_edgeTempBuffer[pixel] = depthData[pixel];
				continue;
			}

			/* calculate the convolution values for all the masks */
			for (int i = 0; i < 2; i++)
			{
				float dp3 = G[i][0].Dot(I[0]) + G[i][1].Dot(I[1]) + G[i][2].Dot(I[2]);
				cnv[i] = dp3 * dp3;
			}

			BYTE color = static_cast<BYTE>(0.5 * sqrt(cnv[0]*cnv[0]+cnv[1]*cnv[1]));

			m_edgeTempBuffer[pixel].rgbRed = 255;
			m_edgeTempBuffer[pixel].rgbGreen = 255;
			m_edgeTempBuffer[pixel].rgbBlue = 255;
		}
	}

	memcpy(depthData, m_edgeTempBuffer, m_frameWidth * m_frameHeight * sizeof(RGBQUAD));
	//memset(m_edgeTempBuffer, NULL, m_frameWidth * m_frameHeight * sizeof(RGBQUAD));
}
