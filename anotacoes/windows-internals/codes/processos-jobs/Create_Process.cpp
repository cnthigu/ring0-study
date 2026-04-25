// Texto: ../../processos-jobs/processos/Creation Of Process.md
#include <windows.h>
#include <iostream>

int main() {
    STARTUPINFO si = {};
    PROCESS_INFORMATION pi = {};

    // Caminho do executavel
    LPCWSTR notepadPath = L"C:\\Windows\\System32\\notepad.exe";

    if (CreateProcess(notepadPath, NULL, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
        std::wcout << L"Process ID (PID): " << pi.dwProcessId << std::endl;

        // Fechar handles de processo e de thread
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
    }
    else {
        std::cerr << "CreateProcess failed. Error: " << GetLastError() << std::endl;
    }

    return 0;
}
