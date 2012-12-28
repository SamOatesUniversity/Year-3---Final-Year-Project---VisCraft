#include "CKinect.h"


CKinect::CKinect() :
	m_hwnd(NULL),
	m_kinectID(NULL),
	m_nextDepthFrameEvent(NULL),
	m_depthStreamHandle(NULL)
{
	m_nuiSensor = nullptr;
	m_drawDepth = nullptr;
	m_D2DFactory = nullptr;
}

CKinect::~CKinect()
{

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
		240, 240,
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
	if (!m_drawDepth->Initialize(m_hwnd, m_D2DFactory, 320, 240, 320 * 4))
		return false;

	const HRESULT initResult = m_nuiSensor->NuiInitialize(NUI_INITIALIZE_FLAG_USES_DEPTH |  NUI_INITIALIZE_FLAG_USES_COLOR);
	if (FAILED(initResult))
		return false;

	m_nextDepthFrameEvent = CreateEvent( NULL, TRUE, FALSE, NULL );

	const HRESULT depthStreeam = m_nuiSensor->NuiImageStreamOpen(
		NUI_IMAGE_TYPE_DEPTH,
		NUI_IMAGE_RESOLUTION_320x240,
		0,
		2,
		m_nextDepthFrameEvent,
		&m_depthStreamHandle
	);

	if (FAILED(depthStreeam))
		return false;

	//

	ShowWindow(m_hwnd, SW_SHOW);

	return true;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	return DefWindowProc(hwnd, msg, wParam, lParam);
}