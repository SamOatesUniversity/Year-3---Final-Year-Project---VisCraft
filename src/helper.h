#pragma once

/**
	Header file includes
*/

#include <windows.h>
#include <iostream>
#include <sstream>
#include <string>

/**
	Custom Assert
*/

#ifndef NDEBUG
	#define ASSERT(condition, message) \
	do { \
		if (!condition) { \
			std::stringstream msg; \
			msg << "Assert Failed: \"" #condition "\"\n In " << __FILE__ \
				<< "(" << __LINE__ << ")\n \"" << message << "\"\nBreak into debugger?"; \
			if (MessageBox(NULL, msg.str().c_str(), "Assert Failed", MB_YESNO | MB_ICONERROR) == IDYES) { \
				DebugBreak(); \
			} \
		} \
	} while (false)
#else
	#define ASSERT(condition, message) do { } while (false)
#endif


/**
	Function prototypes helpers
*/

/*!
 * \brief delete memory and null the pointer
 */
template<typename T> 
void SafeDelete(T*& ptr) {
	if (ptr) 
	{
		delete ptr;
		ptr = nullptr;
	}
}

/*!
 * \brief destroy a window and null the pointer
 */
template<typename T> 
void SafeDestroyWindow(T*& ptr) {
	if (ptr) 
	{
		DestroyWindow(ptr);
		ptr = nullptr;
	}
}

/*!
 * \brief release and null a pointer
 */
template<typename T> 
void SafeRelease(T*& ptr) {
	if (ptr) 
	{
		ptr->Release();
		ptr = nullptr;
	}
}

/*!
 * \brief release and delete, then null a pointer
 */
template<typename T> 
void SafeReleaseDelete(T*& ptr) {
	if (ptr) 
	{
		ptr->Release();
		delete ptr;
		ptr = nullptr;
	}
}
