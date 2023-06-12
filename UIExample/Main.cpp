#include "Framework.h"

static std::wstring SdkBuildInformation{};

// Sub Menus
#define BASE_FILE TEXT("&File")
#define BASE_SETTINGS TEXT("&Settings")

// Submenu of Submenu
#define BASE_SUBSUB_MENUX TEXT("&XMenu")
sSubMenuItemW XSETTINGS_TEST = sSubMenuItemW{ BASE_SETTINGS, BASE_SUBSUB_MENUX };

// Commands
#define BASE_TEST TEXT("Test")
#define BASE_TEST2 TEXT("Test2")
#define BASE_XTEST TEXT("XItem")
sSubMenuItemW SETTINGS_TEST = sSubMenuItemW{ BASE_SETTINGS, BASE_TEST2 };
sSubMenuItemW SETTINGS_XTEST = sSubMenuItemW{ BASE_SUBSUB_MENUX, BASE_XTEST };

// command id's
UINT MENU_TEST{ 0 };
UINT MENU_SETTINGS_TEST2{ 0 };
UINT XMENU_SETTINGS_XTEST{ 0 };

#pragma region "Extra Shit"
const uiString AM_PM(int inBuf)
{
	if (inBuf >= 12) {
		return uiString(TEXT("PM"));
	}
	return uiString(TEXT("AM"));
}

const uiString TimeStamp(void)
{
	uiChar timestamp[MAX_PATH]{ 0 };
	SYSTEMTIME st;
	GetLocalTime(&st);
	const uiString szAMPM = AM_PM(st.wHour);
	WORD fHour = st.wHour;
	if (fHour > 12) { fHour -= 12; }
	if (fHour == 0) { fHour += 12; }
	//This string has a forced length, to change this edit the initalized value for *timestamp in Main.cpp
#ifdef UNICODE
	const wchar_t timeformat[]{ TEXT("[%02i:%02i:%02i %ls] ") };
	swprintf_s(timestamp, timeformat, fHour, st.wMinute, st.wSecond, szAMPM.c_str());
#else
	const char timeformat[]{ TEXT("[%02i:%02i:%02i %s] ") };
	sprintf_s(timestamp, timeformat, fHour, st.wMinute, st.wSecond, szAMPM);
#endif
	return uiString(timestamp);
}
#pragma endregion

#pragma region "          UI QUEUE ARRAYS          "
std::deque<sAddChat> AddTextArr{};
std::deque<sAddTextA> AddTextArrA{};
std::deque<sAddTextW> AddTextArrW{};
#pragma endregion

void DestroyVars() { /* these vars will mention a memory leak if not destroyed at destruct */
	AddTextArr.~deque();
	AddTextArrA.~deque();
	AddTextArrW.~deque();
	SdkBuildInformation.~basic_string();
	{
		XSETTINGS_TEST.submenu.~basic_string();
		XSETTINGS_TEST.item.~basic_string();
		SETTINGS_XTEST.submenu.~basic_string();
		SETTINGS_XTEST.item.~basic_string();
		SETTINGS_TEST.submenu.~basic_string();
		SETTINGS_TEST.item.~basic_string();
	}
}

