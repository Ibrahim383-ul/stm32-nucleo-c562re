# 🚀 STM32 NUCLEO-C562RE Hardware & CAN-FD Showcase

![STM32](https://img.shields.io/badge/MCU-STM32C562RET6-03234B?style=for-the-badge&logo=stmicroelectronics)
![Arm](https://img.shields.io/badge/Architecture-Arm%C2%AE%20Cortex%C2%AE--M33-0091BD?style=for-the-badge&logo=arm)
![CAN-FD](https://img.shields.io/badge/Fieldbus-CAN--FD%20Transceiver-E7352C?style=for-the-badge)
![Zephyr](https://img.shields.io/badge/RTOS-Zephyr%20Supported-black?style=for-the-badge)
![License](https://img.shields.io/badge/License-MIT-green?style=for-the-badge)

Entwicklungs- und Evaluierungs-Plattform für das **STM32 NUCLEO-C562RE** Board mit modernem **Arm® Cortex®-M33** Mikrocontroller. Dieses Projekt demonstriert den Einstieg in professionelle 32-Bit-Embedded-Entwicklung, die Ansteuerung der Onboard-Peripherie sowie industrielle Bus-Kommunikation über den integrierten **CAN-FD Transceiver**.

---

## ⚡ Board-Übersicht & Hardware-Aufbau

<p align="center">
  <!-- HIER EINFACH DEIN BILD VOM PC HINEINZIEHEN (Drag & Drop): -->
  <img width="600" alt="STM32 NUCLEO C562RE Board" src="HIER_BILD_REINZIEHEN" />
</p>

### Technische Eckdaten:
* **Mikrocontroller (MCU):** STM32C562RET6 im LQFP64-Gehäuse
* **Prozessorkern:** Arm® Cortex®-M33 mit MPU & TrustZone®
* **Speicherausstattung:** 512 KiB Flash, 128 KiB SRAM
* **Integrierter Debugger:** Onboard **STLINK-V3EC** (Programmierung & Live-Debugging via USB-C)
* **Konnektivität:** Integrierter CAN-FD Transceiver, USB Type-C® Device, Arduino Uno V3 & ST Morpho Header

---

## 🔌 Pin-Belegung & Schnittstellen

### 1. Integrierter CAN-FD Bus (FDCAN1)
Der Onboard-Transceiver ist direkt auf dem Board mit dem Mikrocontroller verbunden und über die grüne 3-polige Schraubklemme (**CN18**) zugänglich:

| Klemme CN18 | Signal | STM32 Pin | Funktion |
| :--- | :--- | :--- | :--- |
| **Pin 1** | **CANH** | — | CAN High Signalleitung |
| **Pin 2** | **CANL** | — | CAN Low Signalleitung |
| **Pin 3** | **GND** | **GND** | Massebezug |
| *(Intern)* | CAN_RX | **PB8** | FDCAN1 Empfangsleitung |
| *(Intern)* | CAN_TX | **PB9** | FDCAN1 Sendeleitung |
| *(Intern)* | Standby | **PE2** | Transceiver Standby-Steuerung |

> 💡 **Bus-Terminierung:** Über den Jumper **JP9** kann der integrierte **120-Ω Abschlusswiderstand** bei Bedarf direkt zugeschaltet werden.

### 2. Taster & LEDs

| Element | STM32 Pin | Beschreibung |
| :--- | :--- | :--- |
| **User LED (LD1)** | **PA5** | Frei programmierbare grüne LED *(geteilt mit Arduino D13 / SPI1 SCK)* |
| **User Button (B1)** | **PC13** | Blauer programmierbarer Taster |
| **Reset Button (B2)** | **NRST** | Schwarzer System-Reset-Taster |

---

## 🔄 Systemarchitektur

```mermaid
flowchart TD
    Host["Entwicklungs-PC (VS Code / STM32CubeIDE)"] -->|USB-C / SWD Debugging| STLink["On-Board STLINK-V3EC"]
    STLink -->|Flashen & Debuggen| MCU["STM32C562RET6 (Arm Cortex-M33)"]
    
    MCU <-->|PB8 (RX) / PB9 (TX)| Transceiver["On-Board CAN-FD Transceiver"]
    Transceiver <-->|Klemme CN18: CANH / CANL| CAN["CAN-FD Bus (Sensoren / KFZ-Netzwerk)"]
    
    MCU -->|PA5| LED["User LED (Grün)"]
    Button["User Button (PC13)"] -->|Interrupt| MCU
