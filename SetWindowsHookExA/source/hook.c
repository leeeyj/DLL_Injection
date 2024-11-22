#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <conio.h>

typedef void(*fp)();

int main() 
{
    // Inject.dll 로딩 
    HMODULE hModule = LoadLibraryA("./keyHook.dll");

    // Inject.dll의 함수 주소 가져오기
    void(*HookStart)() = (fp)GetProcAddress(hModule, "HookStart"); 
    void(*HookStop)() = (fp)GetProcAddress(hModule, "HookStop"); 

    // Inject.dll의 HookStart 함수 실행
    HookStart();

    // 사용자가 q를 입력하면 후킹 종료 
    printf("press q to quit!\n");
    while(_getch() != 'q');

    // Inject.dll의 HookStop 함수 종료
    HookStop();

    // dll 언로딩
    FreeLibrary(hModule);

    return 0;
}