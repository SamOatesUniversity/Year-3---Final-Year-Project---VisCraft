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
	m_nuiProcessStop(NULL),
	m_hSpeechEvent(NULL),
	m_isRunning(false)
{
	m_nuiSensor = nullptr;
	m_drawDepth = nullptr;
	m_D2DFactory = nullptr;
	m_hand = nullptr;
	m_audioCommandProcessor = nullptr;
	m_pKinectAudioStream = nullptr;
}

CKinect::~CKinect()
{
	SafeDelete(m_drawDepth);
	SafeRelease(m_nuiSensor);
	SafeReleaseDelete(m_hand);
	SafeDelete(m_audioCommandProcessor);
	SafeDelete(m_pKinectAudioStream);
}

const bool CKinect::Create( 
		HWND parent,									//!< Parent window of the kinect debug window
		HINSTANCE hInstance,							//!< 
		CGui *gui										//!<
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
		WS_EX_TOPMOST,
		"KinectPreviewClass",
		"Kinect Preview",
		WS_POPUP,
		7, 30, 
		160, 120,
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

	const HRESULT initResult = m_nuiSensor->NuiInitialize(NUI_INITIALIZE_FLAG_USES_DEPTH | NUI_INITIALIZE_FLAG_USES_AUDIO);
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

	m_hand = new CHand();
	m_hand->Create(640, 480);
	
	// Audio
	HRESULT hr = InitializeAudioStream();
	if (FAILED(hr))
	{
		return false;
	}

	hr = CreateSpeechRecognizer();
	if (FAILED(hr))
	{
		return false;
	}

	hr = LoadSpeechGrammar();
	if (FAILED(hr))
	{
		return false;
	}

	hr = StartSpeechRecognition();
	if (FAILED(hr))
	{
		return false;
	}

	m_audioCommandProcessor = new CAudioProcessor(gui);

	m_nuiProcessStop = CreateEvent( NULL, FALSE, FALSE, NULL );
	m_nuiProcess = CreateThread( NULL, 0, Nui_ProcessThread, this, 0, NULL );

	ShowWindow(m_hwnd, SW_SHOW);

	return true;
}

HRESULT CKinect::LoadSpeechGrammar()
{
	HRESULT hr = m_pSpeechContext->CreateGrammar(1, &m_pSpeechGrammar);

	if (SUCCEEDED(hr))
	{
		// Populate recognition grammar from file
		LPCWSTR file = L"VisCraft-Phrases.grxml";
		hr = m_pSpeechGrammar->LoadCmdFromFile(file, SPLO_STATIC);
	}

	return hr;
}

HRESULT CKinect::CreateSpeechRecognizer()
{
	ISpObjectToken *pEngineToken = NULL;

	HRESULT hr = CoCreateInstance(CLSID_SpInprocRecognizer, NULL, CLSCTX_INPROC_SERVER, __uuidof(ISpRecognizer), (void**)&m_pSpeechRecognizer);

	if (SUCCEEDED(hr))
	{
		m_pSpeechRecognizer->SetInput(m_pSpeechStream, FALSE);
		hr = SpFindBestToken(SPCAT_RECOGNIZERS, L"Language=409;Kinect=True", NULL, &pEngineToken);

		if (SUCCEEDED(hr))
		{
			m_pSpeechRecognizer->SetRecognizer(pEngineToken);
			hr = m_pSpeechRecognizer->CreateRecoContext(&m_pSpeechContext);
		}
	}

	SafeRelease(pEngineToken);

	return hr;
}

HRESULT CKinect::InitializeAudioStream()
{
	INuiAudioBeam*      pNuiAudioSource = NULL;
	IMediaObject*       pDMO = NULL;
	IPropertyStore*     pPropertyStore = NULL;
	IStream*            pStream = NULL;

	// Get the audio source
	HRESULT hr = m_nuiSensor->NuiGetAudioSource(&pNuiAudioSource);
	if (SUCCEEDED(hr))
	{
		hr = pNuiAudioSource->QueryInterface(IID_IMediaObject, (void**)&pDMO);

		if (SUCCEEDED(hr))
		{
			hr = pNuiAudioSource->QueryInterface(IID_IPropertyStore, (void**)&pPropertyStore);

			// Set AEC-MicArray DMO system mode. This must be set for the DMO to work properly.
			// Possible values are:
			//   SINGLE_CHANNEL_AEC = 0
			//   OPTIBEAM_ARRAY_ONLY = 2
			//   OPTIBEAM_ARRAY_AND_AEC = 4
			//   SINGLE_CHANNEL_NSAGC = 5
			PROPVARIANT pvSysMode;
			PropVariantInit(&pvSysMode);
			pvSysMode.vt = VT_I4;
			pvSysMode.lVal = (LONG)(4); // Use OPTIBEAM_ARRAY_ONLY setting. Set OPTIBEAM_ARRAY_AND_AEC instead if you expect to have sound playing from speakers.
			pPropertyStore->SetValue(MFPKEY_WMAAECMA_SYSTEM_MODE, pvSysMode);
			PropVariantClear(&pvSysMode);

			// Set DMO output format
			WAVEFORMATEX wfxOut = {AudioFormat, AudioChannels, AudioSamplesPerSecond, AudioAverageBytesPerSecond, AudioBlockAlign, AudioBitsPerSample, 0};
			DMO_MEDIA_TYPE mt = {0};
			MoInitMediaType(&mt, sizeof(WAVEFORMATEX));

			mt.majortype = MEDIATYPE_Audio;
			mt.subtype = MEDIASUBTYPE_PCM;
			mt.lSampleSize = 0;
			mt.bFixedSizeSamples = TRUE;
			mt.bTemporalCompression = FALSE;
			mt.formattype = FORMAT_WaveFormatEx;	
			memcpy(mt.pbFormat, &wfxOut, sizeof(WAVEFORMATEX));

			hr = pDMO->SetOutputType(0, &mt, 0);

			if (SUCCEEDED(hr))
			{
				m_pKinectAudioStream = new KinectAudioStream(pDMO);

				hr = m_pKinectAudioStream->QueryInterface(IID_IStream, (void**)&pStream);

				if (SUCCEEDED(hr))
				{
					hr = CoCreateInstance(CLSID_SpStream, NULL, CLSCTX_INPROC_SERVER, __uuidof(ISpStream), (void**)&m_pSpeechStream);

					if (SUCCEEDED(hr))
					{
						hr = m_pSpeechStream->SetBaseStream(pStream, SPDFID_WaveFormatEx, &wfxOut);
					}
				}
			}

			MoFreeMediaType(&mt);
		}
	}

	SafeRelease(pStream);
	SafeRelease(pPropertyStore);
	SafeRelease(pDMO);
	SafeRelease(pNuiAudioSource);

	return hr;
}

