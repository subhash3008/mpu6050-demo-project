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
#include "ImuSensorDriver.hpp"
#include "LoggerDriver.hpp"

class ImuTask : public Task
{
public:
  // Constructor for ImuTask that initializes a new task instance
  // using references to an ImuSensor and a Logger.
  ImuTask(ImuSensorDriver& aps_Imu, LoggerDriver& aps_Logger);

  // freertos task run function to implement forever loop and functionality
  void run() override;

private:
  ImuSensorDriver& mps_Imu; // IMU Driver instance
  LoggerDriver& ms_Logger;  // Logger instance to log the data
};

#endif // IMU_TASK_HPP
