#include "stdafx.h"
#include "oneinstance.h"

namespace
{
    const UINT WM_NOTIFY_MAINAPP_EXIT = WM_USER + 1003;

    BOOL GetInstanceWindow(LPCWSTR lpName, HWND& hWnd)
    {
        hWnd = NULL;
        HANDLE hMap = OpenFileMappingW(FILE_MAP_ALL_ACCESS, FALSE, lpName);
        if (hMap == NULL)
            return FALSE;

        LPVOID lpBuf = MapViewOfFile(hMap, FILE_MAP_ALL_ACCESS, 0, 0, 0);
        BOOL bRetVal = FALSE;
        if (lpBuf != NULL)
        {
            hWnd = (HWND)*((DWORD_PTR*)lpBuf);
            bRetVal = IsWindow(hWnd);
            if (!bRetVal)
                hWnd = NULL;
            UnmapViewOfFile(lpBuf);
        }

        CloseHandle(hMap);
        return bRetVal;
    }
}

namespace OneInstace
{
    BOOL CreateOneInstance(LPCWSTR lpName)
    {
        HANDLE hMap = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, sizeof(DWORD_PTR), lpName);
        return hMap != NULL;
    }

    BOOL SetMainAppInstance(HWND hWnd, LPCWSTR lpName)
    {
        if (hWnd == NULL || !IsWindow(hWnd))
            return FALSE;

        HANDLE hMap = OpenFileMappingW(FILE_MAP_ALL_ACCESS, FALSE, lpName);
        if (hMap == NULL)
            return FALSE;

        LPVOID lpBuf = MapViewOfFile(hMap, FILE_MAP_READ | FILE_MAP_WRITE, 0, 0, 0);
        BOOL bRetVal = FALSE;
        if (lpBuf != NULL)
        {
            *((DWORD_PTR*)lpBuf) = reinterpret_cast<DWORD_PTR>(hWnd);
            bRetVal = TRUE;
            UnmapViewOfFile(lpBuf);
        }

        CloseHandle(hMap);
        return bRetVal;
    }

    BOOL IsRunning(LPCWSTR lpName)
    {
        HANDLE hMap = OpenFileMappingW(FILE_MAP_ALL_ACCESS, FALSE, lpName);
        if (hMap == NULL)
            return FALSE;
        CloseHandle(hMap);
        return TRUE;
    }

    BOOL NotifyInstanceExit(LPCWSTR lpName)
    {
        HWND hWnd = NULL;
        if (!GetInstanceWindow(lpName, hWnd))
            return FALSE;
        SendMessage(hWnd, WM_NOTIFY_MAINAPP_EXIT, 0, 0);
        return TRUE;
    }
}
