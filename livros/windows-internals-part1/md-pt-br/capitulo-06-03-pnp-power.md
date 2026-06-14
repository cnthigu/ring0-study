# Capítulo 6 — Plug and Play e Power Management

> Livro didático baseado em *Windows Internals, 7th Edition*.
> Atualizado para Windows 11 com exemplos práticos em C++ moderno.

> Navegação: [Anterior](capitulo-06-02-irp.md) | [Índice Cap.6](capitulo-06.md) | [Próximo](capitulo-06-04-io-async.md)

---

## O PnP Manager: a vida de um dispositivo

O **PnP Manager** (Plug and Play) gerencia todo o ciclo de vida de dispositivos de hardware:
- Enumeração de dispositivos durante boot e hotplug
- Carregamento dos drivers corretos (via INF + setup)
- Notificação de remoção segura
- Coordenação com o Power Manager

```
Ciclo de vida de um dispositivo USB recém conectado:

  1. Hardware detectado pelo Bus Driver (USB Hub Driver)
  2. Bus Driver cria um PDO (Physical Device Object) para o dispositivo
  3. Bus Driver notifica o PnP Manager: "novo dispositivo"
  4. PnP Manager consulta o PDO: "qual é seu hardware ID?"
     → ex: USB\VID_0483&PID_5740 (STM32 USB device)
  5. PnP Manager busca no Registry (HKLM\SYSTEM\CurrentControlSet\Enum)
     se há um driver instalado para este ID
  6. Se sim: carrega o .sys, chama DriverEntry
  7. Driver cria FDO (Function Device Object), ataca ao PDO
  8. PnP Manager envia IRP_MN_START_DEVICE para inicializar
  9. Dispositivo aparece no Device Manager e está pronto para usar
```

---

## A hierarquia de Device Objects

```
Para um teclado USB:

  [PDO] usbhub.sys          ← criado pelo hub USB
    ↑ AttachDevice
  [FDO] hidusb.sys          ← HID USB driver (função principal)
    ↑ AttachDevice
  [FDO/FiDO] kbdhid.sys    ← HID keyboard filter/function driver
    ↑ AttachDevice
  [FiDO] kbdclass.sys      ← keyboard class filter (acima de todos os teclados)

Quando você pressiona uma tecla:
  Interrupção USB → usbhub ISR → DPC → IRP_MJ_INTERNAL_DEVICE_CONTROL
  → hidusb.sys processa → kbdhid.sys converte para scan code
  → kbdclass.sys enfileira → csrss.exe lê e gera WM_KEYDOWN
```

---

## IRP_MJ_PNP e seus minor codes

O PnP usa IRPs com `IRP_MJ_PNP` e vários minor codes:

```
IRP_MN_START_DEVICE          → "inicialize o hardware"
IRP_MN_STOP_DEVICE           → "pare para reconfiguração (não remova)"
IRP_MN_REMOVE_DEVICE         → "o dispositivo foi removido"
IRP_MN_QUERY_REMOVE_DEVICE   → "posso remover? alguma objeção?"
IRP_MN_CANCEL_REMOVE_DEVICE  → "desistiu da remoção"
IRP_MN_SURPRISE_REMOVAL       → "foi removido sem aviso (eject abrupto)"
IRP_MN_QUERY_CAPABILITIES    → "quais são as capacidades do dispositivo?"
IRP_MN_QUERY_DEVICE_RELATIONS → "enumere filhos"
IRP_MN_FILTER_RESOURCE_REQUIREMENTS → "otimize recursos (IRQ, I/O ports)"
```

