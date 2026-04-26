/**
 * @file BlinkTask.hpp
 * @author Subahsh Chandra
 * @brief Header file for defining a task for handling user led operation
 * @version 0.1
 * @date 2026-04-26
 * 
 * @copyright Copyright (c) 2026
 * 
 */

#ifndef BLINK_TASK_HPP
#define BLINK_TASK_HPP

#include "Task.hpp"
#include "Led.hpp"

/**
 * @brief Freertos task which handles blinking of the user led
 * 
 */
class BlinkTask : public Task
{
public:
  // Constructor which initializes the blinktask instance with
  // a reference to an existing Led object.
  BlinkTask(Led& ars_Led);

  // task function which must have a forever loop and implement
  // the task functionality
  void run() override;

private:
  Led& mrs_Led; // Led driver reference to be blinked
};

#endif // BLINK_TASK_HPP
