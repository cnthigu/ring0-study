# Capítulo 6 — O I/O Manager e o Modelo de Driver WDM

> Livro didático baseado em *Windows Internals, 7th Edition*.
> Atualizado para Windows 11 com exemplos práticos em C++ moderno.

> Navegação: [Índice Cap.6](capitulo-06.md) | [Próximo](capitulo-06-02-irp.md)

---

## O I/O Manager: o controlador de tráfego de I/O

O **I/O Manager** é o componente do kernel que define a arquitetura de como I/O é processado. Ele:
- Define os objetos de I/O: file objects, device objects, driver objects
- Cria e encaminha **IRPs (I/O Request Packets)** para os drivers
- Gerencia o modelo de driver em camadas (layered drivers)
- Implementa I/O assíncrono, I/O de cancelamento, e completion routines
- Coordena com o PnP Manager e o Power Manager

```
Arquitetura de I/O do Windows:

  Aplicação
    │  CreateFile / ReadFile / WriteFile / DeviceIoControl
    ▼
  Win32 Subsystem (kernel32.dll / ntdll.dll)
    │  NtCreateFile / NtReadFile / NtWriteFile
    ▼
  I/O Manager (ntoskrnl.exe)
    │  Cria IRP → encaminha para a pilha de drivers
    ▼
  Driver Stack (camadas de drivers)
    ├── Filter Driver 1 (ex: antivírus → filtro de arquivo)
    ├── Filter Driver 2 (ex: criptografia transparente)
    ├── Function Driver (ex: ntfs.sys, tcpip.sys, SCSI miniport)
    └── Bus Driver (ex: PCI, USB, ACPI)
    │
    ▼
  Hardware (disco, NIC, USB device...)
```

---

## Objetos de I/O: File Object, Device Object, Driver Object

```
Driver Object (DRIVER_OBJECT)
  → criado quando o driver é carregado pelo SCM ou PnP Manager
  → contém ponteiros para rotinas do driver (DriverEntry, IRP handlers)
  → um por driver (.sys)

Device Object (DEVICE_OBJECT)
  → criado pelo driver para representar um dispositivo
  → cada device na pilha é um device object separado
  → possui um "stack" vertical (AttachDevice)

File Object (FILE_OBJECT)
  → criado para cada handle de arquivo aberto
  → aponta para o Device Object alvo
  → contém: nome do arquivo, posição atual, flags de acesso
```

```c
// Em um driver (miniexemplo do DriverEntry):
NTSTATUS DriverEntry(PDRIVER_OBJECT DriverObject, PUNICODE_STRING RegistryPath) {
    // Registrar handlers de IRP:
    DriverObject->MajorFunction[IRP_MJ_CREATE]  = MeuDriverCreate;
    DriverObject->MajorFunction[IRP_MJ_READ]    = MeuDriverRead;
    DriverObject->MajorFunction[IRP_MJ_WRITE]   = MeuDriverWrite;
    DriverObject->MajorFunction[IRP_MJ_CLOSE]   = MeuDriverClose;
    DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = MeuDriverIoctl;
    DriverObject->DriverUnload = MeuDriverUnload;
    
    // Criar device object:
    UNICODE_STRING devName = RTL_CONSTANT_STRING(L"\\Device\\MeuDispositivo");
    PDEVICE_OBJECT pDevObj;
    return IoCreateDevice(DriverObject, sizeof(MEU_CONTEXTO),
                          &devName, FILE_DEVICE_UNKNOWN,
                          FILE_DEVICE_SECURE_OPEN, FALSE, &pDevObj);
}
```

---

## A pilha de drivers (Driver Stack)

Um dispositivo real como um disco SATA tem múltiplas camadas de drivers. Cada camada é um **Device Object** diferente, todos ligados em uma pilha vertical:

```
Pilha de drivers de um disco SATA (de cima para baixo):

  [FDO] disk.sys         ← Function Device Object — gerencia o disco
    ↓ IoAttachDeviceToDeviceStack
  [LDO] storport.sys     ← Port driver (Storport miniport architecture)
    ↓ IoAttachDeviceToDeviceStack
  [PDO] pci.sys          ← Physical Device Object — representação PCI do disco
    ↓
  [Bus] PCI Bus Driver   ← gerencia o barramento PCI

Quando ReadFile gera um IRP_MJ_READ:
  I/O Manager → disk.sys → storport.sys → pci.sys → hardware
  (cada driver pode processar parcialmente e passar adiante)
```

---

## Comunicação user mode → driver: DeviceIoControl

```cpp
// User mode: abrir o driver e enviar IOCTL
HANDLE hDev = CreateFileW(
    L"\\\\.\\MeuDispositivo",  // nome de dispositivo no namespace Win32
    GENERIC_READ | GENERIC_WRITE,
    0, nullptr,
    OPEN_EXISTING, 0, nullptr);

// Definir o código do IOCTL:
// CTL_CODE(DeviceType, Function, Method, Access)
#define IOCTL_MEU_COMANDO CTL_CODE(FILE_DEVICE_UNKNOWN, 0x800, METHOD_BUFFERED, FILE_ANY_ACCESS)

DWORD bytesReturned;
char inputBuffer[64] = "Comando";
char outputBuffer[256] = {};

DeviceIoControl(hDev,
    IOCTL_MEU_COMANDO,
    inputBuffer, sizeof(inputBuffer),
    outputBuffer, sizeof(outputBuffer),
    &bytesReturned, nullptr);

printf("Driver respondeu: %s\n", outputBuffer);
CloseHandle(hDev);
```

