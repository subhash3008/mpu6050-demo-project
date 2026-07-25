/**
 * @file WatchdogTask.cpp
 * @author Subhash Chandra
 * @brief Implements member functions for watchdog task
 * @version 0.1
 * @date 2026-07-25
 * 
 * @copyright Copyright (c) 2026
 * 
 */

/***************************************************
* INCLUDES
***************************************************/
#include "WatchdogTask.hpp"
#include "WatchdogManager.hpp"

/***************************************************
* Global variables
***************************************************/
constexpr uint16_t gu32_WatchdogTolerance = 250U;

/***************************************************
* Member Functions
***************************************************/
void WatchdogTask::
run()
{
  TickType_t lu32_LastWake = xTaskGetTickCount();

  WatchdogManager::init();

  while (1)
  {
    WatchdogManager::process();

    vTaskDelayUntil(&lu32_LastWake, gu32_WatchdogTolerance);
  }
}