```c
// Tratando PnP no driver:
NTSTATUS MeuDriverPnp(PDEVICE_OBJECT DevObj, PIRP Irp) {
    PIO_STACK_LOCATION stack = IoGetCurrentIrpStackLocation(Irp);
    
    switch (stack->MinorFunction) {
    case IRP_MN_START_DEVICE:
        // Inicializar hardware:
        // - mapear I/O ports e MMIO
        // - conectar interrupção (IoConnectInterrupt)
        // - inicializar DPC, timer
        return InicializarHardware(DevObj, Irp);
        
    case IRP_MN_REMOVE_DEVICE:
        // Cleanup:
        // - desconectar interrupção
        // - liberar recursos
        // - deletar device object
        IoDeleteDevice(DevObj);
        Irp->IoStatus.Status = STATUS_SUCCESS;
        IoCompleteRequest(Irp, IO_NO_INCREMENT);
        return STATUS_SUCCESS;
        
    case IRP_MN_QUERY_REMOVE_DEVICE:
        // Posso ser removido? (verificar se há I/O pendente)
        if (HaIoPendente()) {
            Irp->IoStatus.Status = STATUS_UNSUCCESSFUL; // não pode remover agora
        } else {
            Irp->IoStatus.Status = STATUS_SUCCESS; // pode remover
        }
        IoCompleteRequest(Irp, IO_NO_INCREMENT);
        return Irp->IoStatus.Status;
    
    default:
        // Passar para o driver de baixo (para IRPs que não tratamos):
        IoSkipCurrentIrpStackLocation(Irp);
        return IoCallDriver(MinhaExtensao->NextDevice, Irp);
    }
}
```

---

## Power Management: IRP_MJ_POWER

O **Power Manager** coordena transições de energia do sistema e de dispositivos:

```
Estados de energia do sistema (S-states):
  S0 → Ligado (Working)
  S1 → Sleep leve (CPU parada, RAM alimentada)
  S2 → Sleep mais profundo
  S3 → Sleep (RAM alimentada, tudo mais desligado) — suspend-to-RAM
  S4 → Hibernate (RAM gravada no disco, tudo desligado) — suspend-to-disk
  S5 → Desligado (Soft Off)

Estados de energia de dispositivo (D-states):
  D0 → Ligado (Full Power)
  D1 → Sleep leve de dispositivo
  D2 → Sleep médio de dispositivo
  D3 Hot  → Desligado mas ainda com energia de barramento
  D3 Cold → Completamente sem energia
```

```c
// Tratando power management no driver:
NTSTATUS MeuDriverPower(PDEVICE_OBJECT DevObj, PIRP Irp) {
    PIO_STACK_LOCATION stack = IoGetCurrentIrpStackLocation(Irp);
    
    if (stack->MinorFunction == IRP_MN_SET_POWER) {
        POWER_STATE powerState = stack->Parameters.Power.State;
        
        if (powerState.DeviceState == PowerDeviceD3) {
            // Sistema vai dormir — salvar estado do hardware
            SalvarEstadoHardware();
        } else if (powerState.DeviceState == PowerDeviceD0) {
            // Sistema acordou — restaurar hardware
            RestaurarEstadoHardware();
        }
    }
    
    // Sempre passar para baixo (bus driver gerencia o hardware):
    PoStartNextPowerIrp(Irp);
    IoSkipCurrentIrpStackLocation(Irp);
    return PoCallDriver(MinhaExtensao->NextDevice, Irp);
}
```

---

## Connected Standby / Modern Standby (Windows 10+)

No Windows 10 e 11, computadores modernos (principalmente tablets e laptops com ARM/Intel Evo) usam **Modern Standby** em vez do S3 clássico:

```
S3 (Sleep clássico):
  CPU completamente para
  Sem conectividade de rede
  Acordar em ~2 segundos

Modern Standby (S0 Idle):
  CPU em estado de baixíssima frequência (não parada)
  Conectividade WiFi mantida em background
  Atualizações de email/notificações chegam enquanto "dormindo"
  Acordar em < 1 segundo
  Similar ao "deep sleep" de smartphones
```

Do ponto de vista de drivers, Modern Standby usa o mesmo modelo de D-states, mas a transição S0→Idle é mais frequente e gerenciada pelo ACPI e Power Framework (PoFx).

---

## Device Manager e diagnóstico

```
Device Manager (devmgmt.msc):
  → Dispositivos com ! amarelo: driver não carregou (erro no DriverEntry?)
  → Código de erro 43: "hardware reportou problema"
  → Código de erro 10: "não pôde iniciar" (IRP_MN_START_DEVICE falhou)

WinDbg:
kd> !pnptriage          ← diagnóstico de PnP (crash relacionado a PnP)
kd> !devnode 0 1        ← árvore de device nodes com detalhes
kd> !devstack <devobj>  ← pilha de um dispositivo específico

Logs de PnP:
  Event Viewer → Windows Logs → System → filtrar por "Microsoft-Windows-Kernel-PnP"
  → Veja eventos de instalação, remoção, falhas de driver
```

---

*Próximo: [I/O Assíncrono e Completion Ports](capitulo-06-04-io-async.md)*