HRESULT CKinect::StartSpeechRecognition()
{
	HRESULT hr = m_pKinectAudioStream->StartCapture();

	if (SUCCEEDED(hr))
	{
		// Specify that all top level rules in grammar are now active
		hr = m_pSpeechGrammar->SetRuleState(NULL, NULL, SPRS_ACTIVE);

		// Specify that engine should always be reading audio
		hr = m_pSpeechRecognizer->SetRecoState(SPRST_ACTIVE_ALWAYS);

		// Specify that we're only interested in receiving recognition events
		hr = m_pSpeechContext->SetInterest(SPFEI(SPEI_RECOGNITION), SPFEI(SPEI_RECOGNITION));

		// Ensure that engine is recognizing speech and not in paused state
		m_pSpeechContext->Pause(0);
		hr = m_pSpeechContext->Resume(0);
		if (SUCCEEDED(hr))
		{
			m_hSpeechEvent = m_pSpeechContext->GetNotifyEventHandle();
		}
	}

	return hr;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	return DefWindowProc(hwnd, msg, wParam, lParam);
}

DWORD WINAPI CKinect::Nui_ProcessThread(LPVOID param)
{
	CKinect *kinect = reinterpret_cast<CKinect*>(param);
	DWORD result = kinect->Nui_ProcessThread();
	return result;
}

DWORD WINAPI CKinect::Nui_ProcessThread()
{
	// Main thread loop
	m_isRunning = true;
	while (m_isRunning)
	{
		// Wait for any of the events to be signaled
		static const int numEvents = 3;
		HANDLE hEvents[numEvents] = { m_nuiProcessStop, m_nextDepthFrameEvent, m_hSpeechEvent };

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
			m_isRunning = false;
			continue;

		// Depth event
		case WAIT_OBJECT_0 + 1:
			Nui_GotDepthAlert();
			continue;

		// Depth event
		case WAIT_OBJECT_0 + 2:
			ProcessSpeech();
			continue;
		}
	}

	return 0;
}

void CKinect::ProcessSpeech()
{
	static const float ConfidenceThreshold = 0.15f;

	SPEVENT curEvent;
	ULONG fetched = 0;
	HRESULT hr = S_OK;

	m_pSpeechContext->GetEvents(1, &curEvent, &fetched);

	while (fetched > 0)
	{
		switch (curEvent.eEventId)
		{
		case SPEI_RECOGNITION:
			if (SPET_LPARAM_IS_OBJECT == curEvent.elParamType)
			{
				// this is an ISpRecoResult
				ISpRecoResult* result = reinterpret_cast<ISpRecoResult*>(curEvent.lParam);
				SPPHRASE* pPhrase = NULL;

				hr = result->GetPhrase(&pPhrase);
				if (SUCCEEDED(hr))
				{
					if ((pPhrase->pProperties != NULL) && (pPhrase->pProperties->pFirstChild != NULL))
					{
						const SPPHRASEPROPERTY* pSemanticTag = pPhrase->pProperties->pFirstChild;
						if (pSemanticTag->SREngineConfidence > ConfidenceThreshold)
						{
							m_audioCommandProcessor->Process(pSemanticTag);
						}
					}
					::CoTaskMemFree(pPhrase);
				}
			}
			break;
		}

		m_pSpeechContext->GetEvents(1, &curEvent, &fetched);
	}

	return;
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

void CKinect::Destroy()
{
	SetEvent(m_nuiProcessStop);
	ShowWindow(m_hwnd, SW_HIDE);
	do {
		Sleep(10);
	} while (m_isRunning);

	if (m_pKinectAudioStream != nullptr) 
	{
		m_pKinectAudioStream->StopCapture();
	}
}

const D3DXVECTOR2 CKinect::GetHandPosition()
{
	return m_hand->GetHandPosition();
}

HandState::Enum CKinect::GetHandState() const
{
	return m_hand->GetHandState();
}
