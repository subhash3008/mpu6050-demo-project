# STM32 FreeRTOS C++ IMU Demo using MPU-6050

## Overview

This project uses embedded architecture for **STM32 + C++ + FreeRTOS** using a single static `Application` object.

Features:

* FreeRTOS native API (no CMSIS-RTOS wrapper)
* Object-oriented task structure in C++
* Boot phases (`initDrivers`, `runDiagnostics`, `startTasks`, etc.)
* MPU-6050 accelerometer + gyroscope over I2C
* UART logger task
* LED heartbeat task
* Queue-based logging
* Safe startup / safe mode fallback

---

## Project Architecture

```text
main.c
 └── App_Start()
      └── static Application app
           ├── Led
           ├── BlinkTask
           ├── LoggerDriver
           ├── LoggerTask
           ├── ImuSensorDriver
           ├── ImuTask
```

---

## Naming Convention

### Scope Specifiers

* l  -> for local scope
* g  -> global scope
* m  -> member scope
* a  -> arguments

### Type Specifiers
* i -> integer
* u -> unsigned integer
* f -> float
* p -> pointer
* r -> reference
* s -> struct
* e -> enum


```code
uint8_t Foo::
function(uint8_t au8_Input, uint8_t* apu8_Input2, uint8_t& aru8_Input3)
{
  uint8_t lu8_Result = 0U;
  float lf_FloatVariable = 0.0f;
}
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

```text
[53501][INF] IMU AX=876, AY=-3392, AZ=14236, GX=-284, GY=237, GZ=-60
[54001][INF] IMU AX=808, AY=-3484, AZ=14348, GX=-261, GY=273, GZ=-43
```

---

## Hardware Requirements

* STM32 board supported by STM32CubeIDE (This project uses STM32F446RTE6 Nucleo Board)
* MPU-6050 module
* UART serial connection
* LED connected to configured GPIO pin

---

## Peripheral Configuration

### I2C1

Used for MPU-6050 communication.

Typical pins depend on MCU family.

In this Project: SDA Pin - PB8, SCL Pin - PB9

### USART2

Used for logging output.

Recommended terminal settings:

* 115200 baud
* 8 data bits
* no parity
* 1 stop bit

### GPIO

User LED configured as output.

---

## MPU-6050 Notes

Current driver behavior:

* Wakes device by clearing sleep bit (`PWR_MGMT_1 = 0x00`)
* Reads raw accelerometer and gyro registers
* Performs complimentary filtering to compensate for drift in gyroscope data and noise in accelerometer data

### Pitch and Roll
Pitch is the movement of object around horizontal axis i.e. an airplane's nose going up and down.
Roll is the movement of an object around the vertical axis i.e. an airplane's wings going up and down.
Only these two are currently considered in the project. Yaw is not being used currently.

### Theory for Complimentary filter

The filter implemented works as a way to complement two filter's transfer function so as to combine them i.e. becoming all pass filter. Gyroscope filtering uses high pass filter to compensate for the drift and Accelerometer filtering uses low pass filter to compensate for the noise.

Tuning weight (Alpha) is used to determine the percentage contribution for each sensor.
Formula becomes:
Fused Orientation Angle = Alpha * Gyro Prediction + (1 - Alpha) * Accelerometer angle

Here, 
* Alpha = Tuning weight
* Gyro Prediction = Previously Accumulated Angle + Integrated Gryoscope Data
  * Integrated Gryoscope Data = Gyroscope Anglular velocity * Sampling time interval
* Accelerometer Angle

Here, we want to stabilize the pitch and roll values and not combine them for now. So, we use the complimentary filter formula separately for both pitch and roll.
Hence, Formula becomes

Fused Data = Alpha * Accumulated Data + (1 - Alpha) * Current Data

---

## Build Notes (STM32CubeIDE)

* Enable C++ support in project settings
* Compile `.cpp` files with G++
* Keep Cube-generated files in C
* Link against FreeRTOS sources

---

## License

Use internally or adapt for your project.
