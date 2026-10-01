# Nucleo C562RE

| Eigenschaft | Details |
| :--- | :--- |
| **Name** | `nucleo_c562re` |
| **Hersteller (Vendor)** | [STMicroelectronics](https://www.st.com/en/evaluation-tools/nucleo-c562re.html) |
| **Status** | Maintained |
| **Architektur** | [ARM](https://docs.zephyrproject.org/latest/boards/index.html#arch=arm) |
| **SoC** | [stm32c562xx](https://docs.zephyrproject.org/latest/boards/index.html#soc=stm32c562xx) |
| **Speicher** | **128 KiB RAM**, **512 KiB Flash** |

---

## Overview

The STM32 Nucleo-64 development board with **STM32C562RE MCU**, supports Arduino and ST morpho connectivity.

The STM32 Nucleo-64 board provides an affordable and flexible way for users to try out new concepts and build prototypes by choosing from the various combinations of performance and power consumption features provided by the STM32 microcontroller. For the compatible boards, the internal or external SMPS significantly reduces power consumption in Run mode.

The ARDUINO® Uno V3 connectivity support and the ST morpho headers allow the easy expansion of the functionality of the STM32 Nucleo open development platform with a wide choice of specialized shields.

The STM32 Nucleo-64 board does not require any separate probe as it integrates the ST-LINK debugger/programmer.

<p align="center">
  <img src="https://docs.zephyrproject.org/latest/_images/nucleo_c562re1.webp" alt="Nucleo C562RE" width="550" />
</p>

More information about the board can be found at the [Nucleo C562RE website](https://www.st.com/en/evaluation-tools/nucleo-c562re.html).

---

## Hardware

* **STM32C562RET6 microcontroller** based on the **Arm® Cortex®-M33** core in an LQFP64 package
* 32.768 kHz LSE crystal oscillator
* 24 MHz HSE crystal oscillator
* One user LED (LD1)
* Three push-buttons: user, reset, and boot
* USB Type-C® (USB full speed, Device mode)
* **CAN FD transceiver** onboard
* **Board connectors:**
  * USB Type-C® connector
  * MIPI10 connector for debugging (SWD, JTAG)
  * CAN FD connector (CN18)
  * ARDUINO® Uno V3 connector
  * ST morpho expansion connectors for full access to the STM32 I/Os
* Flexible power-supply options: ST-LINK USB VBUS, USB connector, or external sources
* On-board **STLINK-V3EC** debugger/programmer with USB re-enumeration capability: mass storage, Virtual COM port, and debug port

More information about STM32C562RE can be found in the [STM32C5 Reference Manual (RM0522)](https://www.st.com/resource/en/reference_manual/rm0522-stm32c5.pdf).

---

## Supported Features

The `nucleo_c562re` board supports the hardware features listed below:

| Type | Location | Description | Compatible String |
| :--- | :--- | :--- | :--- |
| **CPU** | on-chip | ARM Cortex-M33 CPU | `arm,cortex-m33` |
| **CAN** | on-chip / on-board | STM32 FDCAN CAN FD controller | `st,stm32-fdcan` |
| **ADC** | on-chip | STM32 ADC | `st,stm32n6-adc` |
| **DAC** | on-chip | STM32 family DAC | `st,stm32-dac` |
| **DMA** | on-chip | STM32U5 DMA controller | `st,stm32u5-dma` |
| **Clock control** | on-chip | STM32C5 RCC, HSE, LSE | `st,stm32c5-rcc` |
| **Crypto** | on-chip | HASH Processor & AES Accelerator | `st,stm32-hash`, `st,stm32-aes` |
| **GPIO** | on-chip | STM32 GPIO Controller | `st,stm32-gpio` |
| **Headers** | on-board | Arduino Uno R3 & ST Morpho | `arduino-header-r3`, `st-morpho-header` |
| **I2C / I3C** | on-chip | I2C V2 & I3C controller | `st,stm32-i2c-v2`, `st,stm32-i3c` |
| **SPI** | on-chip | STM32H7 SPI controller | `st,stm32h7-spi` |
| **Serial** | on-chip | LPUART, USART, UART | `st,stm32-lpuart`, `st,stm32-usart` |
| **USB** | on-chip | STM32 USB controller | `st,stm32-usb` |
| **Timer / PWM** | on-chip | General Purpose & Advanced Timers | `st,stm32-timers`, `st,stm32-pwm` |
| **RTC / WDT** | on-chip | Real-Time Clock & Watchdog | `st,stm32-rtc`, `st,stm32-watchdog` |

---

## Connections and IOs

Each of the GPIO pins can be configured by software as output (push-pull or open-drain), as input (with or without pull-up or pull-down), or as peripheral alternate function.

> ⚠️ **Pin Conflict Note:** In default configuration, there is a potential conflict on **PA5** (ARDUINO® D13) pin that is connected both to the green LED (LD1) and to SPI1 SCK (ARDUINO® SPI). It is not recommended to use both functions simultaneously.

### CAN FD Interface
The board has an onboard CAN FD transceiver connected to **FDCAN1**:
* `PB8`: CAN receive (`CAN_RX`)
* `PB9`: CAN transmit (`CAN_TX`)
* `PE2`: Transceiver standby control (Low = normal operation, High = standby)

The CAN FD bus is available on screw connector **CN18**:
* **Pin 1:** `CANH`
* **Pin 2:** `CANL`
* **Pin 3:** `GND`

> 💡 **Termination:** Jumper **JP9** connects the onboard 120-ohm termination resistor when closed.

### USB Configuration
By default, the dead-battery pull-downs are not present on the connector (self-powered mode). Therefore, the board cannot be powered from the user USB-C connector using a USB-C-to-USB-C cable. A legacy USB-C-to-USB-A cable works as expected, or you can power via the ST-Link USB port.

---

## Programming and Debugging

The `nucleo_c562re` board includes an integrated **STLINK-V3EC** debug tool interface.

| Runner | Flash | Debug | Attach | GDB Server |
| :--- | :---: | :---: | :---: | :---: |
| **STM32CubeProgrammer** | ✅ *(default)* | — | — | — |
| **ST-LINK GDB Server** | — | ✅ *(default)* | ✅ | ✅ |
| **pyOCD** | ✅ | ✅ | ✅ | ✅ |

### Flashing an application (CAN-FD Telemetrie)

Anwendung mit Zephyr RTOS kompilieren und auf das Board flashen:

```bash
# 1. CAN-FD Telemetrie-Projekt kompilieren
west build -b nucleo_c562re .

# 2. Auf das Nucleo-Board flashen
west flash
