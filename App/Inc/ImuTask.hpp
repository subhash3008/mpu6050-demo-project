/**
 * @file ImuTask.hpp
 * @author Subhash Chandra
 * @brief Header file for IMU task
 * @version 0.1
 * @date 2026-04-26
 * 
 * @copyright Copyright (c) 2026
 * 
 */
#ifndef IMU_TASK_HPP
#define IMU_TASK_HPP

#include "Task.hpp"
#include "Queue.hpp"
#include "ImuSensor.hpp"
#include "Logger.hpp"

class ImuTask : public Task
{
public:
  // Constructor for ImuTask that initializes a new task instance
  // using references to an ImuSensor and a Logger.
  ImuTask(ImuSensor& aps_Imu, Logger& aps_Logger);

  // freertos task run function to implement forever loop and functionality
  void run() override;

private:
  ImuSensor& mps_Imu; // IMU Driver instance
  Logger& ms_Logger;  // Logger instance to log the data
};

#endif // IMU_TASK_HPP
