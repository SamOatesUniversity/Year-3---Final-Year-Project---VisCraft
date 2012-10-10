
#include "cinput.h"

/*!
 * \brief Construct the input class, initializing member data
 */
CInput::CInput() :
	m_directInput(nullptr),
	m_mouse(nullptr),
	m_keyboard(nullptr)
{

}

/*!
 * \brief Destruct class, release should be called before deletion
 */
CInput::~CInput()
{
	ASSERT(!m_mouse, "Release has not been called before the deletion of a CInput object");
	ASSERT(!m_keyboard, "Release has not been called before the deletion of a CInput object");
	ASSERT(!m_directInput, "Release has not been called before the deletion of a CInput object");
}

/*!
 * \brief initialize all input devices
 * \return true on sucess, false otherwise
 */
bool CInput::Create( 
		HINSTANCE hInstance,			//!< The applications instance handle
		HWND hwnd,						//!< The main window handle
		int screenWidth,				//!< The screens width
		int screenHeight				//!< The screen height
	)
{
	m_screenWidth = screenWidth;
	m_screenHeight = screenHeight;

	m_mouseX = m_mouseY = 0;

	if (FAILED(DirectInput8Create(hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&m_directInput, NULL)))
		return false;

	if (FAILED(m_directInput->CreateDevice(GUID_SysKeyboard, &m_keyboard, NULL)))
		return false;

	if (FAILED(m_keyboard->SetDataFormat(&c_dfDIKeyboard)))
		return false;

	if (FAILED(m_keyboard->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_EXCLUSIVE)))
		return false;

	

	if (FAILED(m_directInput->CreateDevice(GUID_SysMouse, &m_mouse, NULL)))
		return false;

	if (FAILED(m_mouse->SetDataFormat(&c_dfDIMouse)))
		return false;

	if (FAILED(m_mouse->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE)))
		return false;

	m_keyboard->Acquire();
	m_mouse->Acquire();

	return true;
}

/*!
 * \brief Free all input devices
 */
void CInput::Release()
{
	// Release the mouse.
	m_mouse->Unacquire();
	SafeRelease(m_mouse);

	// Release the keyboard.
	m_keyboard->Unacquire();
	SafeRelease(m_keyboard);

	// Release the main interface to direct input.
	SafeRelease(m_directInput);
}

/*!
 * \brief Update input devices
 * \return true if all device updated successfully, false otherwise
 */
bool CInput::Update()
{
	// Read the current state of the keyboard.
	if (!HandleKeyboard())
		return false;

	// Read the current state of the mouse.
	if (!HandleMouse())
		return false;

	// Process the changes in the mouse.
	HandleMousePosition();

	return true;
}

/*!
 * \brief Update keyboard key state, or if device lost, try to reacquire it.
 * \return true on success, false otherwise
 */
const bool CInput::HandleKeyboard()
{
	// Read the keyboard device.
	const HRESULT result = m_keyboard->GetDeviceState(sizeof(m_keyboardState), (LPVOID)&m_keyboardState);
	if (FAILED(result))
	{
		// If the keyboard lost focus or was not acquired then try to get control back.
		if((result == DIERR_INPUTLOST) || (result == DIERR_NOTACQUIRED))
			m_keyboard->Acquire();
		else
			return false;
	}

	return true;
}

/*!
 * \brief Update mouse state, or if device lost, try to reacquire it.
 * \return true on success, false otherwise
 */
const bool CInput::HandleMouse()
{
	// Read the mouse device.
	const HRESULT result = m_mouse->GetDeviceState(sizeof(DIMOUSESTATE), (LPVOID)&m_mouseState);
	if(FAILED(result))
	{
		// If the mouse lost focus or was not acquired then try to get control back.
		if((result == DIERR_INPUTLOST) || (result == DIERR_NOTACQUIRED))
			m_mouse->Acquire();
		else
			return false;
	}

	return true;
}

/*!
 * \brief Lock the mouse position to the screen
 */
void CInput::HandleMousePosition()
{
	// Update the location of the mouse cursor based on the change of the mouse location during the frame.
	m_mouseX += m_mouseState.lX;
	m_mouseY += m_mouseState.lY;

	// Ensure the mouse location doesn't exceed the screen width or height.
	if (m_mouseX < 0) m_mouseX = 0;
	if (m_mouseY < 0) m_mouseY = 0;
	if (m_mouseX > m_screenWidth) m_mouseX = m_screenWidth;
	if (m_mouseY > m_screenHeight) m_mouseY = m_screenHeight;
}

/*!
 * \brief Lock the mouse position to the screen
 * \return true if the key is pressed, false otherwise
 */
const bool CInput::IsKeyPressed( 
		const int key 
	)
{
	if (m_keyboardState[key] & 0x80)
		return true;

	return false;
}
