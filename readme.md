# STM32 FreeRTOS C++ MPU-6050 Demo

## Overview

This project demonstrates a clean embedded architecture for **STM32 + C++ + FreeRTOS** using a single static `Application` object.

Features:

* FreeRTOS native API (no CMSIS-RTOS wrapper)
* Object-oriented task structure in C++
* Boot phases (`initDrivers`, `runDiagnostics`, `startTasks`, etc.)
* MPU-6050 accelerometer + gyroscope over I2C
* UART logger task
* LED heartbeat task
* Queue-based inter-task communication
* Safe startup / safe mode fallback

---

## Project Architecture

```text
main.c
 └── App_Start()
      └── static Application app
           ├── Led statusLed
           ├── Queue<ImuData>
           ├── Mpu6050 driver
           ├── BlinkTask
           ├── ImuTask
           └── LoggerTask
```

---

## Boot Flow

```text
HAL_Init()
SystemClock_Config()
Peripheral Init
App_Start()
  -> initDrivers()
  -> runDiagnostics()
  -> loadConfiguration()
  -> startTasks()
  -> enterNormalMode()
vTaskStartScheduler()
```

If diagnostics fail, the system enters **safe mode** and blinks the LED rapidly.

---

## Tasks

### BlinkTask

* Toggles status LED every 500 ms.
* Confirms scheduler is alive.

### ImuTask

* Reads MPU-6050 every 100 ms.

### LoggerTask

* Sends formatted output over UART.

Example UART output:

---

## Hardware Requirements

* STM32 board supported by STM32CubeIDE
* MPU-6050 module
* USB/UART serial connection
* LED connected to configured GPIO pin

---

## Peripheral Configuration

### I2C1

Used for MPU-6050 communication.

Typical pins depend on MCU family.

### USART2

Used for logging output.

Recommended terminal settings:

* 115200 baud
* 8 data bits
* no parity
* 1 stop bit

### GPIO

Status LED configured as output push-pull.

---

## MPU-6050 Notes

Current driver behavior:

* Wakes device by clearing sleep bit (`PWR_MGMT_1 = 0x00`)
* Reads raw accelerometer and gyro registers
* No scaling/filtering yet

---

## Build Notes (STM32CubeIDE)

* Enable C++ support in project settings
* Compile `.cpp` files with G++
* Keep Cube-generated files in C
* Link against FreeRTOS sources

---

## License

Use internally or adapt for your project.
