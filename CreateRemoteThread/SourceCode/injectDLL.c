#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <windows.h>
#include <tchar.h>

BOOL SetPrivilege(LPCTSTR lpszPrivilege, BOOL bEnablePrivilege) 
{
    TOKEN_PRIVILEGES tp;
    HANDLE hToken;
    LUID luid;

    if( !OpenProcessToken(GetCurrentProcess(),
                          TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, 
			              &hToken) )
    {
        _tprintf("OpenProcessToken error: %u\n", GetLastError());
        return FALSE;
    }

    if( !LookupPrivilegeValue(NULL,           // lookup privilege on local system
                              lpszPrivilege,  // privilege to lookup 
                              &luid) )        // receives LUID of privilege
    {
        _tprintf("LookupPrivilegeValue error: %u\n", GetLastError() ); 
        return FALSE; 
    }

    tp.PrivilegeCount = 1;
    tp.Privileges[0].Luid = luid;
    if( bEnablePrivilege )
        tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
    else
        tp.Privileges[0].Attributes = 0;

    // Enable the privilege or disable all privileges.
    if( !AdjustTokenPrivileges(hToken, 
                               FALSE, 
                               &tp, 
                               sizeof(TOKEN_PRIVILEGES), 
                               (PTOKEN_PRIVILEGES) NULL, 
                               (PDWORD) NULL) )
    { 
        _tprintf("AdjustTokenPrivileges error: %u\n", GetLastError() ); 
        return FALSE; 
    } 

    if( GetLastError() == ERROR_NOT_ALL_ASSIGNED )
    {
        _tprintf("The token does not have the specified privilege. \n");
        return FALSE;
    } 

    return TRUE;
}

// InjectDLL(PID, DLLPath)
BOOL InjectDLL(DWORD dwPID, LPCTSTR szDLLPath)
{   
    HANDLE hThread = NULL;
    HANDLE hProcess = NULL; 
    HMODULE hMod = NULL; 
    LPVOID pRemoteBuf = NULL;
    DWORD dwBufSize = (DWORD)(_tcslen(szDLLPath)+1) * sizeof(TCHAR);
    LPTHREAD_START_ROUTINE pThreadProc;

    // 1. PID를 이용하여 대상 프로세스의 HANDLE을 구한다. 
    if(!(hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwPID)))
    {
        _tprintf("OpenProcess(%d) failed!!! [%d]\n", dwPID, GetLastError());
        return FALSE; 
    }

    // 2. 대상 프로세스의 메모리에 szDLLPath(DLL 경로)의 크기(dwBufSize)만큼 메모리 할당 
    pRemoteBuf = VirtualAllocEx(hProcess, NULL, dwBufSize, MEM_COMMIT, PAGE_READWRITE);

    // 3. 할당 받은 메모리에 DLL 경로 쓰기 
    WriteProcessMemory(hProcess, pRemoteBuf, (LPVOID)szDLLPath, dwBufSize, NULL);

    // 4. LoadLibrary 함수의 주소를 구한다.
    hMod = GetModuleHandleA("kernel32.dll");
    pThreadProc = (LPTHREAD_START_ROUTINE)GetProcAddress(hMod, "LoadLibraryA");
    // LoadLibraryW for UNICODE(2바이트 씩 값을 읽음)
    // LoadLibraryA for ANSI

    // 5. 원격으로 pThreadProc 실행 
    hThread = CreateRemoteThread(hProcess,      // 원격 스레드를 실행할 프로세스
                                 NULL,          // 
                                 0,             // 
                                 pThreadProc,   // 원격으로 실행할 스레드 함수 
                                 pRemoteBuf,    // 원격으로 실행할 스레드 함수의 파라미터 
                                 0,             //
                                 NULL);   
                        
    WaitForSingleObject(hThread, INFINITE);

    CloseHandle(hThread);
    CloseHandle(hProcess);

    return TRUE;
}


int _tmain(int argc, TCHAR* argv[])
{
    if(argc != 3)
    {
        _tprintf("Usage: %s <PID> <DLL>\n", argv[0]);
        return 1;
    }
    
    // change privilege
    if(!SetPrivilege(SE_DEBUG_NAME, TRUE))return 1;

    if(InjectDLL((DWORD)_tstol(argv[1]), argv[2])) _tprintf("InjectDLL(\"%s\") success!!!\n", argv[2]);
    else _tprintf("InjectDLL(\"%s\") failed!!!\n", argv[2]);
    return 0;
}