int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPTSTR lpCmdLine, _In_ int nShowCmd) {
#if _DEBUG
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif
	//Load the application icon. (unless you want the default Icon)
	//HICON Application_Icon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));

	// Setup the UIInterface DLL
	DWORD(WINAPI * InterfaceVersion)(VOID)           = 0;      // Predefined UiSdkVersion Function
	DWORD(WINAPI * InterfaceFun)(LPVOID lpParameter) = 0;      // Predefined UIThread Function
	std::wstring(WINAPI * BuildInf)(VOID)            = 0;      // Predefined UiSdkBuildInformation Function
	HINSTANCE InterfaceModule = LoadLibrary(L"Interface.SUI"); // Load the UI DLL

	if (InterfaceModule) {
		*(void**)&InterfaceVersion = GetProcAddress(InterfaceModule, "UiSdkVersion");          //UiSdkVersion Address
		*(void**)&InterfaceFun     = GetProcAddress(InterfaceModule, "UiThread");              //UIThread Address
		*(void**)&BuildInf         = GetProcAddress(InterfaceModule, "UiSdkBuildInformation"); //UiSdkBuildInformation Address
		uint32_t SdkVer{ 0 };
		if (InterfaceVersion) {
			SdkVer = InterfaceVersion();
			SdkBuildInformation = BuildInf();
		}
		if (InterfaceFun && (SdkVer == UISDK_VERSION)) {
			HANDLE hThread = CreateThread(NULL, 0, InterfaceFun, NULL /* NULL = default icon, (LPVOID)Application_Icon = our main thread icon */, 0, NULL);
			HANDLE hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
			MyMessageData data = { hEvent, GetCurrentThreadId() /* message data */ };
			if (hThread != INVALID_HANDLE_VALUE && hThread != nullptr && hEvent != INVALID_HANDLE_VALUE && hEvent != nullptr) {
				BOOL result = FALSE;
				while (!result) { /* because our ui thread dosent become available immediatly */
					Sleep(10);
					result = PostThreadMessage(GetThreadId(hThread), WM_MY_MESSAGE, 0, reinterpret_cast<LPARAM>(&data));
				}
				WaitForSingleObject(hEvent, INFINITE);
				CloseHandle(hEvent); /* our initalization is done */
			}
			else { /* Thread creation or Event creation failed */
				ExitProcess(EXIT_FAILURE);
			}

			/*
				UI is Loaded and ready to go
			*/
			MSG msg = { 0 };
			while (TRUE) { /* main thread message pump */
				if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
				{
					/*
					
						This means you closed the UI Shutdown your APP

					*/
					if (msg.message == WM_QUIT || msg.message == WM_DESTROY)
					{
						OutputDebugString(TEXT("WM_QUIT | WM_DESTROY\r\n"));
						//cleanup
						DestroyVars();
						break; /* this breaks the while loop our UI exited, only reason quit and destroy are out side the switch statement */
					}
					switch (msg.message) {
						case WM_MY_MESSAGE: {
							OutputDebugString(TEXT("event message\r\n"));
							break;
						}
						case WM_UIINITIALIZED: { // send an add chat or some shit
							/*
								
								This means Start your main Application shit

							*/

							/*
								sAddChat is WIDE format only and it is here only if you want to customize
								output colors
							*/
							//{
							//	sAddChat test2;
							//	test2.FmtMessage(TEXT("%s"), TimeStamp().c_str());
							//	test2.FmtMessage(colors::darkslategray, TEXT("UIInit: %s\r\n"), TEXT("ack."));
							//	test2.FmtMessage(TEXT("%s"), TimeStamp().c_str());
							//	test2.FmtMessage(colors::teal, colors::deeppurple, TEXT("TestMessage: %s\r\n"), TEXT("just a message."));
							//	/*
							//		our struct must never go out of scope
							//		and
							//		our UI will tell us when to remove it
							//		via: WM_UIPOPADDCHAT
							//	*/
							//	AddTextArr.push_back(test2);
							//	PostThreadMessage(GetThreadId(hThread), WM_UIINITIALIZED_ACK, 0, reinterpret_cast<LPARAM>(&AddTextArr.front()));
							//}
							/*
								sAddTextA sAddTextW
								MB and UNICODE add text feature
								sAddTextA: for those lazy fuckers that wont quit using multibyte mode.
							*/
							//{ // test sAddTextA
							//	sAddTextA testA{};
							//	testA.FmtMessage("%ls", TimeStamp().c_str());
							//	testA.FmtMessage(colors::deeppurple, colors::yellow, "Testing AddText: %s\r\n", "A (MB)");
							//	AddTextArrA.push_back(testA);
							//	PostThreadMessage(GetThreadId(hThread), WM_UIADDTEXTA, 0, reinterpret_cast<LPARAM>(&AddTextArrA.front()));
							//}
							{ // test sAddTextW
								sAddTextW testW{};
								testW.FmtMessage(TEXT("%s"), TimeStamp().c_str());
								testW.FmtMessage(colors::deeppurple, TEXT("%s\r\n"), SdkBuildInformation.c_str());
								AddTextArrW.push_back(testW);
								PostThreadMessage(GetThreadId(hThread), WM_UIADDTEXTW, 0, reinterpret_cast<LPARAM>(&AddTextArrW.front()));
							}
							// Add Menu Items
							{
								// Add a base sub menu
								PostThreadMessage(GetThreadId(hThread), WM_MENU_ADDSUBMENUW, 0, reinterpret_cast<LPARAM>(&BASE_SETTINGS));
								// Add a base command
								PostThreadMessage(GetThreadId(hThread), WM_MENU_ADDCOMMANDW, 0, reinterpret_cast<LPARAM>(&BASE_TEST));
								// Add command to sub menu
								PostThreadMessage(GetThreadId(hThread), WM_MENU_ADDSUBMENUCOMMANDW, 0, reinterpret_cast<LPARAM>(&SETTINGS_TEST));
								// Add submenu of a submenu
								PostThreadMessage(GetThreadId(hThread), WM_MENU_ADDSUBMENUSUBMENUW, 0, reinterpret_cast<LPARAM>(&XSETTINGS_TEST));
								// Add command of a submenu->submenu
								PostThreadMessage(GetThreadId(hThread), WM_MENU_ADDSUBMENUCOMMANDW, 0, reinterpret_cast<LPARAM>(&SETTINGS_XTEST));
							}
							break;
						}
                        /////////////////////////////////////////////////////////
						// Menu
	  				    /////////////////////////////////////////////////////////
						case WM_MENU_UPDATE: { // update our id holder
							sMessageBoxText* menuname = reinterpret_cast<sMessageBoxText*>(msg.lParam);
							if (menuname->MessageW == std::wstring(BASE_TEST)) {
								MENU_TEST = msg.wParam;
							}
							if (menuname->MessageW == std::wstring(BASE_TEST2)) {
								MENU_SETTINGS_TEST2 = msg.wParam;
							}
							if (menuname->MessageW == std::wstring(BASE_XTEST)) {
								XMENU_SETTINGS_XTEST = msg.wParam;
							}
							break;
						}
						case WM_MENU_COMMAND: {
							if (msg.wParam == MENU_TEST) {
								sAddTextW testW{};
								testW.FmtMessage(TEXT("%s"), TimeStamp().c_str());
								testW.FmtMessage(colors::seagreen, TEXT("%s\r\n"), TEXT("Test menu command was pressed."));
								AddTextArrW.push_back(testW);
								PostThreadMessage(GetThreadId(hThread), WM_UIADDTEXTW, 0, reinterpret_cast<LPARAM>(&AddTextArrW.front()));
							}
							if (msg.wParam == MENU_SETTINGS_TEST2) {
								sAddTextW testW{};
								testW.FmtMessage(TEXT("%s"), TimeStamp().c_str());
								testW.FmtMessage(colors::darkslategray, TEXT("%s\r\n"), TEXT("Settings->Test2 menu command was pressed."));
								AddTextArrW.push_back(testW);
								PostThreadMessage(GetThreadId(hThread), WM_UIADDTEXTW, 0, reinterpret_cast<LPARAM>(&AddTextArrW.front()));
							}
							if (msg.wParam == XMENU_SETTINGS_XTEST) {
								sAddTextW testW{};
								testW.FmtMessage(TEXT("%s"), TimeStamp().c_str());
								testW.FmtMessage(colors::darkslategray, TEXT("%s\r\n"), TEXT("Settings->XMenu->XItem menu command was pressed."));
								AddTextArrW.push_back(testW);
								PostThreadMessage(GetThreadId(hThread), WM_UIADDTEXTW, 0, reinterpret_cast<LPARAM>(&AddTextArrW.front()));
							}
							break;
						}
                        /////////////////////////////////////////////////////////

						case WM_POPADDTEXTA: {
							AddTextArrA.pop_front();
							break;
						}
						case WM_POPADDTEXTW: {
							AddTextArrW.pop_front();
							break;
						}
						case WM_UIPOPADDCHAT: { // UI posted our message, remove it from the queue
							if (AddTextArr.size() > 0) {
								AddTextArr.pop_front();
							}
							break;
						}
						case WM_UIMESSAGEINPUT: { // Message from the Edit Input box
							sMessageBoxText* pData = reinterpret_cast<sMessageBoxText*>(msg.lParam);
							// once we are done processing this, queue that we are ready for more if any.

							sAddTextW testW{};
							testW.FmtMessage(TEXT("%s"), TimeStamp().c_str());
							testW.FmtMessage(colors::gray, TEXT("%s\r\n"), pData->MessageW.c_str());
							AddTextArrW.push_back(testW);
							PostThreadMessage(GetThreadId(hThread), WM_UIADDTEXTW, 0, reinterpret_cast<LPARAM>(&AddTextArrW.front()));

							// if this message is not sent back you will not recieve anymore edit process messages
							PostThreadMessage(GetThreadId(hThread), WM_INPUTCOMPLETED, 0, NULL);
							break;
						}
						default:
						{
							TranslateMessage(&msg);
							DispatchMessage(&msg);
						}
					}
				} else {
					Sleep(10);
				}
			}

			// Wait for the ui thread to exit, if it hasnt yet done so.
			WaitForSingleObject(hThread, INFINITE);
			CloseHandle(hThread);
		}
		/*
			2 reasons to get here Thread process finally completed
			or we failed to retrieve the UIThread Function
		*/
		FreeLibrary(InterfaceModule);
	} else { // failed to load the interface
		ExitProcess(EXIT_FAILURE);
	}
	/*
		last chance to cleanly kill your application
	*/
	ExitProcess(EXIT_SUCCESS);
}