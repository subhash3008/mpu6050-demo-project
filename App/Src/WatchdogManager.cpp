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
* Member Functions
***************************************************/

void WatchdogManager::
init()
{
  const uint32_t lu32_Now = xTaskGetTickCount();

  ms_Tasks[static_cast<uint8_t>(TaskId::BLINK_TASK)] = {
    .u32_TimeoutMs = 1000U,
    .u32_LastAliveTick = lu32_Now
  };
  ms_Tasks[static_cast<uint8_t>(TaskId::COM_TASK)] = {
    .u32_TimeoutMs = 1000U,
    .u32_LastAliveTick = lu32_Now
  };
  ms_Tasks[static_cast<uint8_t>(TaskId::LOGGER_TASK)] = {
    .u32_TimeoutMs = 1000U,
    .u32_LastAliveTick = lu32_Now
  };
  ms_Tasks[static_cast<uint8_t>(TaskId::IMU_TASK)] = {
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
  const uint32_t lu32_Now = xTaskGetTickCount();

  for (uint8_t i = 0; i < static_cast<uint8_t>(TaskId::COUNT); ++i)
  {
    const uint32_t lu32_ElapsedTime = lu32_Now - ms_Tasks[i].u32_LastAliveTick;
    
    if (lu32_ElapsedTime > pdMS_TO_TICKS(ms_Tasks[i].u32_TimeoutMs))
    {
      /* This task has not reportd. Don't trigger the watchdog. */
      return;
    }

    HAL_IWDG_Refresh(&hiwdg);
  }
}