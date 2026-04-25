// Texto: ../../Processes and Jobs/Processes/PEB - Part 2.md
#include <Windows.h>
#include <winternl.h>
#include <stdio.h>

// Estruturas PEB/LDR redefinidas (uso educacional)
typedef struct _MY_PEB_LDR_DATA {
	ULONG Length;
	BOOL Initialized;
	PVOID SsHandle;
	LIST_ENTRY InLoadOrderModuleList;
	LIST_ENTRY InMemoryOrderModuleList;
	LIST_ENTRY InInitializationOrderModuleList;
} MY_PEB_LDR_DATA, * PMY_PEB_LDR_DATA;

typedef struct _MY_LDR_DATA_TABLE_ENTRY
{
	LIST_ENTRY InLoadOrderLinks;
	LIST_ENTRY InMemoryOrderLinks;
	LIST_ENTRY InInitializationOrderLinks;
	PVOID DllBase;
	PVOID EntryPoint;
	ULONG SizeOfImage;
	UNICODE_STRING FullDllName;
	UNICODE_STRING BaseDllName;
} MY_LDR_DATA_TABLE_ENTRY, * PMY_LDR_DATA_TABLE_ENTRY;

int main()
{
	// Endereco do PEB (Teb x64, gs+0x60)
	PPEB PEB_Address = (PPEB) __readgsqword( 0x60 );

	// PEB->Ldr
	PMY_PEB_LDR_DATA P_Ldr = (PMY_PEB_LDR_DATA) PEB_Address->Ldr;

	// Primeiro no da InLoadOrderModuleList
	PLIST_ENTRY P_NextModule = P_Ldr->InLoadOrderModuleList.Flink;

	// Exibir como LDR_DATA_TABLE_ENTRY
	PMY_LDR_DATA_TABLE_ENTRY P_DataTableEntry = (PMY_LDR_DATA_TABLE_ENTRY) P_NextModule;

	while (P_DataTableEntry->DllBase != NULL)
	{
		// Base do modulo
		PVOID P_ModuleBase = P_DataTableEntry->DllBase;

		// Nome do modulo
		UNICODE_STRING BaseDllName = P_DataTableEntry->BaseDllName;

		printf("DLL Name : %ls\n", BaseDllName.Buffer);
		printf("DLL Base Address: %p\n", P_ModuleBase);
		printf("\n");

		// Proxima entrada
		P_DataTableEntry = (PMY_LDR_DATA_TABLE_ENTRY) P_DataTableEntry->InLoadOrderLinks.Flink;
	}
	return 0;
}
