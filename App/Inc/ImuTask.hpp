#ifndef IMU_TASK_HPP
#define IMU_TASK_HPP

#include "Task.hpp"
#include "Queue.hpp"
#include "ImuSensor.hpp"
#include "Logger.hpp"

class ImuTask : public Task
{
public:
  // ImuTask(ImuSensor& aps_Imu, Queue<ImuData, 16>& aps_DataQueue);
  ImuTask(ImuSensor& aps_Imu, Logger& aps_Logger);

  void run() override;

private:
  ImuSensor& mps_Imu;
  Logger& ms_Logger;
  // Queue<ImuData, 16>& mps_DataQueue;
};

#endif // IMU_TASK_HPP
