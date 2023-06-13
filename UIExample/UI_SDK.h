#if !defined(UIDLLSDK_HEADER)
#define UIDLLSDK_HEADER
#pragma once

/*

	This SDK is for use with the Interface.SUI
	
	if there have been any updates to the interface
	check that the WM_ messages are still relevent

*/
#define UISDK_VERSION 4

#pragma region "Init message"
struct MyMessageData // our initialize message
{
	HANDLE hEvent;
	DWORD fromThreadID; // id of the main thread so we can send messages back
};
#pragma endregion

#define WM_INITIALIZE        (WM_USER + 1001) //main thread giving its info for initialization
#define WM_MYUI_MESSAGE      (WM_USER + 1002) //ui to gui
#define WM_UIINITIALIZED     (WM_USER + 1003) //ui telling the main thread its up and running
#define WM_UIINITIALIZED_ACK (WM_USER + 1004) //main thread acknoledging the init

#define WM_UIADDCHAT         (WM_USER + 1005) //ui thread to the gui to add chat
#define WM_UIPOPADDCHAT      (WM_USER + 1006) //ui telling the main thread to pop the message
#define WM_MTREQADDCHAT      (WM_USER + 1007) //Main thread request add chat

#define WM_UIMESSAGEINPUT    (WM_USER + 1008) // Message input from the editbox
#define WM_INPUTCOMPLETED    (WM_USER + 1009) // Main Thread tells UI its ready for more input (if any exists).

#define WM_UIADDTEXTA        (WM_USER + 1010) // sAddTextA
#define WM_UIADDTEXTW        (WM_USER + 1011) // sAddTextW
#define WM_POPADDTEXTA       (WM_USER + 1012) // UI completed its sAddTextA
#define WM_POPADDTEXTW       (WM_USER + 1013) // UI completed its sAddTextW

#define WM_MENU_UPDATE              (WM_USER + 1014) // UI->Thread (UI telling point of origin menu command data [Name + id])
#define WM_MENU_COMMAND             (WM_USER + 1015)
#define WM_MENU_ADDCOMMANDA         (WM_USER + 1016) // thread->ui add menu command item
#define WM_MENU_ADDCOMMANDW         (WM_USER + 1017) // thread->ui add menu command item
#define WM_MENU_ADDSUBMENUA         (WM_USER + 1018) // thread->ui add submenu item
#define WM_MENU_ADDSUBMENUW         (WM_USER + 1019) // thread->ui add submenu item
#define WM_MENU_ADDSUBMENUCOMMANDA  (WM_USER + 1020) // thread->ui add submenu command
#define WM_MENU_ADDSUBMENUCOMMANDW  (WM_USER + 1021) // thread->ui add submenu command
#define WM_MENU_ADDSUBMENUSUBMENUA  (WM_USER + 1022) // thread->ui add submenu of submenu
#define WM_MENU_ADDSUBMENUSUBMENUW  (WM_USER + 1023) // thread->ui add submenu of submenu

#define WM_WINDOW_CAPTIONA          (WM_USER + 1024) // thread->ui set window caption
#define WM_WINDOW_CAPTIONW          (WM_USER + 1025) // thread->ui set window caption

#pragma region "COLORS"
enum class colors : uint32_t {
	black,
	green,
	silver,
	lime,
	seagreen,
	gray,
	darkslategray,
	olive,
	white,
	yellow,
	maroon,
	navy,
	red,
	blue,
	purple,
	teal,
	fuchsia,
	aqua,
	deeppurple
};
inline COLORREF ColorByName(const colors name)
{
	switch (name) {
	case colors::black: { return 0x000000L; } //Black
	case colors::green: { return 0x008000L; } //Green
	case colors::silver: { return 0xc0c0c0L; } //Silver 0xc0c0c0L
	case colors::lime: { return 0x00ff00L; } //Lime 
	case colors::seagreen: { return 0x32CD32L; } //seagreen 0x80CD43L 65B0161C
	case colors::gray: { return 0x808080L; } //Gray //old 0x808080L
	case colors::darkslategray: { return 0x2F4F4FL; } //DarkSlateGray
	case colors::olive: { return 0x008080L; } //Olive
	case colors::white: { return 0xffffffL; } //White
	case colors::yellow: { return 0x00CDCDL; } //Yellow //old 0x00ffffL //gold3 0x00ADCDL
	case colors::maroon: { return 0x000080L; } //Maroon
	case colors::navy: { return 0x800000L; } //Navy
	case colors::red: { return 0x0000eeL; } //Red
	case colors::blue: { return 0xCD5F3AL; } //Blue //0xCD5F3AL //old 0xff0000L (to much blur on a black background)
	case colors::purple: { return 0x800080L; } //Purple
	case colors::teal: { return 0x808000L; } //Teal
	case colors::fuchsia: { return 0xff00ffL; } //Fuchsia 
	case colors::aqua: { return 0xffff00L; } //Aqua 
	case colors::deeppurple: { return 0xDB7093L; } //deeppurple 
	default: {return (COLORREF)0x000000L; }
	} //orange = 0x3A5FCDL
}
#pragma endregion