```c
// Kernel mode: processar o IOCTL
NTSTATUS MeuDriverIoctl(PDEVICE_OBJECT DevObj, PIRP Irp) {
    PIO_STACK_LOCATION stack = IoGetCurrentIrpStackLocation(Irp);
    ULONG code = stack->Parameters.DeviceIoControl.IoControlCode;
    
    if (code == IOCTL_MEU_COMANDO) {
        // METHOD_BUFFERED: I/O Manager copiou input para Irp->AssociatedIrp.SystemBuffer
        char* input = (char*)Irp->AssociatedIrp.SystemBuffer;
        char* output = (char*)Irp->AssociatedIrp.SystemBuffer; // mesmo buffer para METHOD_BUFFERED
        
        sprintf_s(output, 256, "Driver recebeu: %s", input);
        
        Irp->IoStatus.Status = STATUS_SUCCESS;
        Irp->IoStatus.Information = strlen(output) + 1; // bytes escritos
    }
    
    IoCompleteRequest(Irp, IO_NO_INCREMENT);
    return STATUS_SUCCESS;
}
```

---

## Tipos de I/O: Buffered, Direct e Neither

O método de transferência define como dados circulam entre user mode e kernel:

```
METHOD_BUFFERED (mais comum):
  → I/O Manager copia input para kernel buffer
  → Driver escreve no mesmo kernel buffer
  → I/O Manager copia output de volta para user buffer
  → Simples, seguro, overhead de cópia

METHOD_IN_DIRECT / METHOD_OUT_DIRECT:
  → I/O Manager "bloqueia" o user buffer via MDL (Memory Descriptor List)
  → Driver acessa diretamente o user buffer (mapeado em kernel space)
  → Zero-copy para grandes transferências
  → Requer MDL handling pelo driver

METHOD_NEITHER:
  → I/O Manager passa os ponteiros user-mode diretamente
  → Driver é responsável por validar e acessar com ProbeForRead/Write
  → Máxima flexibilidade, máximo risco de segurança
```

```c
// METHOD_IN_DIRECT: acessar buffer do usuário via MDL
NTSTATUS ReadHandler(PDEVICE_OBJECT DevObj, PIRP Irp) {
    PIO_STACK_LOCATION stack = IoGetCurrentIrpStackLocation(Irp);
    ULONG bytesRequested = stack->Parameters.Read.Length;
    
    // O MDL descreve os buffers do usuário já "pinados" em memória:
    PMDL pMdl = Irp->MdlAddress;
    void* userBuffer = MmGetSystemAddressForMdlSafe(pMdl, NormalPagePriority);
    
    // Copiar dados do hardware para o buffer do usuário (mapeado):
    RtlCopyMemory(userBuffer, meusDadosDeHardware, bytesRequested);
    
    Irp->IoStatus.Status = STATUS_SUCCESS;
    Irp->IoStatus.Information = bytesRequested;
    IoCompleteRequest(Irp, IO_DISK_INCREMENT);
    return STATUS_SUCCESS;
}
```

---

## WDM vs. KMDF vs. UMDF

O Windows oferece três frameworks para desenvolver drivers:

| Framework | Nível | Prós | Contras |
|-----------|-------|------|---------|
| **WDM** (Windows Driver Model) | Bruto, baixo nível | Controle total, acesso a tudo | Muito complexo, propenso a erros |
| **KMDF** (Kernel Mode Driver Framework) | Kernel mode, abstrato | Trata IRPs automaticamente, mais seguro | Ainda em kernel mode |
| **UMDF** (User Mode Driver Framework) | User mode | Falhas não derrubam o sistema, debugging fácil | Latência maior, sem acesso a IRQL alto |

```c
// KMDF simplifica muito:
// Em vez de tratar IRPs manualmente, registra callbacks de alto nível:

EVT_WDF_IO_QUEUE_IO_READ MinhaLeituira;

VOID MinhaLeitura(
    WDFQUEUE Queue,
    WDFREQUEST Request,
    size_t Length)
{
    WDFMEMORY mem;
    WdfRequestRetrieveOutputMemory(Request, &mem);
    
    void* buf = WdfMemoryGetBuffer(mem, nullptr);
    // preencher buf com dados...
    
    WdfRequestCompleteWithInformation(Request, STATUS_SUCCESS, Length);
    // O framework cuida de completar o IRP!
}
```

---

## Diagnóstico do I/O Manager

```
WinDbg:
kd> !drvobj \Driver\disk      ← ver o Driver Object do disco
kd> !devobj \Device\Harddisk0 ← ver Device Object de um disco
kd> !irpfind                  ← IRPs ativos no sistema (pode ser lento)
kd> !irp <endereço>           ← detalhes de um IRP específico

Para ver a pilha de drivers de um dispositivo:
kd> !devstack \Device\Harddisk0\DR0
```

---

*Próximo: [IRPs: I/O Request Packets em Detalhes](capitulo-06-02-irp.md)*
