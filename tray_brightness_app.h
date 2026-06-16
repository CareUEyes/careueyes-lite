#pragma once

#include "brightness_gamma_control.h"
#include "power_notify.h"

class CTrayBrightnessApp
{
public:
    CTrayBrightnessApp();
    ~CTrayBrightnessApp();

    bool Initialize(HINSTANCE hInstance);
    int Run();

private:
    static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
    LRESULT HandleMessage(UINT message, WPARAM wParam, LPARAM lParam);

    bool CreateHiddenWindow_();
    void InitIcons_();
    void AddTrayIcon_(BOOL bNotify);
    void UpdateTrayIcon_();
    void RemoveTrayIcon_();
    void TogglePause_();
    void ApplyBrightness_(BOOL bRestore);
    void ShowTrayMenu_();
    void RegisterSystemNotify_();
    void RegisterPowerOnNotify_();
    void UnRegisterPowerOnNotify_();
    void RestoreBrightnessLater_(DWORD dwDelayMS);
    void Shutdown_();
    void UpdateMenuChecks_(HMENU hMenu);

private:
    HINSTANCE m_hInstance;
    HWND m_hWnd;
    HICON m_hIcon;
    HICON m_hIconPause;
    HICON m_hIconCur;
    UINT m_taskbarRestartMessage;
    HPOWERNOTIFY m_hMonitorPowerOn;
    HPOWERNOTIFY m_hConsoleDisplayState;
    HPOWERNOTIFY m_hSessionDisplayState;
    BOOL m_bPause;
    BOOL m_bShuttingDown;
    CBrightnessGammaControl m_gammaControl;
};
