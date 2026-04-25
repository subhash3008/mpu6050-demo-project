#ifndef LOGGER_TASK_HPP
#define LOGGER_TASK_HPP

#include "Task.hpp"
#include "Queue.hpp"
#include "LogMessage.hpp"

class LoggerTask : public Task
{
public:
  explicit LoggerTask(Queue<LogMessage, 16>& aps_Queue);

  void run() override;

  void onTxComplete();

private:
  Queue<LogMessage, 16>& ms_Queue;
  volatile bool mb_Busy{false};
  char mc_TxBuf[128];

};

#endif // LOGGER_TASK_HPP
