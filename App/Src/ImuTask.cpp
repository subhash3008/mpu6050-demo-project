/**
 * @file ImuTask.cpp
 * @author Subhash Chandra
 * @brief Implements task function and associated functionality for IMU
 * @version 0.1
 * @date 2026-04-26
 * 
 * @copyright Copyright (c) 2026
 * 
 */

/***************************************************
* INCLUDES
***************************************************/
#include "ImuTask.hpp"

/***************************************************
* MEMBER FUNCTIONS
***************************************************/

/**
 * @brief Constructor for ImuTask that takes references to an ImuSensor and a Logger,
 *        initializing its member references mps_Imu and ms_Logger with those arguments.
 * 
 * @param aps_Imu     IMU Driver reference
 * @param aps_Logger  Logger reference
 */
ImuTask::
ImuTask(ImuSensor& aps_Imu, Logger& aps_Logger)
: mps_Imu(aps_Imu), ms_Logger(aps_Logger)
{}


/**
 * @brief Overridden task entry point that periodically reads IMU register data, processes
 *        the logs the accelerometer and gyroscope values if the read succeeds.
 */
void ImuTask::
run()
{
  TickType_t lu_LastWake = xTaskGetTickCount();
  uint16_t lu16_ImuTaskDelay = 500U;
  ImuData ls_Data;

  while(1)
  {
    if (mps_Imu.readReg(ls_Data))
    {
      // mps_DataQueue.send(ls_Data);
      ms_Logger.info(
        "IMU AX=%d, AY=%d, AZ=%d, GX=%d, GY=%d, GZ=%d",
        ls_Data.i16_AX, ls_Data.i16_AY, ls_Data.i16_AZ,
        ls_Data.i16_GX, ls_Data.i16_GY, ls_Data.i16_GZ
      );
    }

    vTaskDelayUntil(&lu_LastWake, lu16_ImuTaskDelay);
  }
}
