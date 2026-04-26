/**
 * @file Task.hpp
 * @author Subhash Chandra
 * @brief Wrapper for FreeRTOS task
 * @version 0.1
 * @date 2026-04-26
 * 
 * @copyright Copyright (c) 2026
 * 
 */
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

/**
 * @brief Base class for creating task in cpp application. It is a
 *        wrapper over the freertos task.
 */
class Task
{
public:
  virtual ~Task() = default;
  virtual void run() = 0;

  /**
   * @brief Creates the task
   * 
   * @param apc_Name        Name of the task
   * @param au16_StackWords Size of the task in stack words
   * @param alu_Priority    Priroty of the task 
   * @return true   if the task is successfully created
   * @return false  if the task is not successfully created
   */
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

private:
  TaskHandle_t ms_Handle{nullptr}; // Task handle

private:
  /**
   * @brief Entry function for the task
   * 
   * @param arg 
   */
  static void entry(void* arg)
  {
    // Cast the argument to the task reference and trigger the member run function
    static_cast<Task*>(arg)->run();
  }
};

#endif // TASK_HPP
