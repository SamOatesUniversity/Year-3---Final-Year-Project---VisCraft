#pragma once

/**
	Header file includes
*/

#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#include <D3DX10math.h>

#include "helper.h"

/**
	Additionally included libraries
*/

#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

/**
	Input enums
*/

struct MouseButton {
	enum Enum {
		Left = 0,
		Right = 1,
		Middle = 2,
		Noof
	};
};

class CInput {
private:
	IDirectInput8					*m_directInput;								//!< The direct input object
	IDirectInputDevice8				*m_keyboard;								//!< The keyboard input device

	unsigned char					m_keyboardState[256];						//!< The current key states of the keyboard

	int								m_screenWidth;								//!< The windows screen width
	int								m_screenHeight;								//!< The windows screen height

	D3DXVECTOR2						m_mousePosition;							//!< The position of the mouse
	bool							m_mouseButton[MouseButton::Noof];			//!< The states of the mouse buttons

private:
									//! Update keyboard key state, or if device lost, try to reacquire it.
	const bool						HandleKeyboard();		

									//! Update mouse state, or if device lost, try to reacquire it.
	const bool						HandleMouse();

									//! Lock the mouse position to the screen
	void							HandleMousePosition();

public:
									//! Construct the input class, initializing member data
									CInput();

									//! Destruct class, release should be called before deletion
									~CInput();

									//! initialize all input devices
	bool							Create(
										HINSTANCE hInstance,					//!< The applications instance handle
										HWND hwnd,								//!< The main window handle
										int screenWidth,						//!< The screen width
										int screenHeight						//!< The screens height
									);

									//! Free all input devices
	void							Release();
	
									//! Update input devices
	bool							Update();

									//! Check if a key is pressed
	const bool						IsKeyPressed( 
										const int key							//!< The key to check
									) const;

									//! Sets the position of the mouse
	void							SetMousePosition(
										const int x,							//!< The new x position of the mouse
										const int y								//!< The new y position of the mouse
									);

									//! Get the cursor position
	const D3DXVECTOR2				GetMousePosition() const;

									//! Get if a mouse button is down
	const bool						IsMouseDown(
										const MouseButton::Enum button
									) const;

									//! Sets the state of a mouse button
	void							SetMouseButton( 
										MouseButton::Enum mouseButton,			//!< The buttons of which state needs setting
										bool down								//!< Is the button down or up?
									);
};