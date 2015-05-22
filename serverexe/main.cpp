#include <SDKDDKVer.h>
#include <windows.h>
#include <string>
#include <io.h>
#include <direct.h>
#include <stdlib.h>
#include <stdio.h>

HMODULE InjectDLL(DWORD adw_ProcessId, const std::string& as_DllFile);
bool FileExists(char *file);

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	char dllpath[1024];
	_getcwd(dllpath, 1024);
	strcat(dllpath, "\\jk2mf.dll");

	// look for jk2kmf.dll
	if ( !FileExists(dllpath) )
	{
		MessageBoxA(NULL, "jk2mf.dll not found!", "Error", NULL);
		return 0;
	}

	char dedpath[1024];
	_getcwd(dedpath, 1024);
	strcat(dedpath, "\\jk2ded.exe");

	char mppath[1024];
	_getcwd(mppath, 1024);
	strcat(mppath, "\\jk2mp.exe");

	// none of them exist? aborting!
	if ( !FileExists(dedpath) && !FileExists(mppath) )
	{
		MessageBoxA(NULL, "No jk2ded.exe or jk2mp.exe could be found in the current directory", "Error", NULL);
		return 0;
	}

	char cmdline[2048];

	char finalPath[1024];
	if ( FileExists(dedpath) )
	{
		strcpy(finalPath, dedpath);
		strcat(cmdline, "jk2ded.exe ");
	}
	else
	{
		strcpy(finalPath, mppath);
		strcat(cmdline, "jk2mp.exe ");
	}

    STARTUPINFO          si = { sizeof(si) };
    PROCESS_INFORMATION  pi;

	strcat(cmdline, lpCmdLine);

	// Bis jetzt alles OK, starte pausiert
	if ( !CreateProcessA(finalPath, cmdline, NULL, NULL, TRUE, CREATE_SUSPENDED, NULL, NULL, LPSTARTUPINFOA(&si), &pi) )
	{
		char error[256];
		sprintf(error, "jk2mp.exe could not be started! (%d)", GetLastError());
		MessageBoxA(NULL, error, "Error", NULL);
		return 0;
	}

	char jk2version1[13];
	char jk2version2[13];

	// Wenn einer dieser Adressen nicht "JK2MP: v1.04" ist können wir den PureKiller leider nicht starten
	ReadProcessMemory(pi.hProcess, (void*)0x04f66a0, jk2version1, 13, NULL);
	ReadProcessMemory(pi.hProcess, (void*)0x046f5d0, jk2version2, 13, NULL);
	if ( strcmp(jk2version1, "JK2MP: v1.04") && strcmp(jk2version2, "JK2MP: v1.04") )
	{
		TerminateProcess(pi.hProcess, 0);
		MessageBoxA(NULL, "JK2MF only supports version 1.04 of JK2", "Error", NULL);
		return 0;
	}

	// DLL injizieren
	InjectDLL(pi.dwProcessId, dllpath);

	ResumeThread(pi.hThread);

	return 1;
}

HMODULE InjectDLL(DWORD adw_ProcessId, const std::string& as_DllFile)
{
    HMODULE hLocKernel32 = GetModuleHandleW(L"KERNEL32");
    FARPROC hLocLoadLibrary = GetProcAddress(hLocKernel32, "LoadLibraryA");
    HANDLE hToken;
    TOKEN_PRIVILEGES tkp;

    if(OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
    {
        LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &tkp.Privileges[0].Luid);
        tkp.PrivilegeCount = 1;
        tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
        AdjustTokenPrivileges(hToken, 0, &tkp, sizeof(tkp), NULL, NULL);
        CloseHandle(hToken);
    }

    HANDLE hProc = OpenProcess(PROCESS_ALL_ACCESS, FALSE, adw_ProcessId);
    if (hProc == NULL)
        return NULL;

    LPVOID hRemoteMem = VirtualAllocEx(hProc, NULL, as_DllFile.size()*sizeof(char), MEM_COMMIT, PAGE_READWRITE);
    DWORD numBytesWritten;
    WriteProcessMemory(hProc, hRemoteMem, as_DllFile.c_str(), as_DllFile.size()*sizeof(char), &numBytesWritten);
    HANDLE hRemoteThread = CreateRemoteThread(hProc, NULL, 0, (LPTHREAD_START_ROUTINE)hLocLoadLibrary, hRemoteMem, 0, NULL);

    ::WaitForSingleObject( hRemoteThread, INFINITE );
    DWORD  hLibModule = 0;
    ::GetExitCodeThread( hRemoteThread, &hLibModule );
    ::VirtualFreeEx(hProc, hRemoteMem, as_DllFile.size()*sizeof(char), MEM_RELEASE);
    ::CloseHandle(hProc);

    return (HMODULE)hLibModule;
}

bool FileExists(char *file)
{
	if (access(file, 0) == 0)
	{
		return true;
	}
	else return false;
}
