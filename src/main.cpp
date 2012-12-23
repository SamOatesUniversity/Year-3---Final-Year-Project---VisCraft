
#include "main.h"

/*!
 * \brief Main entry point of the application
 * \return TRUE on success, FALSE on unknown error else an error code
 */
int WINAPI WinMain(
	_In_ HINSTANCE hInstance,				//!< A handle to the current instance of the application.
	_In_opt_ HINSTANCE hPrevInstance,			//!< A handle to the previous instance of the application. This parameter is always NULL.
	_In_ LPSTR cmdline,						//!< Command line arguments
	_In_ int cmdshow							//!< The initial show status of the application
	)
{
	#if defined(_DEBUG)
		_CrtSetDbgFlag(_CrtSetDbgFlag(_CRTDBG_REPORT_FLAG)|_CRTDBG_LEAK_CHECK_DF);
	#endif

	CVisCraft *const visCraft = new CVisCraft();
	if (!visCraft->Create()) 
	{
		delete visCraft;
		return FALSE;
	}

	visCraft->Run();

	visCraft->Release();
	delete visCraft;

	return TRUE;
}