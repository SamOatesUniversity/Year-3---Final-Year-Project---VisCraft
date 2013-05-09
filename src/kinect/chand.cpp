#include "chand.h"
#include "../cviscraft.h"
#include "../cgizmo.h"

CHand::CHand() : m_edgeTempBuffer(nullptr)
{
	m_frameWidth = 0;
	m_frameHeight = 0;
	m_handState = HandState::NotFound;
	m_startClose = 0;
	m_center = D3DXVECTOR2(m_frameWidth * 0.5f, m_frameHeight * 0.5f);
	m_configuratState = ConfigurationState::None;

	for (unsigned int dtmIndex = 0; dtmIndex < HandState::Noof; ++dtmIndex)
	{
			m_handStateDTM[dtmIndex] = nullptr;
	}
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

	m_edgeTempBuffer = new RGBQUAD[frameWidth * frameHeight];

	m_lastPosition = CVisCraft::GetInstance()->GetWindowDimension();
	m_lastPosition.x *= 0.5f;
	m_lastPosition.y *= 0.75f;

	m_configuratState = ConfigurationState::None;

	m_handSize.x = 90;
	m_handSize.y = 30;

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
			depthData[depthIndex].rgbRed = 0;
		}
	}

	// from the clamped data, try and find a bounding box for the hand
	if (!SampleToHandArea(depthData)) {
		m_handState = HandState::NotFound;
		return depthData;
	}

	// From the hand bounding box, perform edge detection
	DetectHandEdges(depthData);

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
	bool valid = true;
	if (left >= right) valid = false;
	if (top >= bottom) valid = false;
	if (bottom > m_frameHeight) valid = false;
	if (top < 0) valid = false;
	if (right > m_frameWidth) valid = false;
	if (left < 0) valid = false;

	if (!valid)
	{
		m_handState = HandState::NotFound;
		return false;
	}

	const int widthOfHand = right - left;
	const int heightOfHand = bottom - top;

	// if the sample are is massive, its probably not a hand... or its a giant.
	if (widthOfHand > m_handSize.x) valid = false;
	if (widthOfHand < m_handSize.y) valid = false;

	if (!valid)
	{
		m_handState = HandState::NotFound;
		return false;
	}

	// Update our stored sample location
	m_handArea.Set(0, 0, 0, 0);
	m_handArea[HandAreaSamplePoint::Left] = left;
	m_handArea[HandAreaSamplePoint::Right] = right;
	m_handArea[HandAreaSamplePoint::Top] = top;
	m_handArea[HandAreaSamplePoint::Bottom] = bottom;
	
	float xPos = left + (widthOfHand * 0.5f);
	float yPos = top + (heightOfHand * 0.5f);

	HandState::Enum oldState = m_handState;

	float differenceToMax = m_handSize.x - widthOfHand;
	float differnceToMin = widthOfHand - m_handSize.y;

	std::stringstream buf;
	buf << "Distance To Max: " << differenceToMax << ", Distance To Min: " << differnceToMin << "\n";

	OutputDebugString(buf.str().c_str());

	m_handState = differenceToMax < differnceToMin ? HandState::OpenHand : HandState::ClosedFist;

	if (oldState != HandState::ClosedFist && m_handState == HandState::ClosedFist)
	{
		if (m_startClose == 0) 
		{
			m_startClose = clock();
			m_handState = HandState::OpenHand;
		}
		else if (static_cast<float>((clock() - m_startClose)/CLOCKS_PER_SEC) > 0.01f)
		{
			m_startClose = 0;
		}
		else
		{
			m_handState = HandState::OpenHand;
		}
	}
	else if (m_handState == HandState::OpenHand)
	{
		m_startClose = 0;
	}

	m_palm = D3DXVECTOR2(xPos, yPos);

	if (oldState == HandState::NotFound || (m_handState == HandState::ClosedFist && oldState == HandState::OpenHand))
	{
		m_center = m_palm;
	}

	if (m_handState != HandState::NotFound)
	{
		CVisCraft::GetInstance()->GetGizmo()->SetInputType(InputType::Kinect); 
	}
		
	return true;
}

