
#include "cviscraft.h"

CVisCraft* CVisCraft::m_instance = nullptr;

/*!
 * \brief The constructor
 */
CVisCraft::CVisCraft() : 
	m_applicationName(NULL),
	m_hinstance(NULL),
	m_hwnd(NULL),
	m_splashhwnd(NULL),
	m_spashBitmap(NULL)
{
	m_input = nullptr;
	m_renderer = nullptr;
	m_camera = nullptr;
	m_terrain = nullptr;
	m_shader = nullptr;
	m_gizmo = nullptr;
	m_kinect = nullptr;
	m_gui = nullptr;

	m_running = false;
}

/*!
 * \brief The destructor
 */
CVisCraft::~CVisCraft() 
{
	VISASSERT(!m_input || !m_renderer || !m_camera || !m_terrain || !m_shader, "Release has not been called before the deletion of a VisCraft object.\nTHIS WILL LEAK MEMORY!");
}

/*!
 * \brief Initializes the application
 * \return true on success, false if a problem was encountered.
 */
const bool CVisCraft::Create()
{
	m_instance = this;

	// Initialize the width and height of the screen to zero before sending the variables into the function.
	m_screenWidth = 0;
	m_screenHeight = 0;

	// Initialize the windows api.
	if (!CreateWindowInternal(m_screenWidth, m_screenHeight))
	{
		return false;
	}

	// Create the d3d renderer class
	m_renderer = new CRenderer();
	if (!m_renderer->Create(m_hwnd, m_screenWidth, m_screenHeight))
	{
		VISASSERT(false, "Failed to create renderer");
		return false;
	}

	m_camera = new CCamera();
	m_camera->SetPosition(50.0f, 25.0f, -7.0f);

	m_terrain = new CTerrain();
	if (!m_terrain->Create(m_renderer))
	{
		VISASSERT(false, "Failed to create terrain class");
		return false;
	}

	//m_terrain->LoadHeightMap("heightmap.bmp");

	m_shader = new CShader();
	if (!m_shader->Create(m_renderer))
	{
		VISASSERT(false, "Failed to create/load shaders");
		return false;
	}

	m_gizmo = new CGizmo();
	if (!m_gizmo->Create(m_renderer))
	{
		VISASSERT(false, "Failed to create gizmo");
		return false;
	}

	// Create the gui
	m_gui = new CGui();
	if (!m_gui->Create(m_renderer))
	{
		VISASSERT(false, "Failed to create gui");
		return false;
	}

	m_kinect = new CKinect();
	if (!m_kinect->Create(m_hwnd, m_hinstance, m_gui))
	{
		//VISASSERT(false, "Failed to create the kinect interface class");
		//return false;
		SafeDelete(m_kinect);
		::MessageBox(NULL, "Failed to Initialize the Kinect!", "VisCraft", MB_OK);
	}

	// Create the input handling class
	m_input = new CInput();
	if (!m_input->Create(m_hinstance, m_hwnd, m_screenWidth, m_screenHeight))
	{
		VISASSERT(false, "Failed to create input class");
		return false;
	}

	// Bring the window up on the screen and set it as main focus.
	ShowWindow(m_hwnd, SW_SHOWMAXIMIZED);
	SetForegroundWindow(m_hwnd);
	SetFocus(m_hwnd);
	DestroyWindow(m_splashhwnd);
	
	::ShowCursor(FALSE);

	m_running = true;

	return true;
}

/*!
 * \brief Create the window we will render on
 */
bool CVisCraft::CreateWindowInternal( 
		int& width, 
		int& height 
)
{
	// Get the instance of this application.
	m_hinstance = GetModuleHandle(NULL);

	// Give the application a name.
	m_applicationName = "VisCraft";

	// Create the splash screen loading banner, as initializing d3d11 + kinect + audio takes some time
	CreateSplashScreen();

	// Setup the windows class with default settings.
	WNDCLASSEX wc;
	wc.style         = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc   = WindowsProcedure;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = m_hinstance;
	wc.hIcon		 = LoadIcon(NULL, IDI_WINLOGO);
	wc.hIconSm       = wc.hIcon;
	wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName  = NULL;
	wc.lpszClassName = m_applicationName;
	wc.cbSize        = sizeof(WNDCLASSEX);

	// Register the window class.
	RegisterClassEx(&wc);

	// Determine the resolution of the clients desktop screen.
	width  = GetSystemMetrics(SM_CXSCREEN);
	height = GetSystemMetrics(SM_CYSCREEN);

	// Set the position of the window to the top left corner.
	int posX = 0, posY = 0;

	// Create the window with the screen settings and get the handle to it.
	m_hwnd = CreateWindowEx(
		WS_EX_APPWINDOW, 
		m_applicationName, 
		m_applicationName, 
		WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_OVERLAPPEDWINDOW,
		posX, posY, 
		width, height, 
		NULL, NULL, m_hinstance, NULL);

	if (m_hwnd == nullptr)
	{
		VISASSERT(false, "Failed to create internal render window.");
		return false;
	}

	return true;
}

