/**
	Header file includes
*/
#include <windows.h>

/**
	Defines and Globals
*/

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
								//! Render the current state of the world scene to the window
	bool						Render();		

								//! Create the window we will render on
	void						CreateWindowInternal(
									int& width,									//!< A refernce for what the width of the window will be
									int& height									//!< A refernce for what the height of the window will be
								);

public:
								//! The constructor
								VisCraft();

								//! The copy constructor
								VisCraft(
									const VisCraft& other						//!< 
								);

								//! The destructor
								~VisCraft();

								//! Initializes the application
	const bool					Create();

								//! Destroy anything we created
	void						Release();

								//! The main run loop
	void						Run();

								//! 
	LRESULT CALLBACK			MessageHandler(
									HWND hwnd,									//!< 
									UINT msg,									//!< 
									WPARAM wParam,								//!< 
									LPARAM lParam								//!< 
								);
};