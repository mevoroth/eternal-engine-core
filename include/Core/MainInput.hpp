#pragma once

#if ETERNAL_PLATFORM_WINDOWS
#include <Windows.h>
#else

#ifndef HINSTANCE
#define HINSTANCE	void*
#endif

#ifndef LPSTR
#define LPSTR		const char*
#endif

#endif

struct ANativeActivity;

namespace Eternal
{
	namespace Core
	{
		struct MainInput
		{
			const char* ApplicationName				= nullptr;
			HINSTANCE hInstance						= nullptr;
			HINSTANCE hPrevInstance					= nullptr;
			LPSTR lpCmdLine							= nullptr;
			int nCmdShow							= 0;

			ANativeActivity* AndroidNativeActivity	= nullptr;
			void* AndroidSavedState					= nullptr;
			size_t AndroidSavedStateSize			= 0ull;
		};
	}
}

#if !ETERNAL_PLATFORM_WINDOWS
#undef HINSTANCE
#undef LPSTR
#endif