/*!
 * \brief The main run loop
 */
void CVisCraft::Run()
{
	// Initialize the message structure.
	MSG msg;
	ZeroMemory(&msg, sizeof(MSG));

	// Loop until there is a quit message from the window or the user.
	while (m_running)
	{
		// Handle the windows messages.
		if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		// If windows signals to end the application then exit out.
		if(msg.message == WM_QUIT || !Update())
		{
			return;
		}
	}
}

/*!
 * \brief Destroy anything we created
 */
void CVisCraft::Release()
{
	ShowWindow(m_hwnd, SW_HIDE);
	if (m_kinect != nullptr) 
	{
		m_kinect->Destroy();
	}
	
	SafeReleaseDelete(m_input);
	SafeReleaseDelete(m_renderer);
	SafeDelete(m_camera);
	SafeReleaseDelete(m_terrain);
	SafeReleaseDelete(m_shader);
	SafeDelete(m_gizmo);
	SafeDelete(m_kinect);
	SafeReleaseDelete(m_gui);

	// Remove the window.
	DestroyWindow(m_hwnd);
	DestroyWindow(m_splashhwnd);

	// Remove the application instance.
	UnregisterClass(m_applicationName, m_hinstance);
	m_hinstance = NULL;

	// Release the pointer to this class.
	m_instance = nullptr;
}

/*!
 * \brief Render the current state of the world scene to the window
 * \return true if successful, false otherwise.
 */
bool CVisCraft::Update()
{
	m_input->Update();
	if (m_input->IsKeyPressed(DIK_ESCAPE) == true)
		return false;

	// toggle wireframe mode
	if (m_input->IsKeyPressed(DIK_F1) == true)
	{
		while (m_input->IsKeyPressed(DIK_F1)) m_input->Update();
		m_terrain->GetFlag(TERRAIN_FLAG_WIREFRAME) ? m_terrain->DisableFlag(TERRAIN_FLAG_WIREFRAME) : m_terrain->EnableFlag(TERRAIN_FLAG_WIREFRAME);
	}	

	// toggle color render mode
	if (m_input->IsKeyPressed(DIK_F2) == true)
	{
		while (m_input->IsKeyPressed(DIK_F2)) m_input->Update();
		m_terrain->GetFlag(TERRAIN_FLAG_COLORRENDER) ? m_terrain->DisableFlag(TERRAIN_FLAG_COLORRENDER) : m_terrain->EnableFlag(TERRAIN_FLAG_COLORRENDER);
	}	

	// change brushes
	// toggle wireframe mode
	if (m_input->IsKeyPressed(DIK_1) == true)
	{
		while (m_input->IsKeyPressed(DIK_1)) m_input->Update();
		m_gizmo->SetCurrentBrush(BrushType::Raise);
		m_gui->SetActiveBrush(BrushType::Raise);
	}	

	if (m_input->IsKeyPressed(DIK_2) == true)
	{
		while (m_input->IsKeyPressed(DIK_2)) m_input->Update();
		m_gizmo->SetCurrentBrush(BrushType::Lower);
		m_gui->SetActiveBrush(BrushType::Lower);
	}	

	if (m_input->IsKeyPressed(DIK_3) == true)
	{
		while (m_input->IsKeyPressed(DIK_3)) m_input->Update();
		m_gizmo->SetCurrentBrush(BrushType::Deform);
		m_gui->SetActiveBrush(BrushType::Deform);
	}	

	if (m_input->IsKeyPressed(DIK_4) == true)
	{
		while (m_input->IsKeyPressed(DIK_4)) m_input->Update();
		m_gizmo->SetCurrentBrush(BrushType::Level);
		m_gui->SetActiveBrush(BrushType::Level);
	}

	if (m_input->IsKeyPressed(DIK_5) == true)
	{
		while (m_input->IsKeyPressed(DIK_5)) m_input->Update();
		m_gizmo->SetCurrentBrush(BrushType::Noise);
		m_gui->SetActiveBrush(BrushType::Noise);
	}

	if (m_input->IsKeyPressed(DIK_6) == true)
	{
		while (m_input->IsKeyPressed(DIK_6)) m_input->Update();
		m_gizmo->SetCurrentBrush(BrushType::Smooth);
		m_gui->SetActiveBrush(BrushType::Smooth);
	}

	if (m_input->IsKeyPressed(DIK_UPARROW) == true)
	{
		while (m_input->IsKeyPressed(DIK_UPARROW)) m_input->Update();
		IBrush *brush = m_gizmo->GetCurrentBrush();
		brush->SetSize(brush->GetSize() + 1);
	}	

	if (m_input->IsKeyPressed(DIK_DOWNARROW) == true)
	{
		while (m_input->IsKeyPressed(DIK_DOWNARROW)) m_input->Update();
		IBrush *brush = m_gizmo->GetCurrentBrush();
		brush->SetSize(brush->GetSize() - 1);
	}	

	if (m_input->IsKeyPressed(DIK_TAB) == true)
	{
		while (m_input->IsKeyPressed(DIK_TAB)) m_input->Update();
		m_gui->SetVisible(!m_gui->IsVisible());
	}

	if (m_gui->IsVisible() && m_gizmo->GetInputType() == InputType::Mouse)
	{
		if (m_input->IsMouseDown(MouseButton::Left))
		{
			m_gui->HandleMouseInput(m_input->GetMousePosition());
			m_input->SetMouseButton(MouseButton::Left, false);
		}
	}

	m_camera->Control(m_input);

	if (!m_gui->IsVisible()) {
		m_gizmo->Control(m_input, m_terrain, m_camera, m_kinect);
	}

	if (!RenderGraphics())
		return false;

	return true;
}

