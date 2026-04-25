#include "BlinkTask.hpp"

const int g_TimeToToggle = 500;

BlinkTask::
BlinkTask(Led& ars_Led)
: mrs_Led(ars_Led)
{}

void BlinkTask::
run()
{
  TickType_t ls_LastWakeTime = xTaskGetTickCount();
  TickType_t ls_Period = pdMS_TO_TICKS(g_TimeToToggle);
  while(1)
  {
    mrs_Led.toggle();
    vTaskDelayUntil(&ls_LastWakeTime, ls_Period);
  }
}
