#pragma once

#include <windows.h>
#include <NuiApi.h>

#include <propsys.h>
#include <wmcodecdsp.h>
#include <uuids.h>

#include <sapi.h>
#include <sphelper.h>

#include "DrawDevice.h"
#include "ImageRenderer.h"

#include "KinectAudioStream.h"
#include "CAudioProcessor.h"
#include "chand.h"

#include "avi_utils.h"
//#include <vld.h>

#include "../CGui.h"

#pragma comment(lib, "Kinect10.lib")

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

class CKinect {

private:

	HWND										m_hwndDepth;									//!< 
	ID2D1Factory								*m_D2DFactory;							//!< 
	DrawDevice									*m_drawDepth;							//!< 
	ImageRenderer								*m_drawColor;							//!< 

	INuiSensor									*m_nuiSensor;							//!< 
	BSTR										m_kinectID;								//!< 

	HANDLE										m_nuiProcess;							//!< 

	HANDLE										m_nuiProcessStop;						//!< 
	HANDLE										m_nextDepthFrameEvent;					//!< 
	HANDLE										m_hSpeechEvent;							//!< 
	HANDLE										m_nextColorFrameEvent;					//!< 

	HANDLE										m_depthStreamHandle;					//!< 
	HANDLE										m_colorStreamHandle;					//!< 

	RGBQUAD										m_rgbWk[640*480];						//!< 

	CHand										*m_hand;								//!< 

	KinectAudioStream							*m_pKinectAudioStream;					//!< Audio stream captured from Kinect.
	ISpStream									*m_pSpeechStream;						//!< Stream given to speech recognition engine
	ISpRecognizer								*m_pSpeechRecognizer;					//!< Speech recognizer
	ISpRecoContext								*m_pSpeechContext;						//!< Speech recognizer context
	ISpRecoGrammar								*m_pSpeechGrammar;						//!< Speech grammar
	CAudioProcessor								*m_audioCommandProcessor;				//!< Process audio scemantics

	ISpVoice									*m_voice;

	bool										m_isRunning;							//!< The running state of the processing thread

	clock_t										m_lastScreenshot;						//!< 
	std::vector<std::string>					m_screenshots;				

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
													HINSTANCE hInstance,				//!< 
													CGui *gui							//!< 
												);

												//! Destroy the kinect
	void										Destroy();

												//! 
	void										Nui_GotDepthAlert();

												//! 
	void										Nui_GotColorAlert();

												//!
	RGBQUAD										Nui_ShortToQuad_Depth( 
													USHORT s 
												);

												//! 
	HRESULT										StartSpeechRecognition();

												//! 
	HRESULT										InitializeAudioStream();

												//! 
	HRESULT										CreateSpeechRecognizer();

												//! 
	HRESULT										LoadSpeechGrammar();

												//! 
	void										ProcessSpeech();

												//! 
	const D3DXVECTOR2							GetHandPosition();

												//! Get the current hand state
	HandState::Enum								GetHandState() const;

												//!
	void										ResetHandPosition()
												{
													m_hand->ResetHandPosition();
												}
};