/*!
 * \brief
 * \return
 */
LRESULT CALLBACK CVisCraft::MessageHandler(
		HWND hwnd, 
		UINT message, 
		WPARAM wParam, 
		LPARAM lParam
	)
{
	if (hwnd == m_splashhwnd)
	{
		HDC hDC;
		PAINTSTRUCT ps;
		RECT area;
		BITMAP bm;

		switch (message)
		{
		case WM_PAINT:
			{
				if (m_spashBitmap == NULL) {
					m_spashBitmap = LoadBitmap(m_hinstance, MAKEINTRESOURCE(IDB_BITMAP_SPLASHSCREEN));
				}

				hDC = ::BeginPaint(m_splashhwnd, &ps);
				::GetClientRect(m_splashhwnd, &area);

				HDC hdcMem = CreateCompatibleDC(hDC);
				HBITMAP hbmOld = (HBITMAP)SelectObject(hdcMem, m_spashBitmap);

				GetObject(m_spashBitmap, sizeof(bm), &bm);
				BitBlt(hDC, 0, 0, bm.bmWidth, bm.bmHeight, hdcMem, 0, 0, SRCCOPY);

				SelectObject(hdcMem, hbmOld);
				DeleteDC(hdcMem);
			}
			break;
		case WM_CLOSE:
		case WM_DESTROY:
			{
				DeleteObject(m_spashBitmap);
				m_spashBitmap = NULL;
				return FALSE;
			}
			break;
		}

		return DefWindowProc(hwnd, message, wParam, lParam);
	}	

	switch (message)
	{
	case WM_MOUSEMOVE:
		{
			const int x = (short)LOWORD(lParam);
			const int y = (short)HIWORD(lParam);
			m_input->SetMousePosition(x, y);
		}
		break;

	case WM_RBUTTONUP:
	case WM_RBUTTONDOWN:
		{
			m_input->SetMouseButton(MouseButton::Right, message == WM_RBUTTONDOWN);
		}
		break;

	case WM_LBUTTONUP:
	case WM_LBUTTONDOWN:
		{
			m_gizmo->SetInputType(InputType::Mouse);
			m_input->SetMouseButton(MouseButton::Left, message == WM_LBUTTONDOWN);
		}
		break;

	case WM_CLOSE:
	case WM_DESTROY:
		{
			PostQuitMessage(0);
			m_hwnd = NULL;
			return FALSE;
		}
	}

	return DefWindowProc(hwnd, message, wParam, lParam);
}

/*!
 * \brief Call all render methods
 * \return true on success, false otherwise
 */
