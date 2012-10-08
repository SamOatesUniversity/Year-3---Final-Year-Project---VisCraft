
#include "main.h"

/*!
 * \brief Main entry point of the application
 * \return TRUE on success, FALSE on unknown error else an error code
 */
int WINAPI WinMain(
	HINSTANCE hInstance,				//!< A handle to the current instance of the application.
	HINSTANCE hPrevInstance,			//!< A handle to the previous instance of the application. This parameter is always NULL.
	LPSTR cmdline,						//!< Command line arguments
	int cmdshow							//!< The initial show status of the application
	)
{
	#if defined(_DEBUG)
		_CrtSetDbgFlag(_CrtSetDbgFlag(_CRTDBG_REPORT_FLAG)|_CRTDBG_LEAK_CHECK_DF);
	#endif

	VisCraft *const visCraft = new VisCraft();
	if (!visCraft->Create()) 
	{
		SafeDelete(visCraft);
		return FALSE;
	}

	visCraft->Run();
	visCraft->Release();

	SafeDelete(visCraft);

	return TRUE;
}