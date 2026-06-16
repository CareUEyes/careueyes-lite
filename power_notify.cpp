#include "stdafx.h"
#include "power_notify.h"
#include "WinNT.h"

BOOL power_notify::IsPowerOn(DWORD dwPowerEvent, DWORD_PTR dwData)
{
    if(dwPowerEvent != PBT_POWERSETTINGCHANGE)
        return FALSE;

    if (IsBadReadPtr((LPVOID)dwData, sizeof(POWERBROADCAST_SETTING)))
        return FALSE;

    POWERBROADCAST_SETTING* pps= (POWERBROADCAST_SETTING*)dwData;
    if(sizeof(DWORD) != pps->DataLength)
        return FALSE;

    if (pps->PowerSetting != GUID_MONITOR_POWER_ON && pps->PowerSetting != GUID_CONSOLE_DISPLAY_STATE && pps->PowerSetting != GUID_SESSION_DISPLAY_STATUS)
        return FALSE;

    DWORD dwStatus = *((DWORD*)pps->Data);
    return (dwStatus != 0);
}

BOOL power_notify::UnRegisterPowerNotify(HPOWERNOTIFY hPowerNotify)
{
    HMODULE hModule = GetModuleHandleW(L"user32.dll");
    if (NULL == hModule)
        return FALSE;

    UnregisterPowerSettingNotification_Type pfnUnRegisterPowerNotify = (UnregisterPowerSettingNotification_Type)GetProcAddress(hModule, "UnregisterPowerSettingNotification");
    if (NULL == pfnUnRegisterPowerNotify)
        return FALSE;

    return pfnUnRegisterPowerNotify(hPowerNotify);
}

BOOL power_notify::RegisterPowerNotify(HWND hWnd, GUID powerID, HPOWERNOTIFY& hPowerNotify)
{
    HMODULE hModule = GetModuleHandleW(L"user32.dll");
    if (NULL == hModule)
        return FALSE;
    RegisterPowerSettingNotification_Type pfnRegisterPowerNotify = (RegisterPowerSettingNotification_Type)GetProcAddress(hModule, "RegisterPowerSettingNotification");
    if (NULL == pfnRegisterPowerNotify)
        return FALSE;
    GUID id = powerID;
    hPowerNotify = pfnRegisterPowerNotify(hWnd, &id, DEVICE_NOTIFY_WINDOW_HANDLE);
    if (hPowerNotify != NULL)
        return TRUE;
    return FALSE;
}
