// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#pragma warning(disable: 4996)

#define WINVER          0x0500
#define _WIN32_WINNT    0x0502
#define _WIN32_IE       0x0501
#define _RICHEDIT_VER   0x0200

#define _CONVERSION_DONT_USE_THREAD_LOCALE
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <assert.h>
#include <list>
#include <algorithm>
#include <vector>
#include <windows.h>
#include <shellapi.h>
#include <shlwapi.h>
#include <atlbase.h>
#include <atlstr.h>
#include <atlpath.h>
#include <atlconv.h>

#pragma comment(lib, "shlwapi.lib")
#pragma comment(lib, "shell32.lib")
