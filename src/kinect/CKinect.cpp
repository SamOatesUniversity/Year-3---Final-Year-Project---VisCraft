#include "CKinect.h"

static const int g_IntensityShiftByPlayerR[] = { 1, 2, 0, 2, 0, 0, 2, 0 };
static const int g_IntensityShiftByPlayerG[] = { 1, 2, 2, 0, 2, 0, 0, 1 };
static const int g_IntensityShiftByPlayerB[] = { 1, 0, 2, 2, 0, 2, 0, 2 };

CKinect::CKinect() :
	m_hwnd(NULL),
	m_kinectID(NULL),
	m_nextDepthFrameEvent(NULL),
	m_depthStreamHandle(NULL),
	m_nuiProcess(NULL),
	m_nuiProcessStop(NULL)
{
	m_nuiSensor = nullptr;
	m_drawDepth = nullptr;
	m_D2DFactory = nullptr;
	m_hand = nullptr;
}

CKinect::~CKinect()
{
	SafeDelete(m_drawDepth);
	SafeRelease(m_nuiSensor);
	SafeDelete(m_hand);
}

const bool CKinect::Create( 
		HWND parent,									//!< Parent window of the kinect debug window
		HINSTANCE hInstance								//!< 
	)
{
	WNDCLASSEX wc;
	wc.cbSize        = sizeof(WNDCLASSEX);
	wc.style         = 0;
	wc.lpfnWndProc   = WndProc;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = hInstance;
	wc.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
	wc.lpszMenuName  = NULL;
	wc.lpszClassName = "KinectPreviewClass";
	wc.hIconSm       = LoadIcon(NULL, IDI_APPLICATION);

	if (!RegisterClassEx(&wc))
		return false;

	m_hwnd = CreateWindowEx(
		WS_EX_CLIENTEDGE,
		"KinectPreviewClass",
		"Kinect Preview",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, 
		640, 480,
		parent, NULL, hInstance, NULL
	);

	if (m_hwnd == NULL)
		return false;

	// setup the kinect

	const HRESULT createResult = NuiCreateSensorByIndex(0, &m_nuiSensor);
	if (FAILED(createResult))
		return false;

	m_kinectID = m_nuiSensor->NuiDeviceConnectionId();
	D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &m_D2DFactory);

	m_drawDepth = new DrawDevice();
	if (!m_drawDepth->Initialize(m_hwnd, m_D2DFactory, 640, 480, 640 * 4))
		return false;

	const HRESULT initResult = m_nuiSensor->NuiInitialize(NUI_INITIALIZE_FLAG_USES_DEPTH);
	if (FAILED(initResult))
		return false;

	m_nextDepthFrameEvent = CreateEvent( NULL, TRUE, FALSE, NULL );

	const HRESULT depthStreeam = m_nuiSensor->NuiImageStreamOpen(
		NUI_IMAGE_TYPE_DEPTH,
		NUI_IMAGE_RESOLUTION_640x480,
		0,
		2,
		m_nextDepthFrameEvent,
		&m_depthStreamHandle
	);

	if (FAILED(depthStreeam))
		return false;

	m_nuiProcessStop = CreateEvent( NULL, FALSE, FALSE, NULL );
	m_nuiProcess = CreateThread( NULL, 0, Nui_ProcessThread, this, 0, NULL );
	//

	m_hand = new CHand();
	m_hand->Create(640, 480);

	ShowWindow(m_hwnd, SW_SHOW);

	return true;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	return DefWindowProc(hwnd, msg, wParam, lParam);
}

DWORD WINAPI CKinect::Nui_ProcessThread(LPVOID param)
{
	CKinect *const kinect = reinterpret_cast<CKinect*>(param);
	return kinect->Nui_ProcessThread();
}

DWORD WINAPI CKinect::Nui_ProcessThread()
{
	// Main thread loop
	bool continueProcessing = true;
	while ( continueProcessing )
	{
		

		// Wait for any of the events to be signaled
		static const int numEvents = 2;
		HANDLE hEvents[numEvents] = { m_nuiProcessStop, m_nextDepthFrameEvent };

		const int eventId = WaitForMultipleObjects( numEvents, hEvents, FALSE, 100 );

		// Process signal events
		switch (eventId)
		{
		case WAIT_TIMEOUT:
			continue;

		case WAIT_FAILED:
			continue;

		// If the stop event, stop looping and exit
		case WAIT_OBJECT_0:
			continueProcessing = false;
			continue;

		// Depth event
		case WAIT_OBJECT_0 + 1:
			Nui_GotDepthAlert();
			continue;
		}
	}

	return 0;
}

void CKinect::Nui_GotDepthAlert( )
{
	NUI_IMAGE_FRAME imageFrame;

	HRESULT hr = m_nuiSensor->NuiImageStreamGetNextFrame(
		m_depthStreamHandle,
		0,
		&imageFrame 
	);

	if (FAILED(hr))
		return;

	INuiFrameTexture *pTexture = imageFrame.pFrameTexture;
	NUI_LOCKED_RECT LockedRect;
	pTexture->LockRect( 0, &LockedRect, NULL, 0 );
	if (LockedRect.Pitch != NULL)
	{
		DWORD frameWidth, frameHeight;

		NuiImageResolutionToSize( imageFrame.eResolution, frameWidth, frameHeight );

		// draw the bits to the bitmap
		RGBQUAD * rgbrun = m_rgbWk;
		USHORT * pBufferRun = (USHORT *)LockedRect.pBits;

		// end pixel is start + width*height - 1
		USHORT * pBufferEnd = pBufferRun + (frameWidth * frameHeight);

		while ( pBufferRun < pBufferEnd )
		{
			*rgbrun = Nui_ShortToQuad_Depth( *pBufferRun );
			++pBufferRun;
			++rgbrun;
		}
	
		m_hand->FindFromDepth(m_rgbWk);		

		m_drawDepth->Draw( (BYTE*) m_rgbWk, frameWidth * frameHeight * 4 );
	}
	else
	{
		OutputDebugString("Buffer length of received texture is bogus\r\n");
	}

	pTexture->UnlockRect( 0 );

	m_nuiSensor->NuiImageStreamReleaseFrame(m_depthStreamHandle, &imageFrame);
}

RGBQUAD CKinect::Nui_ShortToQuad_Depth( 
		USHORT s 
	)
{
	USHORT RealDepth = NuiDepthPixelToDepth(s);
	USHORT Player    = NuiDepthPixelToPlayerIndex(s);

	// transform 13-bit depth information into an 8-bit intensity appropriate
	// for display (we disregard information in most significant bit)
	BYTE intensity = (BYTE)~(RealDepth >> 4);

	// tint the intensity by dividing by per-player values
	RGBQUAD color;
	color.rgbRed   = intensity >> g_IntensityShiftByPlayerR[Player];
	color.rgbGreen = intensity >> g_IntensityShiftByPlayerG[Player];
	color.rgbBlue  = intensity >> g_IntensityShiftByPlayerB[Player];

	return color;
}