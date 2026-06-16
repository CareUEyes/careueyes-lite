#include "stdafx.h"
#include "tray_brightness_app.h"

#include "resource.h"
#include "power_notify.h"
#include "oneinstance.h"
#include "setting.h"
#include <WtsApi32.h>

namespace
{
    const wchar_t kWindowClassName[] = L"CareUEyesOpenSourceBrightnessWindow";
    const wchar_t kWindowPropName[] = L"CareUEyesBrightnessApp";
    const wchar_t kTrayTip[] = L"CareUEyes Brightness";
    const wchar_t kRunKeyPath[] = L"Software\\Microsoft\\Windows\\CurrentVersion\\Run";
    const wchar_t kRunValueName[] = L"CareUEyes Lite";
    const UINT WM_NOTIFY_MAINAPP_EXIT = WM_USER + 1003;
    const UINT WM_TRAY_CLICK = WM_USER + 1013;
    const UINT EVENT_ID_RESTORE_BRIGHTNESS = 1890;

    BOOL UpdateTray(HWND hWnd, HICON hIcon, UINT uMessage, DWORD dwAction, LPCWSTR lpTip, LPCWSTR lpInfo)
    {
        if (!IsWindow(hWnd))
            return FALSE;

        NOTIFYICONDATAW data = {0};
        data.cbSize = sizeof(NOTIFYICONDATAW);
        data.hWnd = hWnd;
        data.uID = 111;
        data.uFlags = NIF_ICON | NIF_MESSAGE;
        data.uCallbackMessage = uMessage;
        data.hIcon = hIcon;
        if (lpTip != NULL)
        {
            data.uFlags |= NIF_TIP;
            wcsncpy_s(data.szTip, _countof(data.szTip), lpTip, _TRUNCATE);
        }
        if (lpInfo != NULL)
        {
            data.uFlags |= NIF_INFO;
            wcsncpy_s(data.szInfo, _countof(data.szInfo), lpInfo, _TRUNCATE);
            data.dwInfoFlags = NIIF_INFO;
        }
        return Shell_NotifyIconW(dwAction, &data);
    }
}

CTrayBrightnessApp::CTrayBrightnessApp()
    : m_hInstance(NULL),
      m_hWnd(NULL),
      m_hIcon(NULL),
      m_hIconPause(NULL),
      m_hIconCur(NULL),
      m_taskbarRestartMessage(0),
      m_hMonitorPowerOn(NULL),
      m_hConsoleDisplayState(NULL),
      m_hSessionDisplayState(NULL),
      m_bPause(FALSE),
      m_bShuttingDown(FALSE)
{
}

CTrayBrightnessApp::~CTrayBrightnessApp()
{
}

bool CTrayBrightnessApp::Initialize(HINSTANCE hInstance)
{
    m_hInstance = hInstance;
    InitIcons_();
    if (!CreateHiddenWindow_())
        return false;

    OneInstace::SetMainAppInstance(m_hWnd, MAINAPP_INSTANCE_UUID);
    RegisterSystemNotify_();
    SyncStartupState_();
    AddTrayIcon_(TRUE);
    ApplyBrightness_(TRUE);
    return true;
}

int CTrayBrightnessApp::Run()
{
    MSG msg = {0};
    while (GetMessage(&msg, NULL, 0, 0) > 0)
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return static_cast<int>(msg.wParam);
}

LRESULT CALLBACK CTrayBrightnessApp::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    CTrayBrightnessApp* self = reinterpret_cast<CTrayBrightnessApp*>(GetPropW(hWnd, kWindowPropName));
    if (message == WM_NCCREATE)
    {
        CREATESTRUCTW* createStruct = reinterpret_cast<CREATESTRUCTW*>(lParam);
        self = reinterpret_cast<CTrayBrightnessApp*>(createStruct->lpCreateParams);
        SetPropW(hWnd, kWindowPropName, self);
        self->m_hWnd = hWnd;
    }

    if (self != NULL)
        return self->HandleMessage(message, wParam, lParam);
    return DefWindowProcW(hWnd, message, wParam, lParam);
}

