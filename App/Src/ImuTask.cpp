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
static constexpr uint16_t gu16_CalibrationSampleCount = 100U;

// delay needed between calibration sample collection in ms
static constexpr uint8_t gu8_CalibrationTimeInterval = 50U;

// delay for ImuTask
static constexpr uint16_t gu16_ImuTaskDelay = 500U; // TODO: change for needed freq later

// sampling rate of the task - linked to gu16_ImuTaskDelay as 1000 / gu16_ImuTaskDelay
static constexpr float gf_ImuRateInHz = 2.0f;

// dt is the time between two filter updates
// since we are using vTaskDelayUntil constant value can be used
static constexpr float gf_ImuTaskDt = 1.0f / gf_ImuRateInHz;

/***************************************************
* HELPER FUNCTIONS
***************************************************/


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
  ImuDataGyroBias ls_GyroBias = mps_Imu.calibrateGyro(gu16_CalibrationSampleCount, gu8_CalibrationTimeInterval);
  ms_Logger.info("Gyro calibration complete.");

  while(1)
  {
    if (mps_Imu.readSensorData(ls_DataRaw)) // read the sensor data
    {
      mps_Imu.convertRawValuesToScaledValues(ls_DataRaw, ls_DataScaled);  // convert to desired units, g and deg/sec

      mps_Imu.removeGyroBias(ls_DataScaled, ls_GyroBias); // remove bias from scaled data

      mps_Imu.integrateGyro(ls_DataScaled, gf_ImuTaskDt);

      float lf_PitchAcc = mps_Imu.calculatePitchFromAccel(ls_DataScaled);
      float lf_RollAcc = mps_Imu.calculateRollFromAccel(ls_DataScaled);

      mps_Imu.applyComplimentaryFilter(ls_DataScaled, lf_PitchAcc, lf_RollAcc);

      // ms_Logger.info(
      //   "IMU AX=%d, AY=%d, AZ=%d, GX=%d, GY=%d, GZ=%d",
      //   ls_DataRaw.i16_Ax, ls_DataRaw.i16_Ay, ls_DataRaw.i16_Az,
      //   ls_DataRaw.i16_Gx, ls_DataRaw.i16_Gy, ls_DataRaw.i16_Gz
      // );

      ms_Logger.info(
        "IMU AX=%.02f, AY=%.02f, AZ=%.02f, GX=%.02f, GY=%.02f, GZ=%.02f, Pitch=%.02f, Roll=%.02f", 
        ls_DataScaled.f_AxG, ls_DataScaled.f_AyG, ls_DataScaled.f_AzG,
        ls_DataScaled.f_GxDps, ls_DataScaled.f_GyDps, ls_DataScaled.f_GzDps,
        ls_DataScaled.f_PitchDeg, ls_DataScaled.f_RollDeg
      );
    }

    vTaskDelayUntil(&lu_LastWake, gu16_ImuTaskDelay);
  }
}
