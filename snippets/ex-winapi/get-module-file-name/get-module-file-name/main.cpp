#include <iostream>
#include <windows.h>


int main()
{
	wchar_t path[MAX_PATH];

	if (GetModuleFileNameW(NULL, path, MAX_PATH))
	{
		wprintf(L"path: %ls\n", path);
	}
	else
	{
		wprintf(L"error: %lu\n", GetLastError());
	}
	return 0;
}