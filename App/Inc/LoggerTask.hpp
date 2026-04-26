/**
 * @file LoggerTask.hpp
 * @author Subhash Chandra
 * @brief Header file for logger task
 * @version 0.1
 * @date 2026-04-26
 * 
 * @copyright Copyright (c) 2026
 * 
 */
#ifndef LOGGER_TASK_HPP
#define LOGGER_TASK_HPP

#include "Task.hpp"
#include "Queue.hpp"
#include "LogMessage.hpp"

/**
 * @brief Manages log message transmission from a LogMessage Queue reference.
 */
class LoggerTask : public Task
{
public:
  // Constructor with LogMessage Queue reference
  explicit LoggerTask(Queue<LogMessage, 16>& aps_Queue);

  // task entry with forever loop and task functionality
  void run() override;

  void onTxComplete();

private:
  Queue<LogMessage, 16>& ms_Queue; // Message Queue
  volatile bool mb_Busy{false}; // To hold all the messages, if one is being sent
  char mc_TxBuf[128]; // Buffer for storing transmission message

};

#endif // LOGGER_TASK_HPP