LRESULT CTrayBrightnessApp::HandleMessage(UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case ID_BRIGHTNESS_20:
        case ID_BRIGHTNESS_40:
        case ID_BRIGHTNESS_60:
        case ID_BRIGHTNESS_80:
        case ID_BRIGHTNESS_100:
            {
                const int brightness = LOWORD(wParam) == ID_BRIGHTNESS_20 ? 20 :
                    LOWORD(wParam) == ID_BRIGHTNESS_40 ? 40 :
                    LOWORD(wParam) == ID_BRIGHTNESS_60 ? 60 :
                    LOWORD(wParam) == ID_BRIGHTNESS_80 ? 80 : 100;
                CSetting::GetInstance()->SetBrightness(brightness);
                CSetting::GetInstance()->SaveSetting();
                m_bPause = FALSE;
                m_hIconCur = m_hIcon;
                ApplyBrightness_(FALSE);
                UpdateTrayIcon_();
            }
            return 0;
        case ID__EXIT:
            Shutdown_();
            return 0;
        case ID_LAUNCH_AT_STARTUP:
            {
                BOOL bEnabled = TRUE;
                CSetting::GetInstance()->GetStartupEnabled(bEnabled);
                const BOOL bNewEnabled = bEnabled ? FALSE : TRUE;
                if (SetLaunchAtStartup_(bNewEnabled))
                {
                    CSetting::GetInstance()->SetStartupEnabled(bNewEnabled);
                    CSetting::GetInstance()->SaveSetting();
                }
            }
            return 0;
        case ID_UPGRADE_TO_PRO:
            ShellExecuteW(NULL, L"open", L"https://care-eyes.com/?lite", NULL, NULL, SW_SHOWNORMAL);
            return 0;
        default:
            break;
        }
        break;
    case WM_NOTIFY_MAINAPP_EXIT:
        Shutdown_();
        return 0;
    case WM_TRAY_CLICK:
        if (lParam == WM_LBUTTONDOWN)
        {
            TogglePause_();
            return 0;
        }
        if (lParam == WM_RBUTTONDOWN)
        {
            ShowTrayMenu_();
            return 0;
        }
        break;
    case WM_TIMER:
        if (wParam == EVENT_ID_RESTORE_BRIGHTNESS)
        {
            KillTimer(m_hWnd, EVENT_ID_RESTORE_BRIGHTNESS);
            ApplyBrightness_(FALSE);
            return 0;
        }
        break;
    case WM_POWERBROADCAST:
        if (power_notify::IsPowerOn(static_cast<DWORD>(wParam), static_cast<DWORD_PTR>(lParam)) ||
            wParam == PBT_APMRESUMEAUTOMATIC)
        {
            RestoreBrightnessLater_(1000);
        }
        return TRUE;
    case WM_WTSSESSION_CHANGE:
        if (wParam != WTS_SESSION_LOCK)
            RestoreBrightnessLater_(2000);
        return 0;
    case WM_DESTROY:
        RemovePropW(m_hWnd, kWindowPropName);
        PostQuitMessage(0);
        return 0;
    default:
        if (message == m_taskbarRestartMessage)
        {
            AddTrayIcon_(FALSE);
            return 0;
        }
        break;
    }

    return DefWindowProcW(m_hWnd, message, wParam, lParam);
}

bool CTrayBrightnessApp::CreateHiddenWindow_()
{
    WNDCLASSEXW wc = {0};
    wc.cbSize = sizeof(wc);
    wc.lpfnWndProc = CTrayBrightnessApp::WndProc;
    wc.hInstance = m_hInstance;
    wc.hIcon = m_hIcon;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.lpszClassName = kWindowClassName;

    if (RegisterClassExW(&wc) == 0 && GetLastError() != ERROR_CLASS_ALREADY_EXISTS)
        return false;

    HWND hWnd = CreateWindowExW(
        0,
        kWindowClassName,
        kTrayTip,
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        NULL,
        NULL,
        m_hInstance,
        this);
    if (hWnd == NULL)
        return false;

    ShowWindow(hWnd, SW_HIDE);
    UpdateWindow(hWnd);
    return true;
}

