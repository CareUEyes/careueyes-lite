#pragma once
#include <windows.h>

class CSetting
{
public:
    static CSetting* GetInstance();

    void SaveSetting();
    BOOL SetBrightness(int nBrightness);
    BOOL GetBrightness(int& nBrightness);

private:
    CSetting();
    void ReadSettings();

private:
    int m_nBrightness;
    WCHAR m_szProfilePath[MAX_PATH];
};
