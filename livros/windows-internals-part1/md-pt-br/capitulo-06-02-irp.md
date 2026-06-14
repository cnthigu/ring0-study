# Capítulo 6 — IRPs: I/O Request Packets em Detalhes

> Livro didático baseado em *Windows Internals, 7th Edition*.
> Atualizado para Windows 11 com exemplos práticos em C++ moderno.

> Navegação: [Anterior](capitulo-06-01-io-manager.md) | [Índice Cap.6](capitulo-06.md) | [Próximo](capitulo-06-03-pnp-power.md)

---

## O IRP: a unidade de trabalho do I/O

Cada operação de I/O no Windows é representada por um **IRP (I/O Request Packet)**. O IRP é a estrutura que viaja de cima a baixo pela pilha de drivers, carregando a requisição e coletando o resultado.

```
Estrutura IRP (simplificada):

IRP
  ├── Type, Size
  ├── MdlAddress        ← MDL descrevendo o buffer do usuário (METHOD_DIRECT)
  ├── AssociatedIrp
  │     └── SystemBuffer ← buffer da I/O Manager (METHOD_BUFFERED)
  ├── Tail.Overlay.ListEntry ← para encadear IRPs em filas
  ├── IoStatus
  │     ├── Status       ← NTSTATUS (preenchido na conclusão)
  │     └── Information  ← bytes transferidos
  ├── RequestorMode      ← KernelMode ou UserMode
  ├── Cancel             ← TRUE se cancelado
  ├── CancelIrql         ← IRQL para cancelamento
  └── Stack[N]           ← IO_STACK_LOCATION para cada driver na pilha
        ├── MajorFunction ← IRP_MJ_READ, IRP_MJ_WRITE, etc.
        ├── MinorFunction ← IRP_MN_*
        ├── CompletionRoutine ← callback quando o IRP sobe de volta
        └── Parameters    ← union com parâmetros por tipo de IRP
```

---

## IO_STACK_LOCATION: um slot por driver

Cada driver na pilha tem seu próprio **IO_STACK_LOCATION**. O I/O Manager aloca o IRP com N stack locations (onde N = número de drivers na pilha):

```
Pilha de I/O (exemplo: ReadFile de 4096 bytes de disco):

IRP
  ├── [Stack[2]] disk.sys
  │     MajorFunction = IRP_MJ_READ
  │     Parameters.Read.Length = 4096
  │     Parameters.Read.ByteOffset = 0x1000
  │
  ├── [Stack[1]] storport.sys
  │     (preenchido por disk.sys antes de passar para baixo)
  │
  └── [Stack[0]] pci.sys
        (preenchido por storport.sys antes de passar para baixo)

O driver atual trabalha com IoGetCurrentIrpStackLocation(Irp)
Para passar para o próximo driver: IoCallDriver(nextDevObj, Irp)
  → I/O Manager decrementa o ponteiro de stack location
```

---

## O ciclo de vida de um IRP

```
1. CRIAÇÃO (I/O Manager):
   IoBuildSynchronousFsdRequest() ou IoAllocateIrp()
   → aloca IRP, inicializa campos básicos

2. ENVIO (para o topo da pilha):
   IoCallDriver(topDevice, irp)
   → cada driver decide: processar, passar adiante, ou ambos

3. PASSAGEM para baixo (cada driver):
   IoSkipCurrentIrpStackLocation(irp)  ← não precisa de callback na volta
   // ou:
   IoCopyCurrentIrpStackLocationToNext(irp) ← copiar parâmetros para o próximo
   IoSetCompletionRoutine(irp, MinhaCallback, ...)  ← registrar callback
   IoCallDriver(nextDevice, irp)        ← passar para o próximo driver

4. PROCESSAMENTO (driver de baixo):
   → faz o I/O real (síncrono) ou enfileira para DPC (assíncrono)
   IoCompleteRequest(irp, IO_DISK_INCREMENT)

5. CONCLUSÃO (sobe pela pilha):
   → I/O Manager chama CompletionRoutines de cada driver (de baixo para cima)
   → Cada driver pode inspecionar/modificar o resultado
   → Ao chegar no topo: sinaliza o evento de conclusão do usuário
```

---

## Major Function Codes: tipos de IRP

```
IRP_MJ_CREATE          → CreateFile / NtCreateFile
IRP_MJ_CLOSE           → CloseHandle
IRP_MJ_READ            → ReadFile / NtReadFile
IRP_MJ_WRITE           → WriteFile / NtWriteFile
IRP_MJ_QUERY_INFORMATION → GetFileInformationByHandle / NtQueryInformationFile
IRP_MJ_SET_INFORMATION → SetFileInformationByHandle
IRP_MJ_QUERY_VOLUME_INFORMATION → GetDiskFreeSpace
IRP_MJ_DEVICE_CONTROL  → DeviceIoControl
IRP_MJ_INTERNAL_DEVICE_CONTROL → IOCTL de kernel para kernel (SCSI requests)
IRP_MJ_FLUSH_BUFFERS   → FlushFileBuffers
IRP_MJ_CLEANUP         → CloseHandle (antes do IRP_MJ_CLOSE)
IRP_MJ_PNP             → Plug and Play (enumerar, iniciar, parar)
IRP_MJ_POWER           → gerenciamento de energia (sleep, hibernate)
IRP_MJ_SYSTEM_CONTROL  → WMI (Windows Management Instrumentation)
```

