#ifndef TASK_HPP
#define TASK_HPP

#ifdef __cplusplus
extern "C"
{
#endif

#include "FreeRTOS.h"
#include "task.h"

#ifdef __cplusplus
}
#endif

#include <stdint.h>

class Task
{
public:
  virtual ~Task() = default;
  virtual void run() = 0;

  bool start(
    const char* apc_Name,
    uint16_t au16_StackWords,
    UBaseType_t alu_Priority)
  {
    return xTaskCreate(
      Task::entry,
      apc_Name,
      au16_StackWords,
      this,
      alu_Priority,
      &ms_Handle) == pdPASS;
  }

protected:
  TaskHandle_t ms_Handle{nullptr};

private:
  static void entry(void* arg)
  {
    static_cast<Task*>(arg)->run();
  }
};

#endif // TASK_HPP
