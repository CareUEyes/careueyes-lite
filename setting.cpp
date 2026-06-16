#include "stdafx.h"
#include "setting.h"
#include <shlobj.h>

namespace
{
    const wchar_t kSectionScreen[] = L"screen";
    const wchar_t kKeyBrightness[] = L"brightness";
    const wchar_t kWorkFolderName[] = L"CareUEyes Lite";
    const wchar_t kConfigFileName[] = L"setting.dat";

    int ClampBrightness(int nBrightness)
    {
        if (nBrightness < 10)
            return 10;
        if (nBrightness > 100)
            return 100;
        return nBrightness;
    }

    BOOL GetCommonSettingFileName(WCHAR szPath[MAX_PATH])
    {
        ZeroMemory(szPath, sizeof(WCHAR) * MAX_PATH);
        if (!SHGetSpecialFolderPathW(NULL, szPath, CSIDL_APPDATA, TRUE))
            return FALSE;

        PathAppendW(szPath, kWorkFolderName);
        switch (SHCreateDirectoryExW(NULL, szPath, NULL))
        {
        case ERROR_SUCCESS:
        case ERROR_FILE_EXISTS:
        case ERROR_ALREADY_EXISTS:
            break;
        default:
            return FALSE;
        }

        PathAppendW(szPath, kConfigFileName);
        return TRUE;
    }
}

CSetting* CSetting::GetInstance()
{
    static CSetting setting;
    return &setting;
}

CSetting::CSetting()
{
    m_nBrightness = 80;
    ZeroMemory(m_szProfilePath, sizeof(m_szProfilePath));
    ReadSettings();
}

void CSetting::ReadSettings()
{
    WCHAR szPath[MAX_PATH] = {0};
    if (!GetCommonSettingFileName(szPath))
        return;
    wcsncpy_s(m_szProfilePath, _countof(m_szProfilePath), szPath, _TRUNCATE);

    m_nBrightness = ClampBrightness(GetPrivateProfileIntW(
        kSectionScreen,
        kKeyBrightness,
        m_nBrightness,
        m_szProfilePath));
}

void CSetting::SaveSetting()
{
    WCHAR szValue[16] = {0};
    _snwprintf_s(szValue, _countof(szValue), _TRUNCATE, L"%d", m_nBrightness);
    WritePrivateProfileStringW(kSectionScreen, kKeyBrightness, szValue, m_szProfilePath);
}

BOOL CSetting::SetBrightness(int nBrightness)
{
    m_nBrightness = ClampBrightness(nBrightness);
    return TRUE;
}

BOOL CSetting::GetBrightness(int& nBrightness)
{
    nBrightness = m_nBrightness;
    return TRUE;
}