const bool CVisCraft::RenderGraphics()
{
	// Clear the scene.
	m_renderer->BeginScene(0.0f, 0.4f, 0.8f);

	// Get the world, view, projection matrices from the camera and Direct3D objects.
	D3DXMATRIX worldMatrix, viewMatrix, projectionMatrix, orthoMatrix;
	m_renderer->GetWorldMatrix(worldMatrix);
	m_camera->GetViewMatrix(viewMatrix);
	m_renderer->GetProjectionMatrix(projectionMatrix);
	m_renderer->GetOrthoMatrix(orthoMatrix);

	// Render the terrain buffers.
	m_terrain->Update();

	if (!m_shader->Render(m_terrain->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix))
		return false;

	m_gizmo->Render(worldMatrix, viewMatrix, projectionMatrix, m_camera);

	m_renderer->EnableZBuffer(false);
	m_renderer->EnableAlphaBlending(true);

	D3DXMATRIX screenMatrix;
	D3DXMatrixIdentity(&screenMatrix);
	D3DXMatrixTranslation(&screenMatrix, m_camera->GetPosition().x, m_camera->GetPosition().y, m_camera->GetPosition().z + 1.0f);

	m_gui->Render(m_renderer, screenMatrix, viewMatrix, orthoMatrix);

	m_renderer->EnableAlphaBlending(false);
	m_renderer->EnableZBuffer(true);

	// Present the rendered scene to the screen.
	m_renderer->EndScene();

	return true;
}

bool CVisCraft::CreateSplashScreen()
{
	WNDCLASSEX wc;
	wc.style         = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc   = WindowsProcedure;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = m_hinstance;
	wc.hIcon		 = LoadIcon(NULL, IDI_WINLOGO);
	wc.hIconSm       = wc.hIcon;
	wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(LTGRAY_BRUSH);
	wc.lpszMenuName  = NULL;
	wc.lpszClassName = "SplashScreenClass";
	wc.cbSize        = sizeof(WNDCLASSEX);

	if (!RegisterClassEx(&wc))
		return false;

	m_splashhwnd = CreateWindowEx(
		WS_EX_OVERLAPPEDWINDOW,
		"SplashScreenClass",
		"",
		WS_POPUP,
		(GetSystemMetrics(SM_CXSCREEN) / 2) - 320, (GetSystemMetrics(SM_CYSCREEN) / 2) - 240,
		640, 480,
		NULL, NULL, m_hinstance, NULL
		);

	if (m_splashhwnd == NULL)
		return false;

	ShowWindow(m_splashhwnd, SW_SHOW);
	SetForegroundWindow(m_splashhwnd);
	SetFocus(m_splashhwnd);

	// Handle the windows messages.
	MSG msg;
	while(::PeekMessage(&msg, m_splashhwnd, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return true;
}

void CVisCraft::NewTerrain()
{
	m_terrain->Reset();
}

void CVisCraft::OpenTerrain()
{
	m_terrain->EnableFlag(TERRAIN_FLAG_LOCK);

	char fileName[MAX_PATH] = "";

	OPENFILENAME ofn;
	ZeroMemory(&ofn, sizeof(ofn));

	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = m_hwnd;
	ofn.lpstrFilter = "Bitmap Files (*.bmp)\0*.bmp\0\0";
	ofn.lpstrFile = fileName;
	ofn.nMaxFile = MAX_PATH;
	ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
	ofn.lpstrDefExt = "";

	BOOL result = GetOpenFileName(&ofn);
	if (result == TRUE) {
		m_terrain->LoadHeightMap(fileName);
	}

	m_terrain->DisableFlag(TERRAIN_FLAG_LOCK);
}

D3DXVECTOR2 CVisCraft::GetWindowDimension() const
{
	return D3DXVECTOR2(static_cast<FLOAT>(m_screenWidth), static_cast<FLOAT>(m_screenHeight));
}

void CVisCraft::SaveTerrain()
{
	m_terrain->EnableFlag(TERRAIN_FLAG_LOCK);

	char fileName[MAX_PATH] = "";

	OPENFILENAME ofn;
	ZeroMemory(&ofn, sizeof(ofn));

	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = m_hwnd;
	ofn.lpstrFilter = "Bitmap Files (*.bmp)\0*.bmp\0\0";
	ofn.lpstrFile = fileName;
	ofn.nMaxFile = MAX_PATH;
	ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
	ofn.lpstrDefExt = "";

	BOOL result = GetSaveFileName(&ofn);
	if (result == TRUE) {
		m_terrain->SaveHeightMap(fileName);
	}

	m_terrain->DisableFlag(TERRAIN_FLAG_LOCK);
}

/*!
 * \brief
 * \return
 */
LRESULT CALLBACK WindowsProcedure(
		HWND hwnd, 
		UINT umessage, 
		WPARAM wparam, 
		LPARAM lparam
	)
{
	return CVisCraft::GetInstance()->MessageHandler(hwnd, umessage, wparam, lparam);
}