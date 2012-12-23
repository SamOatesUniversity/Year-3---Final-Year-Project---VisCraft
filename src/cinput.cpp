
#include "cinput.h"

/*!
 * \brief Construct the input class, initializing member data
 */
CInput::CInput() :
	m_directInput(nullptr),
	m_keyboard(nullptr)
{
	for (int key = 0; key < 255; ++key)
		m_keyboardState[key] = false;

	m_mousePosition = D3DXVECTOR2(0, 0);
	for (int mouseIndex = 0; mouseIndex < MouseButton::Noof; ++ mouseIndex)
		m_mouseButton[mouseIndex] = false;

	m_screenWidth = 0;
	m_screenHeight = 0;
}

/*!
 * \brief Destruct class, release should be called before deletion
 */
CInput::~CInput()
{
	ASSERT(!m_keyboard, "Release has not been called before the deletion of a CInput object");
	ASSERT(!m_directInput, "Release has not been called before the deletion of a CInput object");
}

/*!
 * \brief initialize all input devices
 * \return true on success, false otherwise
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

	::ShowCursor(FALSE);

	if (FAILED(DirectInput8Create(hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&m_directInput, NULL)))
		return false;

	if (FAILED(m_directInput->CreateDevice(GUID_SysKeyboard, &m_keyboard, NULL)))
		return false;

	if (FAILED(m_keyboard->SetDataFormat(&c_dfDIKeyboard)))
		return false;

	if (FAILED(m_keyboard->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_EXCLUSIVE)))
		return false;

	m_keyboard->Acquire();

	return true;
}

/*!
 * \brief Free all input devices
 */
void CInput::Release()
{
	// Release the keyboard.
	m_keyboard->Unacquire();
	SafeRelease(m_keyboard);

	// Release the main interface to direct input.
	SafeRelease(m_directInput);

	::ShowCursor(TRUE);
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
 * \brief Lock the mouse position to the screen
 * \return true if the key is pressed, false otherwise
 */
const bool CInput::IsKeyPressed( 
		const int key													//!< The key to check
	) const
{
	if (m_keyboardState[key] & 0x80)
		return true;

	return false;
}

void CInput::SetMousePosition( 
		const int x,						//!< The new x position of the mouse
		const int y							//!< The new y position of the mouse
	)
{
	m_mousePosition.x = static_cast<float>(x);
	m_mousePosition.y = static_cast<float>(y);
}

/*!
 * \brief Get the relative mouse position
 * \return the absolute mouse position
 */
const D3DXVECTOR2 CInput::GetMousePosition() const 
{
	return D3DXVECTOR2(static_cast<float>(m_mousePosition.x), static_cast<float>(m_mousePosition.y));
}

/*!
 * \brief Get if a mouse button is down
 * \return true if the key is pressed, false otherwise
 */
const bool CInput::IsMouseDown( 
		const MouseButton::Enum button 
	) const
{
	return m_mouseButton[button];
}

/*
*	\brief Sets the state of a mouse button
*/
void CInput::SetMouseButton( 
		MouseButton::Enum mouseButton,							//!< The buttons of which state needs setting
		bool down												//!< Is the button down or up?
	)
{
	m_mouseButton[mouseButton] = down;
}
