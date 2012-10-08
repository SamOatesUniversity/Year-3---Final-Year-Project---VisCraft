
#include "viscraft.h"

/*!
 * \brief The constructor
 */
VisCraft::VisCraft() : 
	m_applicationName(""),
	m_hinstance(NULL),
	m_hwnd(NULL)
{

}

/*!
 * \brief The copy constructor
 */
VisCraft::VisCraft(const VisCraft& other)
{

}

/*!
 * \brief The destructor
 */
VisCraft::~VisCraft() 
{

}

/*!
 * \brief Initializes the application
 * \return true on success, false if a problem was encountered.
 */
const bool VisCraft::Create()
{
	VisCraftPtr = this;

	// Initialize the width and height of the screen to zero before sending the variables into the function.
	int screenWidth = 0;
	int screenHeight = 0;

	// Initialize the windows api.
	CreateWindowInternal(screenWidth, screenHeight);

	return true;
}

/*!
 * \brief Create the window we will render on
 */
void VisCraft::CreateWindowInternal( 
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

	// Setup the screen settings depending on whether it is running in full screen or in windowed mode.

	// If windowed then set it to 800x600 resolution.
	width  = 800;
	height = 600;

	// Place the window in the middle of the screen.
	posX = (GetSystemMetrics(SM_CXSCREEN) - width)  / 2;
	posY = (GetSystemMetrics(SM_CYSCREEN) - height) / 2;

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
	ShowWindow(m_hwnd, SW_SHOW);
	SetForegroundWindow(m_hwnd);
	SetFocus(m_hwnd);

	return;
}

/*!
 * \brief the main run loop
 */
void VisCraft::Run()
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
		if(msg.message == WM_QUIT || !Render())
		{
			return;
		}
	}
}

/*!
 * \brief Destroy anything we created
 */
void VisCraft::Release()
{

}

/*!
 * \brief Render the current state of the world scene to the window
 * \return true if successful, false otherwise.
 */
bool VisCraft::Render()
{
	return true;
}

/*!
 * \brief
 * \return
 */
LRESULT CALLBACK VisCraft::MessageHandler(
		HWND hwnd, 
		UINT umsg, 
		WPARAM wparam, 
		LPARAM lparam
	)
{
	return DefWindowProc(hwnd, umsg, wparam, lparam);
}

/*!
 * \brief
 * \return
 */
LRESULT CALLBACK WindowsProcedure(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam)
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