#pragma region "ADD CHAT STRUCT'S (CURRENTLY ONLY WIDE)"
// if more is needed the UI will need an update
#define MAX_ADDTEXTBUFFER 1024

/* just a prototype for ui message ping pongs */
/*
	This is WIDESTR only and is only here if your 
	wanting to test colors or make additions to 
	the color listing, do note that if you add a
	color put them at the bottom of the enum class

*/
struct sAddChat {
	uint32_t messagecounter{ 0 };
	std::vector<COLORREF> forColorList{};
	std::vector<COLORREF> bakColorList{};
	std::vector<uiString> MessageList{};

#ifdef TEST_ARRAY_DESTRUCT
	~test() {
		FreeMessages();
		forColorList.~vector();
		bakColorList.~vector();
		MessageList.~vector();
	}
#endif

	/// <summary>
	/// Default white text on black background
	/// </summary>
	/// <param name="format"></param>
	/// <param name=""></param>
	void FmtMessage(const uiChar* format, ...) {
		std::vector<uiChar> buffer{};
		buffer.resize(256);
		va_list args;
		va_start(args, format);
#ifdef UNICODE
		/* vswprintf was changed to conform with the ISO C standard (buffer size) */
		vswprintf(buffer.data(), buffer.size(), format, args);
#else
		/* mb version of the above */
		vsprintf_s(buffer.data(), buffer.size(), format, args);
#endif
		va_end(args);

		/* add color to list */
		forColorList.emplace_back(ColorByName(colors::white));
		bakColorList.emplace_back(ColorByName(colors::black));
		/* add message to the list */
		MessageList.emplace_back(uiString(buffer.data()));
		messagecounter++;
	}

	/// <summary>
	/// Default black background
	/// </summary>
	/// <param name="forcolor"></param>
	/// <param name="format"></param>
	/// <param name=""></param>
	void FmtMessage(const colors forcolor, const uiChar* format, ...) {
		std::vector<uiChar> buffer{};
		buffer.resize(256);
		va_list args;
		va_start(args, format);
#ifdef UNICODE
		/* vswprintf was changed to conform with the ISO C standard (buffer size) */
		vswprintf(buffer.data(), buffer.size(), format, args);
#else
		/* mb version of the above */
		vsprintf_s(buffer.data(), buffer.size(), format, args);
#endif
		va_end(args);

		/* add color to list */
		forColorList.emplace_back(ColorByName(forcolor));
		bakColorList.emplace_back(ColorByName(colors::black));
		/* add message to the list */
		MessageList.emplace_back(uiString(buffer.data()));
		messagecounter++;
	}

	/// <summary>
	/// no default values they must be set
	/// </summary>
	/// <param name="forcolor"></param>
	/// <param name="backcolor"></param>
	/// <param name="format"></param>
	/// <param name=""></param>
	void FmtMessage(const colors forcolor, const colors backcolor, const uiChar* format, ...)
	{
		std::vector<uiChar> buffer{};
		buffer.resize(256);
		va_list args;
		va_start(args, format);
#ifdef UNICODE
		/* vswprintf was changed to conform with the ISO C standard (buffer size) */
		vswprintf(buffer.data(), buffer.size(), format, args);
#else
		/* mb version of the above */
		vsprintf_s(buffer.data(), buffer.size(), format, args);
#endif
		va_end(args);

		/* add color to list */
		forColorList.emplace_back(ColorByName(forcolor));
		bakColorList.emplace_back(ColorByName(backcolor));
		/* add message to the list */
		MessageList.emplace_back(uiString(buffer.data()));
		messagecounter++;
	}

	/// <summary>
	/// Clear the message arrays
	/// </summary>
	void FreeMessages() {
		while (MessageList.size() > 0 && forColorList.size() > 0 && bakColorList.size() > 0)
		{
			forColorList.pop_back();
			bakColorList.pop_back();
			MessageList.pop_back();
			messagecounter--;
		}
		forColorList.clear();
		bakColorList.clear();
		MessageList.clear();
	}
};

