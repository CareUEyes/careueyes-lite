#pragma once
#include "windows.h"

#define MAINAPP_INSTANCE_UUID  L"{D6E944C9-5D80-4634-A9AB-A3886AFDCC0A}"

namespace OneInstace
{
    BOOL CreateOneInstance(LPCWSTR lpName);
    BOOL SetMainAppInstance(HWND hWnd, LPCWSTR lpName);
    BOOL IsRunning(LPCWSTR lpName);
    BOOL NotifyInstanceExit(LPCWSTR lpName);
}
