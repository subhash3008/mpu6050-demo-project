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
* CONSTANTS
***************************************************/

// count of samples used for calibration of gyrometer
static constexpr uint16_t gu16_CalibrationSampleCount = 500U;

// delay for ImuTask
static constexpr uint16_t gu16_ImuTaskDelay = 500U; // TODO: change for needed freq later

/***************************************************
* HELPER FUNCTIONS
***************************************************/

/**
 * @brief Removes the bias from the gyrometer measuements
 * 
 * @param as_Data       Measurement data
 * @param as_GyroBias   bias from calibration to be removed
 */
void removeGyroBias(ImuDataScaled& as_Data, const ImuDataGyroBias as_GyroBias)
{
  as_Data.f_GxDps -= as_GyroBias.f_GxBias;
  as_Data.f_GyDps -= as_GyroBias.f_GyBias;
  as_Data.f_GzDps -= as_GyroBias.f_GzBias;
}

/***************************************************
* MEMBER FUNCTIONS
***************************************************/

/**
 * @brief Constructor for ImuTask that takes references to an ImuSensor and a Logger drivers,
 *        initializing its member references mps_Imu and ms_Logger with those arguments.
 * 
 * @param aps_Imu     IMU Driver reference
 * @param aps_Logger  Logger reference
 */
ImuTask::
ImuTask(ImuSensorDriver& aps_Imu, LoggerDriver& aps_Logger)
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
  ImuDataRaw ls_DataRaw;
  ImuDataScaled ls_DataScaled;

  ms_Logger.info("Performing Gyro calibration.");
  ImuDataGyroBias ls_GyroBias = mps_Imu.calibrateGyro(gu16_CalibrationSampleCount);
  ms_Logger.info("Gyro calibration complete.");

  while(1)
  {
    if (mps_Imu.readSensorData(ls_DataRaw)) // read the sensor data
    {
      mps_Imu.convertRawValuesToScaledValues(ls_DataRaw, ls_DataScaled);  // convert to desired units, g and deg/sec

      removeGyroBias(ls_DataScaled, ls_GyroBias); // remove bias from scaled data

      // ms_Logger.info(
      //   "IMU AX=%d, AY=%d, AZ=%d, GX=%d, GY=%d, GZ=%d",
      //   ls_DataRaw.i16_Ax, ls_DataRaw.i16_Ay, ls_DataRaw.i16_Az,
      //   ls_DataRaw.i16_Gx, ls_DataRaw.i16_Gy, ls_DataRaw.i16_Gz
      // );

      ms_Logger.info(
        "IMU AX=%.02f, AY=%.02f, AZ=%.02f, GX=%.02f, GY=%.02f, GZ=%.02f",
        ls_DataScaled.f_AxG, ls_DataScaled.f_AyG, ls_DataScaled.f_AzG,
        ls_DataScaled.f_GxDps, ls_DataScaled.f_GyDps, ls_DataScaled.f_GzDps
      );
    }

    vTaskDelayUntil(&lu_LastWake, gu16_ImuTaskDelay);
  }
}
