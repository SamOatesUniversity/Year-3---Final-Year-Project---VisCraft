/**
	Header file includes
*/
#include <windows.h>
#include "crenderer.h"
#include "kinect/CKinect.h"
#include "ccamera.h"
#include "cterrain.h"
#include "cshader.h"
#include "helper.h"
#include "cinput.h"
#include "cgizmo.h"
#include "CGui.h"
#include "cskybox.h"
#include "cwater.h"
#include "../resource/resource.h"

/**
	Function prototypes
*/

static LRESULT CALLBACK WindowsProcedure(HWND, UINT, WPARAM, LPARAM);

/**
	VisCraft class deceleration
*/
class CVisCraft {

private:
	static CVisCraft*			m_instance;

private:
	char						*m_applicationName;								//!< The applications name
	HINSTANCE					m_hinstance;									//!< The handle to the application instance
	HWND						m_hwnd;											//!< The main application window handle

	int							m_screenWidth;									//!< The current width of the main window
	int							m_screenHeight;									//!< The current height of the main window

	HWND						m_splashhwnd;									//!< 
	HBITMAP						m_spashBitmap;									//!< 

	CKinect						*m_kinect;										//!< The kinect interface class

	CInput						*m_input;										//!< The input handling class instance
	CRenderer					*m_renderer;									//!< The d3d render class instance
	CCamera						*m_camera;										//!< The main camera, representing the view into 3d space
	CTerrain					*m_terrain;										//!< The terrain object
	CShader						*m_shader;										//!< The shader we want to render the scene with
	CSkyBox						*m_skybox;										//!< 
	CWater						*m_water;										//!< 

	CGizmo						*m_gizmo;										//!< The xform gizmo
	CGui						*m_gui;											//!< The gui

	bool						m_running;										//!< Is the application currently running?

private:
								//! Render the current state of the world scene to the window
	bool						Update();		

								//! Create the window we will render on
	bool						CreateWindowInternal(
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

								//! 
	bool						CreateSplashScreen();

								//! 
	void						Close() 
								{
									m_running = false;
								}

								//!
	static CVisCraft			*GetInstance() 
								{
									return m_instance;
								}

								//! 
	CGizmo						*GetGizmo() const 
								{
									return m_gizmo;
								}

								//! 
	void						NewTerrain();

								//!
	void						OpenTerrain();

								//! 
	D3DXVECTOR2					GetWindowDimension() const;

								//! 
	void						SaveTerrain();

								//!
	CTerrain					*GetTerrain()
								{
									return m_terrain;
								}

								//! 
	bool						PrepareData();
};