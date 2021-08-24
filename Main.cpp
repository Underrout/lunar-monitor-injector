#define DLL_PATH "lunar-monitor.dll"

#include <string>
#include <fstream>
#include <Windows.h>
#include <strsafe.h>

#include "InjectDLL.h"

HWND gLmWindowHandle;
HWINEVENTHOOK gCloseEventHook;
BOOL exitNow = false;

int main(int argc, char* argv[])
{
	HWND gLmWindowHandle = (HWND)std::stoull(argv[1], 0, 16);
	DWORD pId;

	if (gLmWindowHandle != NULL)
	{
		GetWindowThreadProcessId(gLmWindowHandle, &pId);

		HANDLE processHandle = OpenProcess(
			PROCESS_CREATE_THREAD | PROCESS_VM_OPERATION | PROCESS_VM_WRITE | SYNCHRONIZE,
			false, pId
		);

		if (processHandle != NULL)
		{
			InjectDLL(TEXT(DLL_PATH), processHandle);

			DWORD dw = WaitForSingleObject(processHandle, INFINITE);

			CloseHandle(processHandle);
		}

		CloseHandle(gLmWindowHandle);
	}
}
