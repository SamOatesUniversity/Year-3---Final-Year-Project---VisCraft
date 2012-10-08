/**
	Header file includes
*/
#include <windows.h>
#include "viscraft.h"

/**
	Additionally included libraries
*/


/**
	Function prototypes helpers
*/

template<typename T> 
void SafeDelete(T*& ptr) {
	if (ptr) 
	{
		delete ptr;
		ptr = NULL;
	}
}

template<typename T> 
void SafeDelete(T*const& ptr) {
	if (ptr) 
	{
		delete ptr;
		const_cast<T*>(ptr) = NULL;
	}
}

/**
	Advanced memory leek detection
*/
#if defined(_DEBUG)
	#include <crtdbg.h>
	#define new new( _NORMAL_BLOCK, __FILE__, __LINE__)
	#pragma warning(disable : 4345)
#endif
