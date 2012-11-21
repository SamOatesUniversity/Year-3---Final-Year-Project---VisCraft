
#include "cviscraft.h"

/*!
 * \brief The constructor
 */
CVisCraft::CVisCraft() : 
	m_applicationName(""),
	m_hinstance(NULL),
	m_hwnd(NULL)
{
	m_input = nullptr;
	m_renderer = nullptr;
	m_camera = nullptr;
	m_terrain = nullptr;
	m_shader = nullptr;
}

/*!
 * \brief The copy constructor
 */
CVisCraft::CVisCraft(const CVisCraft& other)
{
}

/*!
 * \brief The destructor
 */
CVisCraft::~CVisCraft() 
{
	ASSERT(!m_input || !m_renderer || !m_camera || !m_terrain || !m_shader, "Release has not been called before the deletion of a VisCraft object.\nTHIS WILL LEAK MEMORY!");
}

/*!
 * \brief Initializes the application
 * \return true on success, false if a problem was encountered.
 */
const bool CVisCraft::Create()
{
	VisCraftPtr = this;

	// Initialize the width and height of the screen to zero before sending the variables into the function.
	int screenWidth = 0;
	int screenHeight = 0;

	// Initialize the windows api.
	CreateWindowInternal(screenWidth, screenHeight);

	// Create the input handling class
	m_input = new CInput();
	if (!m_input->Create(m_hinstance, m_hwnd, screenWidth, screenHeight))
	{
		Release();
		ASSERT(false, "Failed to create input class");
		return false;
	}

	// Create the d3d renderer class
	m_renderer = new CRenderer();
	if (!m_renderer->Create(m_hwnd, screenWidth, screenHeight))
	{
		Release();
		ASSERT(false, "Failed to create renderer");
		return false;
	}

	m_camera = new CCamera();
	m_camera->SetPosition(50.0f, 25.0f, -7.0f);

	m_terrain = new CTerrain();
	if (!m_terrain->Create(m_renderer))
	{
		Release();
		ASSERT(false, "Failed to create terrain class");
		return false;
	}

	m_terrain->LoadHeightMap("heightmap.bmp");

	m_shader = new CShader();
	if (!m_shader->Create(m_renderer))
	{
		Release();
		ASSERT(false, "Failed to create/load shaders");
		return false;
	}

	return true;
}

/*!
 * \brief Create the window we will render on
 */
void CVisCraft::CreateWindowInternal( 
		int& width, 
		int& height 
)
{
	// Get the instance of this application.
	m_hinstance = GetModuleHandle(NULL);

	// Give the application a name.
	m_applicationName = "VisCraft";

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

	// Bring the window up on the screen and set it as main focus.
	ShowWindow(m_hwnd, SW_SHOWMAXIMIZED);
	SetForegroundWindow(m_hwnd);
	SetFocus(m_hwnd);

	return;
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
	while (true)
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
	SafeReleaseDelete(m_input);
	SafeReleaseDelete(m_renderer);
	SafeDelete(m_camera);
	SafeReleaseDelete(m_terrain);
	SafeReleaseDelete(m_shader);

	// Show the mouse cursor.
	ShowCursor(true);

	// Remove the window.
	DestroyWindow(m_hwnd);

	// Remove the application instance.
	UnregisterClass(m_applicationName, m_hinstance);
	m_hinstance = NULL;

	// Release the pointer to this class.
	VisCraftPtr = NULL;
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

	m_camera->Control(m_input);

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
		UINT umsg, 
		WPARAM wparam, 
		LPARAM lparam
	)
{
	return DefWindowProc(hwnd, umsg, wparam, lparam);
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
	D3DXMATRIX worldMatrix, viewMatrix, projectionMatrix;
	m_renderer->GetWorldMatrix(worldMatrix);
	m_camera->GetViewMatrix(viewMatrix);
	m_renderer->GetProjectionMatrix(projectionMatrix);

	// Render the terrain buffers.
	m_terrain->Update();

	if (!m_shader->Render(m_terrain->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix))
		return false;

	// Present the rendered scene to the screen.
	m_renderer->EndScene();

	return true;
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
	switch(umessage)
	{
	case WM_CLOSE:
	case WM_DESTROY:
		{
			PostQuitMessage(0);
			return FALSE;
		}
	}

	return VisCraftPtr->MessageHandler(hwnd, umessage, wparam, lparam);
}