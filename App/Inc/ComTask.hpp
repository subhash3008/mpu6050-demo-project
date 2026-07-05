/**
 * @file ComTask.hpp
 * @author Subhash Chandra
 * @brief Header file for Command communication task
 * @version 0.1
 * @date 2026-04-26
 * 
 * @copyright Copyright (c) 2026
 * 
 */
#ifndef COM_TASK_HPP
#define COM_TASK_HPP

#include "Task.hpp"
#include "Queue.hpp"
#include "ComProtocol.hpp"
#include "LoggerDriver.hpp"

/**
 * @brief Manages incoming commands and provides response.
 */
class ComTask : public Task
{
public:
  // Constructor with LogMessage Queue reference
  explicit ComTask(ComProtocol& aps_Com, LoggerDriver& aps_Logger);

  // task entry with forever loop and task functionality
  void run() override;

  void handleRxByte();

private:
  ComProtocol& ms_Com;
  LoggerDriver& ms_Logger;
};

#endif // COM_TASK_HPP