void CTrayBrightnessApp::InitIcons_()
{
    m_hIcon = LoadIconW(m_hInstance, MAKEINTRESOURCEW(IDI_ICON1));
    m_hIconPause = LoadIconW(m_hInstance, MAKEINTRESOURCEW(IDI_ICON2));
    m_hIconCur = m_hIcon;
}

void CTrayBrightnessApp::AddTrayIcon_(BOOL bNotify)
{
    LPCWSTR info = bNotify ? L"Brightness control is running in the tray." : NULL;
    UpdateTray(m_hWnd, m_hIconCur, WM_TRAY_CLICK, NIM_ADD, kTrayTip, info);
}

void CTrayBrightnessApp::UpdateTrayIcon_()
{
    const LPCWSTR tip = m_bPause ? L"CareUEyes Brightness (Paused)" : kTrayTip;
    UpdateTray(m_hWnd, m_hIconCur, WM_TRAY_CLICK, NIM_MODIFY, tip, NULL);
}

void CTrayBrightnessApp::RemoveTrayIcon_()
{
    UpdateTray(m_hWnd, m_hIconCur, WM_TRAY_CLICK, NIM_DELETE, kTrayTip, NULL);
}

void CTrayBrightnessApp::TogglePause_()
{
    m_bPause = !m_bPause;
    if (m_bPause)
    {
        m_gammaControl.RestoreDefault();
        m_hIconCur = m_hIconPause;
    }
    else
    {
        m_hIconCur = m_hIcon;
        ApplyBrightness_(FALSE);
    }
    UpdateTrayIcon_();
}

void CTrayBrightnessApp::ApplyBrightness_(BOOL bRestore)
{
    if (m_bPause)
        return;

    if (bRestore)
        m_gammaControl.RestoreDefault();

    int brightness = 80;
    CSetting::GetInstance()->GetBrightness(brightness);
    m_gammaControl.SetBrightness(NULL, brightness);
}

void CTrayBrightnessApp::ShowTrayMenu_()
{
    HMENU hMenu = LoadMenuW(m_hInstance, MAKEINTRESOURCEW(IDR_TRAYMENU));
    if (hMenu == NULL)
        return;

    HMENU hPopup = GetSubMenu(hMenu, 0);
    if (hPopup != NULL)
    {
        UpdateMenuChecks_(hPopup);
        POINT pt = {0};
        GetCursorPos(&pt);
        SetForegroundWindow(m_hWnd);
        TrackPopupMenu(hPopup, TPM_LEFTALIGN | TPM_RIGHTBUTTON, pt.x, pt.y, 0, m_hWnd, NULL);
        PostMessageW(m_hWnd, WM_NULL, 0, 0);
    }

    DestroyMenu(hMenu);
}

void CTrayBrightnessApp::RegisterSystemNotify_()
{
    WTSRegisterSessionNotification(m_hWnd, NOTIFY_FOR_ALL_SESSIONS);
    m_taskbarRestartMessage = RegisterWindowMessageW(L"TaskbarCreated");
    RegisterPowerOnNotify_();
}

void CTrayBrightnessApp::RegisterPowerOnNotify_()
{
    power_notify::RegisterPowerNotify(m_hWnd, power_notify::GUID_MONITOR_POWER_ON, m_hMonitorPowerOn);
    power_notify::RegisterPowerNotify(m_hWnd, power_notify::GUID_CONSOLE_DISPLAY_STATE, m_hConsoleDisplayState);
    power_notify::RegisterPowerNotify(m_hWnd, power_notify::GUID_SESSION_DISPLAY_STATUS, m_hSessionDisplayState);
}

