/**
 * @file FaultManager.cpp
 * @author Subhash Chandra
 * @brief Implements FaultManager class member and associated helper functions
 * @version 0.1
 * @date 2026-07-23
 * 
 * @copyright Copyright (c) 2026
 * 
 */

/***************************************************
* INCLUDES
***************************************************/
#include "FaultManager.hpp"
#include <cstring>

#ifdef __cplusplus
extern "C"
{
#endif

#include "main.h"
#include "stm32f446xx.h"
#include "cmsis_gcc.h"

#include "FreeRTOS.h"
#include "task.h"

#ifdef __cplusplus
}
#endif

/***************************************************
* Global variables and definitions
***************************************************/

static constexpr uint32_t gu32_MagicValue = 0xDEADBEEF;

namespace
{
  __attribute__((section(".noinit"), used))
  volatile CrashInfo g_CrashInfo;
}

/***************************************************
* Helper Functions
***************************************************/
/**
 * @brief Checks if there was caused a watchdog reset
 * @param None
 * @return None
 */
bool handleIfWatchdogReset(CrashInfo& as_CrashInfo)
{
  uint32_t lu32_ResetFlags = RCC->CSR;

  if (lu32_ResetFlags & RCC_CSR_IWDGRSTF)
  {
    as_CrashInfo.u32_Magic = gu32_MagicValue;
    as_CrashInfo.e_FaultType = FaultType::WATCHDOG_FAULT;
  }
}


/***************************************************
* Member Functions
***************************************************/

CrashInfo& FaultManager::
Storage()
{
  return const_cast<CrashInfo&>(g_CrashInfo);
}

void FaultManager::
init()
{
  if (hasFault() ||
      handleIfWatchdogReset(Storage()))
  {
    Storage().u32_ResetCounter++;
  }
}

bool FaultManager::
hasFault()
{
  return (Storage().u32_Magic == gu32_MagicValue);
}

void FaultManager::
clear()
{
  Storage().u32_Magic = 0x00U;
  __HAL_RCC_CLEAR_RESET_FLAGS();
}

FaultType FaultManager::
getFaultType()
{
  return Storage().e_FaultType;
}

const char* FaultManager::
getFaultTypeString()
{
  switch(Storage().e_FaultType)
  {
    case FaultType::HARD_FAULT:
      return "HardFault";
    case FaultType::ASSERT_FAULT:
      return "Assert";
    case FaultType::WATCHDOG_FAULT:
      return "Watchdog";
    case FaultType::STACKOVERFLOW_FAULT:
      return "StackOverflow";
    default:
      return "None";
  }
}

const char* FaultManager::
getFaultTaskName()
{
  return Storage().s_FailedTaskName;
}

uint32_t FaultManager::
getPc()
{
  return Storage().u32_Pc;
}

uint32_t FaultManager::
getLr()
{
  return Storage().u32_Lr;
}

uint32_t FaultManager::
getCfsr()
{
  return Storage().u32_Cfsr;
}

uint32_t FaultManager::
getHfsr()
{
  return Storage().u32_Hfsr;
}

uint32_t FaultManager::
getBfar()
{
  return Storage().u32_Bfar;
}

uint32_t FaultManager::
getMmfar()
{
  return Storage().u32_Mmfar;
}

void FaultManager::
assertFailed(const char* apc_File, uint32_t au32_Line)
{
  (void)apc_File;

  CrashInfo& ls_Crash = Storage();

  ls_Crash.u32_Magic = gu32_MagicValue;
  ls_Crash.e_FaultType = FaultType::ASSERT_FAULT;
  ls_Crash.u32_Line = au32_Line;

  __DSB();
  __ISB();

  NVIC_SystemReset();

  // Await system reset
  while (true)
  {}
}

void FaultManager::
storeStackOverflow(char* as_TaskName)
{
  CrashInfo& ls_Crash = Storage();

  ls_Crash.u32_Magic = gu32_MagicValue;
  ls_Crash.e_FaultType = FaultType::STACKOVERFLOW_FAULT;
  strncpy(ls_Crash.s_FailedTaskName, as_TaskName, sizeof(ls_Crash.s_FailedTaskName) - 1);
}

void FaultManager::
storeWatchdogFailure(const char* as_TaskName)
{
  CrashInfo& ls_Crash = Storage();

  ls_Crash.u32_Magic = gu32_MagicValue;
  ls_Crash.e_FaultType = FaultType::WATCHDOG_FAULT;
  strncpy(ls_Crash.s_FailedTaskName, as_TaskName, sizeof(ls_Crash.s_FailedTaskName) - 1);
}

void FaultManager::
processHardFault(uint32_t* apu32_StackFrame)
{
  CrashInfo& ls_Crash = Storage();

  ls_Crash.u32_Magic = gu32_MagicValue;
  ls_Crash.e_FaultType = FaultType::HARD_FAULT;

  ls_Crash.u32_R0 = apu32_StackFrame[0];
  ls_Crash.u32_R1 = apu32_StackFrame[1];
  ls_Crash.u32_R2 = apu32_StackFrame[2];
  ls_Crash.u32_R3 = apu32_StackFrame[3];
  ls_Crash.u32_R12 = apu32_StackFrame[4];

  ls_Crash.u32_Lr = apu32_StackFrame[5];
  ls_Crash.u32_Pc = apu32_StackFrame[6];
  ls_Crash.u32_Psr = apu32_StackFrame[7];

  ls_Crash.u32_Cfsr = SCB->CFSR;
  ls_Crash.u32_Hfsr = SCB->HFSR;
  ls_Crash.u32_Bfar = SCB->BFAR;
  ls_Crash.u32_Mmfar = SCB->MMFAR;

  __DSB();
  __ISB();

  NVIC_SystemReset();

  while(true)
  {}
}

/***************************************************
* HardFault Handling
***************************************************/

#ifdef __cplusplus
extern "C"
{
#endif

__attribute__((naked))
void HardFault_Handler(void)
{
  __asm volatile
  (
    "TST LR, #4        \n"  // Tests bit 2 of the special EXC_RETURN value stored in LR. 0 -> MSP used, 1 -> PSP used
    "ITE EQ            \n"  // If-Then-Else
    "MRSEQ R0, MSP     \n"  // If MSP was active, copies r0 for stackframe
    "MRSNE R0, PSP     \n"  // otherwise, copies PSP to r0 for stackframe
    "B HardFault_C     \n"  // calls the HardFault_C function
  );
}

void HardFault_C(uint32_t* stackFrame)
{
  FaultManager::processHardFault(stackFrame);
}

/**
 * @brief Hook triggered when stack overflow occurs
 * 
 */
void vApplicationStackOverflowHook(TaskHandle_t xTask, char* pcTaskName)
{
  FaultManager::storeStackOverflow(pcTaskName);

  NVIC_SystemReset();

  while (true)
  {}
}

#ifdef __cplusplus
}
#endif