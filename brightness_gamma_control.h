#pragma once

class CBrightnessGammaControl
{
public:
    CBrightnessGammaControl();
    ~CBrightnessGammaControl();

    BOOL RestoreDefault();
    BOOL SetBrightness(HDC hDC, int nBrightness);

private:
    typedef BOOL (WINAPI *InternalSetDeviceGammaRamp_Type)(HDC hDC, LPVOID lpRamp, BOOL a);

    static BOOL CALLBACK MonitorEnumProc_(HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM dwData);
    BOOL InitInternalSetDeviceGammaRamp_();
    BOOL SetBrightnessGamma_(HDC hDC, int nBrightness);
    BOOL SetAllDisplaysBrightness_(int nBrightness);
    BOOL SetDeviceGammaRampWrapper_(HDC hDC, LPVOID lpRamp);

private:
    InternalSetDeviceGammaRamp_Type m_pInternalSetDeviceGammaRamp;
    int m_nEnumBrightness;
    BOOL m_bEnumApplied;
};