struct sAddTextA {
	uint32_t messagecounter{ 0 };
	std::vector<colors> forColorList{};
	std::vector<colors> bakColorList{};
	std::vector<std::string> MessageList{};

	// w on b default
	void FmtMessage(const char* format, ...) {
		std::vector<char> buffer{};
		buffer.resize(MAX_ADDTEXTBUFFER);
		va_list args;
		va_start(args, format);
		vsprintf_s(buffer.data(), (MAX_ADDTEXTBUFFER - 1), format, args);
		va_end(args);

		/* add color to list */
		forColorList.emplace_back(colors::white);
		bakColorList.emplace_back(colors::black);
		/* add message to the list */
		MessageList.emplace_back(std::string(buffer.data()));
		messagecounter++;
	}

	// for on b default
	void FmtMessage(const colors forcolor, const char* format, ...) {
		std::vector<char> buffer{};
		buffer.resize(MAX_ADDTEXTBUFFER);
		va_list args;
		va_start(args, format);
		vsprintf_s(buffer.data(), (MAX_ADDTEXTBUFFER - 1), format, args);
		va_end(args);

		/* add color to list */
		forColorList.emplace_back(forcolor);
		bakColorList.emplace_back(colors::black);
		/* add message to the list */
		MessageList.emplace_back(std::string(buffer.data()));
		messagecounter++;
	}

	void FmtMessage(const colors forcolor, const colors backcolor, const char* format, ...)
	{
		std::vector<char> buffer{};
		buffer.resize(MAX_ADDTEXTBUFFER);
		va_list args;
		va_start(args, format);
		vsprintf_s(buffer.data(), (MAX_ADDTEXTBUFFER - 1), format, args);
		va_end(args);

		/* add color to list */
		forColorList.emplace_back(forcolor);
		bakColorList.emplace_back(backcolor);
		/* add message to the list */
		MessageList.emplace_back(std::string(buffer.data()));
		messagecounter++;
	}

};

struct sAddTextW {
	uint32_t messagecounter{ 0 };
	std::vector<colors> forColorList{};
	std::vector<colors> bakColorList{};
	std::vector<std::wstring> MessageList{};

	// w on b default
	void FmtMessage(const wchar_t* format, ...) {
		std::vector<wchar_t> buffer{};
		buffer.resize(MAX_ADDTEXTBUFFER);
		va_list args;
		va_start(args, format);
		vswprintf(buffer.data(), (MAX_ADDTEXTBUFFER - 1), format, args);
		va_end(args);

		/* add color to list */
		forColorList.emplace_back(colors::white);
		bakColorList.emplace_back(colors::black);
		/* add message to the list */
		MessageList.emplace_back(std::wstring(buffer.data()));
		messagecounter++;
	}

	// for on b default
	void FmtMessage(const colors forcolor, const wchar_t* format, ...) {
		std::vector<wchar_t> buffer{};
		buffer.resize(MAX_ADDTEXTBUFFER);
		va_list args;
		va_start(args, format);
		vswprintf(buffer.data(), (MAX_ADDTEXTBUFFER - 1), format, args);
		va_end(args);

		/* add color to list */
		forColorList.emplace_back(forcolor);
		bakColorList.emplace_back(colors::black);
		/* add message to the list */
		MessageList.emplace_back(std::wstring(buffer.data()));
		messagecounter++;
	}

	void FmtMessage(const colors forcolor, const colors backcolor, const wchar_t* format, ...)
	{
		std::vector<wchar_t> buffer{};
		buffer.resize(MAX_ADDTEXTBUFFER);
		va_list args;
		va_start(args, format);
		vswprintf(buffer.data(), (MAX_ADDTEXTBUFFER - 1), format, args);
		va_end(args);

		/* add color to list */
		forColorList.emplace_back(forcolor);
		bakColorList.emplace_back(backcolor);
		/* add message to the list */
		MessageList.emplace_back(std::wstring(buffer.data()));
		messagecounter++;
	}

};

#pragma endregion

#pragma region "EDITBOX PROCESS MESSAGES"
struct sMessageBoxText {
	std::string  MessageA{};
	std::wstring MessageW{};
};
#pragma endregion

#pragma region "SUBMENU//COMMAND||SUBMENU"
struct sSubMenuItemA {
	std::string submenu{};
	std::string item{};
};
struct sSubMenuItemW {
	std::wstring submenu{};
	std::wstring item{};
};
#pragma endregion

#endif