/**
 * @file WatchdogManager.cpp
 * @author Subhash chandra
 * @brief Implements member and associated helper functions for Watchdog manager
 * @version 0.1
 * @date 2026-07-24
 * 
 * @copyright Copyright (c) 2026
 * 
 */

/***************************************************
* INCLUDES
***************************************************/
#include "WatchdogManager.hpp"
#include "FaultManager.hpp"

#ifdef __cplusplus
extern "C"
{
#endif

#include "FreeRTOS.h"
#include "task.h"
#include "stm32f4xx.h"

#ifdef __cplusplus
}
#endif

/***************************************************
* External Variables
***************************************************/

#ifdef __cplusplus
extern "C"
{
#endif

extern IWDG_HandleTypeDef hiwdg;

#ifdef __cplusplus
}
#endif


/***************************************************
* Member static Variables
***************************************************/

// Member task array needs definition here as it is static
WatchdogManager::TaskStatus WatchdogManager::ms_Tasks[static_cast<uint8_t>(TaskId::COUNT)];

bool WatchdogManager::mb_StartupFinished;

/***************************************************
* Helper Function
***************************************************/
char* getTaskName(WatchdogManager::TaskId ae_TaskId)
{
  switch(ae_TaskId)
  {
    case WatchdogManager::TaskId::BLINK_TASK:
      return "BlinkTask";
    case WatchdogManager::TaskId::COM_TASK:
      return "ComTask";
    case WatchdogManager::TaskId::LOGGER_TASK:
      return "LoggerTask";
    case WatchdogManager::TaskId::IMU_TASK:
      return "ImuTask";
    default:
      return "InvalidTask";
  }
}

/***************************************************
* Member Functions
***************************************************/

void WatchdogManager::
init()
{
  const uint32_t lu32_Now = xTaskGetTickCount();

  WatchdogManager::ms_Tasks[static_cast<uint8_t>(TaskId::BLINK_TASK)] = {
    .u32_TimeoutMs = 1000U,
    .u32_LastAliveTick = lu32_Now
  };
  WatchdogManager::ms_Tasks[static_cast<uint8_t>(TaskId::COM_TASK)] = {
    .u32_TimeoutMs = 1000U,
    .u32_LastAliveTick = lu32_Now
  };
  WatchdogManager::ms_Tasks[static_cast<uint8_t>(TaskId::LOGGER_TASK)] = {
    .u32_TimeoutMs = 1000U,
    .u32_LastAliveTick = lu32_Now
  };
  WatchdogManager::ms_Tasks[static_cast<uint8_t>(TaskId::IMU_TASK)] = {
    .u32_TimeoutMs = 1000U,
    .u32_LastAliveTick = lu32_Now
  };
}

void WatchdogManager::
alive(TaskId ae_Id)
{
  ms_Tasks[static_cast<uint8_t>(ae_Id)].u32_LastAliveTick = xTaskGetTickCount();
}

void WatchdogManager::
process()
{
  // when the watchdog manager is not finished
  if (!mb_StartupFinished)
  {
    HAL_IWDG_Refresh(&hiwdg);
    return;
  }

  // Normal watchdog process
  const uint32_t lu32_Now = xTaskGetTickCount();

  for (uint8_t i = 0; i < static_cast<uint8_t>(TaskId::COUNT); ++i)
  {
    if (!ms_Tasks[i].b_Registered)
    {
      continue;
    }

    const uint32_t lu32_ElapsedTime = lu32_Now - ms_Tasks[i].u32_LastAliveTick;
    
    if (lu32_ElapsedTime > pdMS_TO_TICKS(ms_Tasks[i].u32_TimeoutMs))
    {
      /* This task has not reportd. Don't trigger the watchdog. */
      FaultManager::storeWatchdogFailure(getTaskName(static_cast<WatchdogManager::TaskId>(i)));
      return;
    }

    HAL_IWDG_Refresh(&hiwdg);
  }
}

void WatchdogManager::
registerTask(TaskId ae_Id)
{
  ms_Tasks[(uint8_t)ae_Id].b_Registered = true;
}