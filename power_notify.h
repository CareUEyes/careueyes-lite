#pragma once
#include "windows.h"
#include "WinUser.h"
#include "WinNT.h"

#ifndef PBT_POWERSETTINGCHANGE
#define PBT_POWERSETTINGCHANGE 0x8013
#endif

#ifndef DEVICE_NOTIFY_WINDOW_HANDLE
#define DEVICE_NOTIFY_WINDOW_HANDLE 0x00000000
#endif

#ifndef HPOWERNOTIFY
typedef PVOID HPOWERNOTIFY;
#endif

#ifndef _POWERBROADCAST_SETTING_DEFINED
#define _POWERBROADCAST_SETTING_DEFINED
typedef struct {
    GUID PowerSetting;
    DWORD DataLength;
    UCHAR Data[1];
} POWERBROADCAST_SETTING, *PPOWERBROADCAST_SETTING;
#endif


namespace power_notify
{
#define DEFINE_GUID_(name, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8) \
    const GUID name \
    = { l, w1, w2, { b1, b2,  b3,  b4,  b5,  b6,  b7,  b8 } }

    DEFINE_GUID_(GUID_MONITOR_POWER_ON, 0x02731015, 0x4510, 0x4526, 0x99, 0xe6, 0xe5, 0xa1, 0x7e, 0xbd, 0x1a, 0xea);
    DEFINE_GUID_(GUID_CONSOLE_DISPLAY_STATE, 0x6fe69556, 0x704a, 0x47a0, 0x8f, 0x24, 0xc2, 0x8d, 0x93, 0x6f, 0xda, 0x47);
    DEFINE_GUID_(GUID_SESSION_DISPLAY_STATUS, 0x2B84C20E, 0xAD23, 0x4ddf, 0x93, 0xDB, 0x05, 0xFF, 0xBD, 0x7E, 0xFC, 0xA5);
    //2B84C20E-AD23-4ddf-93DB-05FFBD7EFCA5
    typedef HPOWERNOTIFY (__stdcall *RegisterPowerSettingNotification_Type)(HANDLE hRecipient,LPCGUID PowerSettingGuid,DWORD Flags);
    typedef BOOL (__stdcall *UnregisterPowerSettingNotification_Type)(HPOWERNOTIFY Handle);
    BOOL IsPowerOn(DWORD dwPowerEvent, DWORD_PTR dwData);

    BOOL UnRegisterPowerNotify(HPOWERNOTIFY hPowerNotify);
    BOOL RegisterPowerNotify(HWND hWnd, GUID powerID, HPOWERNOTIFY& hPowerNotify);
}
