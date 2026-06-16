#include "stdafx.h"
#include "brightness_gamma_control.h"

CBrightnessGammaControl::CBrightnessGammaControl()
{
    m_pInternalSetDeviceGammaRamp = NULL;
    m_nEnumBrightness = 100;
    m_bEnumApplied = FALSE;
    InitInternalSetDeviceGammaRamp_();
}

CBrightnessGammaControl::~CBrightnessGammaControl()
{
}

BOOL CBrightnessGammaControl::RestoreDefault()
{
    return SetAllDisplaysBrightness_(100);
}

BOOL CBrightnessGammaControl::SetBrightness(HDC hDC, int nBrightness)
{
    if (nBrightness < 10)
        nBrightness = 10;
    if (nBrightness > 100)
        nBrightness = 100;
    if (hDC == NULL)
        return SetAllDisplaysBrightness_(nBrightness);
    return SetBrightnessGamma_(hDC, nBrightness);
}

BOOL CBrightnessGammaControl::SetBrightnessGamma_(HDC hDC, int nBrightness)
{
    HDC hGammaDC = hDC;
    if (hGammaDC == NULL)
        hGammaDC = GetDC(NULL);
    if (hGammaDC == NULL)
        return FALSE;

    WORD gammaArray[3][256] = {0};
    for (int i = 0; i < 256; ++i)
    {
        DWORD value = static_cast<DWORD>(i) * 255U * static_cast<DWORD>(nBrightness) / 100U;
        if (value > 65535U)
            value = 65535U;
        gammaArray[0][i] = static_cast<WORD>(value);
        gammaArray[1][i] = static_cast<WORD>(value);
        gammaArray[2][i] = static_cast<WORD>(value);
    }

    const BOOL ok = SetDeviceGammaRampWrapper_(hGammaDC, gammaArray);

    if (hDC == NULL)
        ReleaseDC(NULL, hGammaDC);
    return ok;
}

BOOL CALLBACK CBrightnessGammaControl::MonitorEnumProc_(HMONITOR hMonitor, HDC hdcMonitor, LPRECT, LPARAM dwData)
{
    CBrightnessGammaControl* self = reinterpret_cast<CBrightnessGammaControl*>(dwData);
    if (self == NULL)
        return TRUE;

    if (self->SetBrightnessGamma_(hdcMonitor, self->m_nEnumBrightness))
        self->m_bEnumApplied = TRUE;
    return TRUE;
}

BOOL CBrightnessGammaControl::SetAllDisplaysBrightness_(int nBrightness)
{
    m_nEnumBrightness = nBrightness;
    m_bEnumApplied = FALSE;
    HDC hDC = GetDC(NULL);
    if (hDC == NULL)
        return SetBrightnessGamma_(NULL, nBrightness);

    BOOL ok = EnumDisplayMonitors(hDC, NULL, MonitorEnumProc_, reinterpret_cast<LPARAM>(this));
    ReleaseDC(NULL, hDC);
    if (!ok)
        return SetBrightnessGamma_(NULL, nBrightness);
    if (!m_bEnumApplied)
        return SetBrightnessGamma_(NULL, nBrightness);
    return TRUE;
}

BOOL CBrightnessGammaControl::InitInternalSetDeviceGammaRamp_()
{
    HMODULE hDll = LoadLibraryExA("mscms.dll", 0, LOAD_WITH_ALTERED_SEARCH_PATH);
    if (hDll == NULL)
        return FALSE;
    LPVOID lpFn = GetProcAddress(hDll, "InternalSetDeviceGammaRamp");
    if (lpFn != NULL)
        m_pInternalSetDeviceGammaRamp = (InternalSetDeviceGammaRamp_Type)lpFn;
    return TRUE;
}

BOOL CBrightnessGammaControl::SetDeviceGammaRampWrapper_(HDC hDC, LPVOID lpRamp)
{
    if (m_pInternalSetDeviceGammaRamp != NULL)
        return m_pInternalSetDeviceGammaRamp(hDC, lpRamp, 0);
    return SetDeviceGammaRamp(hDC, lpRamp);
}
