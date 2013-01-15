#pragma once

#include <windows.h>
#include <NuiApi.h>
#include "DrawDevice.h"
#include "chand.h"

#pragma comment(lib, "Kinect10.lib")

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

class CKinect {

private:

	HWND										m_hwnd;									//!< 
	ID2D1Factory								*m_D2DFactory;							//!< 
	DrawDevice									*m_drawDepth;							//!< 

	INuiSensor									*m_nuiSensor;							//!< 
	BSTR										m_kinectID;								//!< 

	HANDLE										m_nuiProcess;							//!< 

	HANDLE										m_nuiProcessStop;						//!< 
	HANDLE										m_nextDepthFrameEvent;					//!< 

	HANDLE										m_depthStreamHandle;					//!< 

	RGBQUAD										m_rgbWk[640*480];						//!< 

	CHand										*m_hand;								//!< 

private:

												//! 
	static DWORD WINAPI							Nui_ProcessThread(
													LPVOID pParam						//!< 
												);

												//! 
	DWORD WINAPI								Nui_ProcessThread();

public:
												//! Class constructor
												CKinect();

												//! Class destructor
												~CKinect();

	const bool									Create(
													HWND parent,						//!< Parent window of the kinect debug window
													HINSTANCE hInstance					//!< 
												);

												//! 
	void										Nui_GotDepthAlert();

												//!
	RGBQUAD										Nui_ShortToQuad_Depth( 
													USHORT s 
												);
};