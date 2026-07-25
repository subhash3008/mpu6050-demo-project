/**
 * @file WatchdogTask.hpp
 * @author Subhash Chandra
 * @brief Task for handling watchdog manager
 * @version 0.1
 * @date 2026-07-25
 * 
 * @copyright Copyright (c) 2026
 * 
 */

#ifndef WATCHDOG_TASK_HPP
#define WATCHDOG_TASK_HPP

#include "Task.hpp"

class WatchdogTask : public Task
{
public:
  void run() override;
};


#endif // WATCHDOG_TASK_HPP