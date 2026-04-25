// Exemplo: obter o caminho completo do executavel deste processo.
// API usada: GetModuleFileNameW (NULL no primeiro argumento = modulo principal).
#include <iostream>
#include <windows.h>


int main()
{
	wchar_t path[MAX_PATH];

	if (GetModuleFileNameW(NULL, path, MAX_PATH))
	{
		wprintf(L"caminho: %ls\n", path);
	}
	else
	{
		wprintf(L"erro: %lu\n", GetLastError());
	}
	return 0;
}