---

## Completion Routines: processamento na volta

```c
// Registrar uma completion routine antes de passar o IRP para baixo:
IoCopyCurrentIrpStackLocationToNext(Irp);

IoSetCompletionRoutine(
    Irp,
    MinhaCompletionRoutine, // callback
    (PVOID)meuContexto,     // contexto
    TRUE,   // invocar em Success
    TRUE,   // invocar em Error
    TRUE    // invocar em Cancel
);

IoCallDriver(proximoDevice, Irp);
return STATUS_PENDING; // indica que completaremos mais tarde

// --- A completion routine (chamada pelo driver de baixo ao completar): ---
NTSTATUS MinhaCompletionRoutine(
    PDEVICE_OBJECT DevObj,
    PIRP Irp,
    PVOID Context)
{
    if (NT_SUCCESS(Irp->IoStatus.Status)) {
        // I/O completou com sucesso — pós-processar
        ProcessarResultado(Irp->AssociatedIrp.SystemBuffer,
                          Irp->IoStatus.Information);
    }
    
    // Retornar STATUS_CONTINUE_COMPLETION para continuar subindo
    // Retornar STATUS_MORE_PROCESSING_REQUIRED para segurar o IRP
    return STATUS_CONTINUE_COMPLETION;
}
```

---

## IRPs Pendentes e I/O Assíncrono

Quando um driver não pode completar o IRP imediatamente (ex: disco ainda lendo), ele marca o IRP como pendente:

```c
NTSTATUS MeuDriverRead(PDEVICE_OBJECT DevObj, PIRP Irp) {
    PIO_STACK_LOCATION stack = IoGetCurrentIrpStackLocation(Irp);
    
    // 1. Marcar como pendente (permitir retornar STATUS_PENDING):
    IoMarkIrpPending(Irp);
    
    // 2. Enfileirar para trabalho assíncrono:
    ExInterlockedInsertTailList(&minhaFila, &Irp->Tail.Overlay.ListEntry, &minhaFilaLock);
    KeSetEvent(&minhaFilaEvento, 0, FALSE);
    
    // 3. Retornar STATUS_PENDING — o I/O Manager sabe que não está completo
    return STATUS_PENDING;
}

// Thread de trabalho (em background):
VOID ThreadDeTrabalho(PVOID contexto) {
    while (TRUE) {
        KeWaitForSingleObject(&minhaFilaEvento, Executive, KernelMode, FALSE, nullptr);
        
        // Retirar IRP da fila e processar:
        PIRP irp = CONTAINING_RECORD(
            ExInterlockedRemoveHeadList(&minhaFila, &minhaFilaLock),
            IRP, Tail.Overlay.ListEntry);
        
        // Fazer o I/O real...
        
        irp->IoStatus.Status = STATUS_SUCCESS;
        irp->IoStatus.Information = bytesProcessados;
        IoCompleteRequest(irp, IO_DISK_INCREMENT);
    }
}
```

---

## Filter Drivers: interceptando IRPs

Filter drivers interceptam IRPs sem implementar um device real. São o mecanismo que antivírus, criptografia transparente e backup de dados usam:

```c
// Minifilter (FilterManager framework — moderno, preferível ao legacy filter):

CONST FLT_REGISTRATION FilterRegistration = {
    sizeof(FLT_REGISTRATION),
    FLT_REGISTRATION_VERSION,
    0,
    nullptr,            // ContextRegistration
    nullptr,            // OperationRegistration (callbacks de IRP)
    FilterUnload,
    InstanceSetup,
    InstanceQueryTeardown,
    nullptr, nullptr, nullptr, nullptr
};

// Callback pré-operação (antes do IRP descer):
FLT_PREOP_CALLBACK_STATUS PreCreateCallback(
    PFLT_CALLBACK_DATA Data,
    PCFLT_RELATED_OBJECTS Objects,
    PVOID* CompletionContext)
{
    PFLT_IO_PARAMETER_BLOCK iopb = Data->Iopb;
    
    if (iopb->MajorFunction == IRP_MJ_CREATE) {
        // Inspecionar o arquivo sendo aberto:
        UNICODE_STRING* name = &Objects->FileObject->FileName;
        
        if (EArquivoProibido(name)) {
            // Bloquear o acesso:
            Data->IoStatus.Status = STATUS_ACCESS_DENIED;
            return FLT_PREOP_COMPLETE;  // não passa para baixo
        }
    }
    
    return FLT_PREOP_SUCCESS_NO_CALLBACK;  // deixar passar
}
```

---

*Próximo: [Plug and Play, Power Management](capitulo-06-03-pnp-power.md)*
