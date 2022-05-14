#define DLL_PATH "lunar-monitor.dll"

#include <string>
#include <Windows.h>
#include <strsafe.h>

#include "InjectDLL.h"

HWND gLmWindowHandle;

int main(int argc, char* argv[])
{
	std::string str = argv[1];
	HWND gLmWindowHandle = (HWND)std::stoull(str, 0, 16);
	DWORD verificationCode = (DWORD)(std::stoi(str.substr(str.find_first_of(':') + 1), 0, 16));
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
			HANDLE pipe = CreateNamedPipe(
				L"\\\\.\\pipe\\lunar_monitor_pipe", // name of the pipe
				PIPE_ACCESS_OUTBOUND, // 1-way pipe -- send only
				PIPE_TYPE_BYTE, // send data as a byte stream
				1, // only allow 1 instance of this pipe
				0, // no outbound buffer
				0, // no inbound buffer
				0, // use default wait time
				NULL // use default security attributes
			);

			InjectDLL(TEXT(DLL_PATH), processHandle);

			if (pipe != NULL && pipe != INVALID_HANDLE_VALUE)
			{
				BOOL result = ConnectNamedPipe(pipe, NULL);
				if (result)
				{
					WriteFile(
						pipe,
						&gLmWindowHandle,
						sizeof(HWND),
						NULL,
						NULL
					);

					WriteFile(
						pipe,
						&verificationCode,
						sizeof(DWORD),
						NULL,
						NULL
					);
				}
				CloseHandle(pipe);
			}

			DWORD dw = WaitForSingleObject(processHandle, INFINITE);

			CloseHandle(processHandle);
		}

		CloseHandle(gLmWindowHandle);
	}
}