void CTrayBrightnessApp::UnRegisterPowerOnNotify_()
{
    if (m_hMonitorPowerOn != NULL)
    {
        power_notify::UnRegisterPowerNotify(m_hMonitorPowerOn);
        m_hMonitorPowerOn = NULL;
    }
    if (m_hConsoleDisplayState != NULL)
    {
        power_notify::UnRegisterPowerNotify(m_hConsoleDisplayState);
        m_hConsoleDisplayState = NULL;
    }
    if (m_hSessionDisplayState != NULL)
    {
        power_notify::UnRegisterPowerNotify(m_hSessionDisplayState);
        m_hSessionDisplayState = NULL;
    }
}

void CTrayBrightnessApp::RestoreBrightnessLater_(DWORD dwDelayMS)
{
    SetTimer(m_hWnd, EVENT_ID_RESTORE_BRIGHTNESS, dwDelayMS, NULL);
}

void CTrayBrightnessApp::Shutdown_()
{
    if (m_bShuttingDown)
        return;
    m_bShuttingDown = TRUE;

    WTSUnRegisterSessionNotification(m_hWnd);
    UnRegisterPowerOnNotify_();
    RemoveTrayIcon_();
    m_gammaControl.RestoreDefault();
    DestroyWindow(m_hWnd);
}

void CTrayBrightnessApp::UpdateMenuChecks_(HMENU hMenu)
{
    const UINT brightnessIds[] = {
        ID_BRIGHTNESS_20,
        ID_BRIGHTNESS_40,
        ID_BRIGHTNESS_60,
        ID_BRIGHTNESS_80,
        ID_BRIGHTNESS_100
    };
    for (size_t i = 0; i < sizeof(brightnessIds) / sizeof(brightnessIds[0]); ++i)
        CheckMenuItem(hMenu, brightnessIds[i], MF_BYCOMMAND | MF_UNCHECKED);

    int brightness = 80;
    CSetting::GetInstance()->GetBrightness(brightness);
    UINT checkedId = brightness <= 20 ? ID_BRIGHTNESS_20 :
        brightness <= 40 ? ID_BRIGHTNESS_40 :
        brightness <= 60 ? ID_BRIGHTNESS_60 :
        brightness <= 80 ? ID_BRIGHTNESS_80 :
        ID_BRIGHTNESS_100;
    CheckMenuItem(hMenu, checkedId, MF_BYCOMMAND | MF_CHECKED);

    BOOL bStartupEnabled = TRUE;
    CSetting::GetInstance()->GetStartupEnabled(bStartupEnabled);
    CheckMenuItem(
        hMenu,
        ID_LAUNCH_AT_STARTUP,
        MF_BYCOMMAND | (bStartupEnabled ? MF_CHECKED : MF_UNCHECKED));
}

bool CTrayBrightnessApp::SyncStartupState_()
{
    BOOL bStartupEnabled = TRUE;
    CSetting::GetInstance()->GetStartupEnabled(bStartupEnabled);
    return SetLaunchAtStartup_(bStartupEnabled);
}

bool CTrayBrightnessApp::SetLaunchAtStartup_(BOOL bEnabled)
{
    CRegKey key;
    LONG result = key.Create(HKEY_CURRENT_USER, kRunKeyPath);
    if (result != ERROR_SUCCESS)
        return false;

    if (bEnabled)
    {
        CStringW modulePath;
        if (!GetModulePath_(modulePath))
            return false;

        CStringW quotedPath;
        quotedPath.Format(L"\"%s\"", static_cast<LPCWSTR>(modulePath));
        result = key.SetStringValue(kRunValueName, quotedPath);
        return result == ERROR_SUCCESS;
    }

    result = key.DeleteValue(kRunValueName);
    return result == ERROR_SUCCESS || result == ERROR_FILE_NOT_FOUND;
}

bool CTrayBrightnessApp::GetModulePath_(CStringW& modulePath) const
{
    WCHAR szPath[MAX_PATH] = {0};
    const DWORD length = GetModuleFileNameW(NULL, szPath, _countof(szPath));
    if (length == 0 || length >= _countof(szPath))
        return false;

    modulePath = szPath;
    return true;
}
