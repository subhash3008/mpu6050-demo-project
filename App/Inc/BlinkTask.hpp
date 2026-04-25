#ifndef BLINK_TASK_HPP
#define BLINK_TASK_HPP

#include "Task.hpp"
#include "Led.hpp"

class BlinkTask : public Task
{
public:
  BlinkTask(Led& ars_Led);
  void run() override;

private:
  Led& mrs_Led;
};

#endif // BLINK_TASK_HPP