void CHand::DrawBox(
		RGBQUAD *depthData, 
		unsigned int x1, 
		unsigned int y1, 
		unsigned int x2, 
		unsigned int y2, 
		unsigned int r, 
		unsigned int g, 
		unsigned int b
	)
{
	for (unsigned int yPos = y1; yPos < y2; ++yPos)
	{
		if (yPos >= m_frameHeight) continue;
		if (yPos <= 0) continue;

		for (unsigned int xPos = x1; xPos < x2; ++xPos)
		{
			if (xPos >= m_frameWidth) continue;
			if (xPos <= 0) continue;
			
			const unsigned int pixel = (yPos * m_frameWidth) + xPos;
			depthData[pixel].rgbRed = r;
			depthData[pixel].rgbGreen = g;
			depthData[pixel].rgbBlue = b;
		}
	}
}

void CHand::DrawHandAreaBounds(
		RGBQUAD *depthData
	)
{
	if (m_handState == HandState::NotFound)
		return;

	static const int thickness = 1;

	const unsigned int left = m_handArea[HandAreaSamplePoint::Left];
	const unsigned int right = m_handArea[HandAreaSamplePoint::Right];
	const unsigned int top = m_handArea[HandAreaSamplePoint::Top];
	const unsigned int bottom = m_handArea[HandAreaSamplePoint::Bottom];

	DrawBox(depthData, left - thickness, top, left + thickness, bottom, 0, m_handState == HandState::OpenHand ? 255 : 0, m_handState == HandState::OpenHand ? 0 : 255);
	DrawBox(depthData, right - thickness, top, right + thickness, bottom, 0, m_handState == HandState::OpenHand ? 255 : 0, m_handState == HandState::OpenHand ? 0 : 255);
	DrawBox(depthData, left, top - thickness, right, top + thickness, 0, m_handState == HandState::OpenHand ? 255 : 0, m_handState == HandState::OpenHand ? 0 : 255);
	DrawBox(depthData, left, bottom - thickness, right, bottom + thickness, 0, m_handState == HandState::OpenHand ? 255 : 0, m_handState == HandState::OpenHand ? 0 : 255);

	DrawBox(
		depthData, 
		static_cast<unsigned int>(m_center.x - 6), 
		static_cast<unsigned int>(m_center.y - 6), 
		static_cast<unsigned int>(m_center.x + 6), 
		static_cast<unsigned int>(m_center.y + 6), 
		255, 255, 0
	);
}

void CHand::DetectHandEdges( 
		RGBQUAD *depthData 
	)
{
	return;

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

			// if the sample data is all the same, return the current color
			if (allSame)
			{
				m_edgeTempBuffer[pixel] = depthData[pixel];
				continue;
			}

			// sample region is most likely an edge
			m_edgeTempBuffer[pixel].rgbRed = 255;
			m_edgeTempBuffer[pixel].rgbGreen = 255;
			m_edgeTempBuffer[pixel].rgbBlue = 255;
		}
	}

	memcpy(depthData, m_edgeTempBuffer, m_frameWidth * m_frameHeight * sizeof(RGBQUAD));
}

void CHand::Release()
{

}

const D3DXVECTOR2 CHand::GetHandPosition()
{
	if (m_handState == HandState::OpenHand || m_handState == HandState::ClosedFist)
	{
		D3DXVECTOR2 differnce = (m_center - m_palm);
		differnce.x = differnce.x * 0.025f;
		differnce.y = differnce.y * 0.025f;

		const float sqrLen = (differnce.x*differnce.x + differnce.y*differnce.y);
		if (sqrLen > 1.0f) {
			differnce.y = differnce.y * 0.5f;
			m_oldPosition = m_lastPosition;
			m_lastPosition = m_lastPosition - differnce;
		}
	}

	return m_lastPosition;
}

HandState::Enum CHand::GetHandState() const
{
	return m_handState;
}
