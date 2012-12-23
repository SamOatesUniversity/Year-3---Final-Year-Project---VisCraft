/**
	Header file includes
*/
#include <windows.h>
#include "crenderer.h"
#include "ccamera.h"
#include "cterrain.h"
#include "cshader.h"
#include "helper.h"
#include "cinput.h"
#include "cgizmo.h"


/**
	Defines and Globals
*/

static class CVisCraft*	VisCraftPtr = nullptr;

/**
	Function prototypes
*/

static LRESULT CALLBACK WindowsProcedure(HWND, UINT, WPARAM, LPARAM);

/**
	VisCraft class deceleration
*/
class CVisCraft {

private:
	char						*m_applicationName;								//!< The applications name
	HINSTANCE					m_hinstance;									//!< The handle to the application instance
	HWND						m_hwnd;											//!< The main application window handle

	CInput						*m_input;										//!< The input handling class instance
	CRenderer					*m_renderer;									//!< The d3d render class instance
	CCamera						*m_camera;										//!< The main camera, representing the view into 3d space
	CTerrain					*m_terrain;										//!< The terrain object
	CShader						*m_shader;										//!< The shader we want to render the scene with

	CGizmo						*m_gizmo;										//!< The xform gizmo

private:
								//! Render the current state of the world scene to the window
	bool						Update();		

								//! Create the window we will render on
	void						CreateWindowInternal(
									int& width,									//!< A refernce for what the width of the window will be
									int& height									//!< A refernce for what the height of the window will be
								);

								//! Call all render methods
	const bool					RenderGraphics();

public:
								//! The constructor
								CVisCraft();

								//! The destructor
								~CVisCraft();

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