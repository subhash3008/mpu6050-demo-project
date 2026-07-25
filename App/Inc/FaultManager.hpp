/**
 * @file FaultManager.hpp
 * @author Subhash Chandra
 * @brief Header for handling faults in the application
 * @version 0.1
 * @date 2026-07-23
 * 
 * @copyright Copyright (c) 2026
 * 
 */

#ifndef FAULT_MANAGER_HPP
#define FAULT_MANAGER_HPP

#include <cstdint>

enum class FaultType : uint32_t
{
  NONE                = 0,
  HARD_FAULT          = 1,
  ASSERT_FAULT        = 2,
  WATCHDOG_FAULT      = 3,
  STACKOVERFLOW_FAULT = 4
};

struct CrashInfo
{
  uint32_t u32_Magic;
  FaultType e_FaultType;
  uint32_t u32_ResetCounter;

  uint32_t u32_R0; // R0-R3, R12 -> arguments passed to crashing function
  uint32_t u32_R1;
  uint32_t u32_R2;
  uint32_t u32_R3;
  uint32_t u32_R12;

  uint32_t u32_Lr;  // Link Register -> contains return address, as in who called the function which generated the fault
  uint32_t u32_Pc;  // Program Counter -> Where the crash happened
  uint32_t u32_Psr; // contains current exception number, condition flags, thumb state

  uint32_t u32_Cfsr;  // Configurable Fault Status Register, contains MemFault (0-7), BusFault(8-15), Usage faults(16-31)
  uint32_t u32_Hfsr;  // Hard Fault Status Register
  uint32_t u32_Mmfar; // Memory Management Fault Address Register, contains address causing mpu fault
  uint32_t u32_Bfar;  // Bus Fault Address Register, contains faulting memory address

  char s_FailedTaskName[16]; // Name of the task which overflowed or triggered the watchdog

  uint32_t u32_Line;
};

class FaultManager
{
public:

  static void init();

  static bool hasFault();

  static void clear();

  static FaultType getFaultType();
  static const char* getFaultTypeString();
  static const char* getFaultTaskName();

  static uint32_t getPc();
  static uint32_t getLr();
  static uint32_t getCfsr();
  static uint32_t getHfsr();
  static uint32_t getBfar();
  static uint32_t getMmfar();

  static void assertFailed(const char* apc_File, uint32_t au32_Line);

  static void processHardFault(uint32_t* apu32_StackFrame);

  static void storeStackOverflow(char* as_TaskName);

  static void storeWatchdogFailure(const char* as_TaskName);

private:
  static CrashInfo& Storage();
};

#endif // FAULT_MANAGER_HPP