#if !defined(GFXDLLSDK_HEADER)
#define GFXDLLSDK_HEADER
#pragma once

/*

	This SDK is for use with the GfxInterface.SUI

*/
#define GFXSDK_VERSION               1

#define ERROR_SDLNOTINITIALIZED      1
#define ERROR_COULDNOTCREATEWINDOW   2
#define ERROR_COULDNOTCREATERENDERER 3
#define ERROR_SDLTHREADINIT          4

#define WM_INITIALIZE               (WM_USER + 1001)

#pragma region "Init message"
struct InitThread // our initialize message
{
	HANDLE hEvent;
	DWORD hThreadID; // id of the main thread so UI can send messages back
	BOOL bInit{ FALSE };
	DWORD dwExitCode[2]{ (DWORD)-1,(DWORD)-1 }; // 0 = SdlThread, 1 = ApiThreadPump
	UINT ExitMessage{ 0 };
};
#pragma endregion


#endif