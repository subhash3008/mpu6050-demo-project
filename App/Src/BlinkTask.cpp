/**
 * @file BlinkTask.cpp
 * @author Subhash Chandra
 * @brief Implements class member and associated functions for BlinkTask
 * @version 0.1
 * @date 2026-04-26
 * 
 * @copyright Copyright (c) 2026
 * 
 */

/***************************************************
* INCLUDES
***************************************************/
#include "BlinkTask.hpp"

/***************************************************
* Private Variables
***************************************************/
const uint16_t gu16_TimeToToggle = 500U;

/***************************************************
* Member Functions
***************************************************/

/**
 * @brief Constructor that initializes BlinkTask instance by binding
 *        its member reference mrs_Led to the provided Led object.
 */
BlinkTask::
BlinkTask(Led& ars_Led)
: mrs_Led(ars_Led)
{}

/**
 * @brief overrides the base task runner and enters an infinite loop where
 *        it toggles mrs_Led and then delays until the next period.
 * 
 */
void BlinkTask::
run()
{
  TickType_t ls_LastWakeTime = xTaskGetTickCount();
  TickType_t ls_Period = pdMS_TO_TICKS(gu16_TimeToToggle);
  while(1)
  {
    mrs_Led.toggle();
    vTaskDelayUntil(&ls_LastWakeTime, ls_Period);
  }
}
