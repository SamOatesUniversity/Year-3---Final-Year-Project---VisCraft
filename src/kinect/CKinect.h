#pragma once

#include <windows.h>
#include <NuiApi.h>
#include "DrawDevice.h"

#pragma comment(lib, "Kinect10.lib")

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

class CKinect {

private:

	HWND										m_hwnd;									//!< 
	ID2D1Factory								*m_D2DFactory;							//!< 
	DrawDevice									*m_drawDepth;							//!< 

	INuiSensor									*m_nuiSensor;							//!< 
	BSTR										m_kinectID;								//!< 

	HANDLE										m_nextDepthFrameEvent;					//!< 
	HANDLE										m_depthStreamHandle;					//!< 

public:
												//! Class constructor
												CKinect();

												//! Class destructor
												~CKinect();

	const bool									Create(
													HWND parent,						//!< Parent window of the kinect debug window
													HINSTANCE hInstance					//!< 
												);
};