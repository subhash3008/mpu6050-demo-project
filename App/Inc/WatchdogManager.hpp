/**
 * @file WatchdogManager.hpp
 * @author Subhash Chandra
 * @brief Handle Watchdog for the application
 * @version 0.1
 * @date 2026-07-24
 * 
 * @copyright Copyright (c) 2026
 * 
 */

#ifndef WATCHDOG_MANAGER_HPP
#define WATCHDOG_MANAGER_HPP

#include <cstdint>

class WatchdogManager
{
public:
  enum class TaskId : uint8_t
  {
    BLINK_TASK,
    LOGGER_TASK,
    COM_TASK,
    IMU_TASK,

    COUNT
  };

  static void init();

  static void alive(TaskId ae_Id);

  static void process();

private:
  struct TaskStatus
  {
    uint32_t u32_TimeoutMs;
    uint32_t u32_LastAliveTick;
  };

  static TaskStatus ms_Tasks[static_cast<uint8_t>(TaskId::COUNT)];
};

#endif // WATCHDOG_MANAGER_HPP