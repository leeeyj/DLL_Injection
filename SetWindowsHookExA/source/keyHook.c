#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// #include <winuser.h>
#include <windows.h>

HINSTANCE g_hInstance = NULL;
HHOOK g_hHook = NULL;
HWND g_hWnd = NULL;

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD dwReason, LPVOID lpvReserved)
{
    switch (dwReason)
    {
        case DLL_PROCESS_ATTACH:
            g_hInstance = hinstDLL;
            break;
        case DLL_THREAD_ATTACH:
            break;
        case DLL_THREAD_DETACH:
            break;
        case DLL_PROCESS_DETACH:
            break;
    }
    return TRUE;
}

LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    char szPath[MAX_PATH] = { 0, };
    char* p = NULL;

    if (nCode >= 0)
    {
        // 키보드가 눌렀다 떨어질 때 
        if (!(lParam & 0x80000000))
        {
            GetModuleFileNameA(NULL, szPath, MAX_PATH);
            p = strrchr(szPath, '\\');

            // 현재 프로세스 이름이 notepad.exe인지 확인 
            // notepad.exe라면 메시지 박스 출력 
            if (!_stricmp(p + 1, "notepad.exe"))
            {
                MessageBoxA(NULL, "Detect Keyboard Input at Notepad.exe", "Alarm", MB_OK);
                return 1;
            }
        }
    }

    return CallNextHookEx(g_hHook, nCode, wParam, lParam);
}

#ifdef __cplisplus
extern "C" {
#endif
__declspec(dllexport) void HookStart()
{
    // 키보드 훅 체인에 KeyboardProc 훅 추가
    HHOOK g_hHook = SetWindowsHookEx(WH_KEYBOARD, KeyboardProc, g_hInstance, 0);

    return;
}

__declspec(dllexport) void HookStop()
{
    if (g_hHook)
    {
        UnhookWindowsHookEx(g_hHook);
        g_hHook = NULL;
    }

    return;
}
#ifdef __cplisplus
}
#endif