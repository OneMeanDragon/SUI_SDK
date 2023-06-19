#include "Framework.h"

static std::wstring SdkBuildInformation{};

void DestroyVars() {
	SdkBuildInformation.~basic_string();
}

int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPTSTR lpCmdLine, _In_ int nShowCmd) {
#if _DEBUG
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif
	UINT retResult{ EXIT_SUCCESS };

	InitThread ThreadInital;

	// Setup the UIInterface DLL
	DWORD(WINAPI * InterfaceVersion)(VOID)           = 0;      // Predefined UiSdkVersion Function
	DWORD(WINAPI * InterfaceFun)(LPVOID lpParameter) = 0;      // Predefined UIThread Function
	std::wstring(WINAPI * BuildInf)(VOID)            = 0;      // Predefined UiSdkBuildInformation Function
	HINSTANCE GfxInterfaceModule = LoadLibrary(L"GfxInterface.SUI"); // Load the Gfx DLL

	if (GfxInterfaceModule) {
		*(void**)&InterfaceVersion = GetProcAddress(GfxInterfaceModule, "UiSdkVersion");          //UiSdkVersion Address
		*(void**)&InterfaceFun     = GetProcAddress(GfxInterfaceModule, "UiThread");              //UIThread Address
		*(void**)&BuildInf         = GetProcAddress(GfxInterfaceModule, "UiSdkBuildInformation"); //UiSdkBuildInformation Address
		uint32_t SdkVer{ 0 };
		if (InterfaceVersion) {
			SdkVer = InterfaceVersion();
			SdkBuildInformation = BuildInf();
		}
		if (InterfaceFun && (SdkVer == GFXSDK_VERSION)) {
			ThreadInital = { 
				CreateEvent(NULL, FALSE, FALSE, NULL), 
				GetCurrentThreadId()
			};
			HANDLE hThread = CreateThread(NULL, 0, InterfaceFun, &ThreadInital, 0, NULL);
			if (hThread != INVALID_HANDLE_VALUE && hThread != nullptr && ThreadInital.hEvent != INVALID_HANDLE_VALUE && ThreadInital.hEvent != nullptr) {
				BOOL result = FALSE;
				while (!result) { /* because our ui thread dosent become available immediatly */
					Sleep(10);
					result = PostThreadMessage(GetThreadId(hThread), WM_INITIALIZE, 0, NULL);
				}
				WaitForSingleObject(ThreadInital.hEvent, INFINITE);
				CloseHandle(ThreadInital.hEvent); /* our initalization is done */
				ThreadInital.hEvent = nullptr;

				// set the id of the sdl dll
				DWORD SDLThreadID = GetThreadId(hThread);

				if (ThreadInital.bInit) {

					// DLL Initialized
					MSG msg = { 0 };
					BOOL bBreak = FALSE;
					while (GetMessage(&msg, NULL, 0, 0) != -1 && !bBreak)
					{
						/*
							This means you closed the UI Shutdown your APP
						*/
						if (msg.message == WM_QUIT || msg.message == WM_DESTROY || ThreadInital.ExitMessage == WM_QUIT)
						{
							OutputDebugString(TEXT("WM_QUIT | WM_DESTROY\r\n"));
							//cleanup
							DestroyVars();
							break; /* this breaks the while loop our UI exited, only reason quit and destroy are out side the switch statement */
						}

						switch (msg.message) {
							case WM_INITIALIZE: {
								OutputDebugString(TEXT("WM_INITIALIZE\r\n"));
								break;
							}
							default: {
								TranslateMessage(&msg);
								DispatchMessage(&msg);
							}
						}
						Sleep(1);
					}

					// Wait for the ui thread to exit, if it hasnt yet done so.
					WaitForSingleObject(hThread, INFINITE);
					CloseHandle(hThread);
				}
			} 
		}
	}

	if(GfxInterfaceModule) FreeLibrary(GfxInterfaceModule);
	ExitProcess(retResult);
}