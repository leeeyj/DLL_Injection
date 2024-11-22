// gcc -shared -o myhack.dll myhack.c 
#include <stdio.h>
#include <stdlib.h>
#include <string.h> 
#include <windows.h>
#include <tchar.h>

HMODULE g_hMod = NULL; 

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
    g_hMod = (HMODULE)hinstDLL;

    switch (fdwReason)
    {
        case DLL_PROCESS_ATTACH:
            MessageBox(NULL, "myHack.dll Injection success", "Alarm", MB_OK);
            break;
    }

    return TRUE;
}