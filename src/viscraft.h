/**
	Header file includes
*/
#include <windows.h>

/**
	Defines and Globals
*/

#define FULL_SCREEN		FALSE
static class VisCraft*	VisCraftPtr = nullptr;

/**
	Function prototypes
*/

static LRESULT CALLBACK WindowsProcedure(HWND, UINT, WPARAM, LPARAM);

/**
	VisCraft class deceleration
*/
class VisCraft {

private:
	char						*m_applicationName;								//!< The applications name
	HINSTANCE					m_hinstance;									//!< The handle to the application instance
	HWND						m_hwnd;											//!< The main application window handle

private:
	bool						Render();								
	void						CreateWindowInternal(
									int& width, 
									int& height
								);
	//void						ShutdownWindows();

public:
								VisCraft();
								VisCraft(
									const VisCraft& other
								);
								~VisCraft();

	const bool					Create();
	void						Release();
	void						Run();

	LRESULT CALLBACK			MessageHandler(
									HWND hwnd, 
									UINT msg, 
									WPARAM wParam, 
									LPARAM lParam
								);
};