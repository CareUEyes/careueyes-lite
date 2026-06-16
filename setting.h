#pragma once
#include <windows.h>

class CSetting
{
public:
    static CSetting* GetInstance();

    void SaveSetting();
    BOOL SetBrightness(int nBrightness);
    BOOL GetBrightness(int& nBrightness);
    BOOL SetStartupEnabled(BOOL bEnabled);
    BOOL GetStartupEnabled(BOOL& bEnabled);

private:
    CSetting();
    void ReadSettings();

private:
    int m_nBrightness;
    BOOL m_bStartupEnabled;
    WCHAR m_szProfilePath[MAX_PATH];
};
