
#include "cinput.h"

/*!
 * \brief
 */
CInput::CInput() :
	m_directInput(nullptr),
	m_mouse(nullptr),
	m_keyboard(nullptr)
{

}

/*!
 * \brief
 */
CInput::~CInput()
{

}

/*!
 * \brief
 * \return
 */
bool CInput::Create( 
		HINSTANCE hInstance,			//!< 
		HWND hwnd,						//!< 
		int screenWidth,				//!< 
		int screenHeight				//!< 
	)
{
	return true;
}

/*!
 * \brief
 */
void CInput::Release()
{

}

/*!
 * \brief
 * \return
 */
bool CInput::Update()
{
	return true;
}


