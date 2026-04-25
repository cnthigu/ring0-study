// Exemplo: listar o nome de cada processo a partir de um snapshot do sistema.
// APIs usadas: CreateToolhelp32Snapshot, depois Process32First / Process32Next em loop.
#include <windows.h>
#include <tlhelp32.h>
#include <iostream>


int main()
{
	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

	if (snapshot == INVALID_HANDLE_VALUE)
	{
		printf("falha ao criar o snapshot, codigo: %lu\n", GetLastError());
		return 1;
	}

	PROCESSENTRY32 pe32;
	pe32.dwSize = sizeof(PROCESSENTRY32);

	if (Process32First(snapshot, &pe32))
	{
		do
		{
			printf("processo: %ls\n", pe32.szExeFile);

		} while (Process32Next(snapshot, &pe32));
	}
	else
	{
		printf("falha ao ler o snapshot, codigo: %lu\n", GetLastError());
	}
	return 0;
}