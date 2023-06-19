#pragma once

#include "TargetVersion.h"
#include <windows.h>

#include <cstdlib>
#include <cstdio>
#include <ctime>
#include <cmath>
#include <memory>
#include <string>
#include <vector>
#include <deque>
#include <mutex>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <map>
#include <utility>
#include <algorithm>
//#include "resource.h"

#define uiNewLine TEXT("\r\n")
#ifdef UNICODE
	#define uiString std::wstring
	#define uiChar wchar_t
#else
	#define uiString std::string
	#define uiChar char
#endif

#include "SDK.h"
