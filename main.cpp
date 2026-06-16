#include "stdafx.h"

#include "tray_brightness_app.h"
#include "oneinstance.h"

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, LPWSTR, int)
{
    HRESULT hRes = CoInitialize(NULL);
    if (FAILED(hRes))
        return 1;

    if (OneInstace::IsRunning(MAINAPP_INSTANCE_UUID))
    {
        CoUninitialize();
        return 0;
    }
    OneInstace::CreateOneInstance(MAINAPP_INSTANCE_UUID);

    CTrayBrightnessApp app;
    if (!app.Initialize(hInstance))
    {
        CoUninitialize();
        return 1;
    }

    const int exitCode = app.Run();
    CoUninitialize();
    return exitCode;
}
