#include <windows.h>
#include <tlhelp32.h>
#include <iostream>

int main()
{
	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

	if (snapshot == INVALID_HANDLE_VALUE)
	{
		printf("erro create snapshot codigo: %lu \n", GetLastError());
		return 1;
	};

	PROCESSENTRY32 pe32;
	pe32.dwSize = sizeof(PROCESSENTRY32);

	if (Process32First(snapshot, &pe32))
	{
		do
		{
			printf("Name: %ls \n", pe32.szExeFile);

		} while(Process32Next(snapshot, &pe32));
	}
	else
	{
		printf("erro no info snapshot: %lu \n", GetLastError());
	}
	return 0;
}