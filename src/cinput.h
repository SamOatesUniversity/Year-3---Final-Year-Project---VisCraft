/**
	Header file includes
*/

#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>

/**
	Additionally included libraries
*/

#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

class CInput {
private:
	IDirectInput8					*m_directInput;								//!< 
	IDirectInputDevice8				*m_keyboard;								//!< 
	IDirectInputDevice8				*m_mouse;									//!< 

	unsigned char					m_keyboardState[256];						//!< 
	DIMOUSESTATE					m_mouseState;								//!< 

	int								m_screenWidth;								//!< 
	int								m_screenHeight;								//!< 
	int								m_mouseX;									//!< 
	int								m_mouseY;									//!< 

public:
									//! 
									CInput();

									//! 
									~CInput();

									//! 
	bool							Create(
										HINSTANCE hInstance,					//!< 
										HWND hwnd,								//!< 
										int screenWidth,						//!< 
										int screenHeight						//!< 
									);

									//! 
	void							Release();
	
									//! 
	bool							Update();
};