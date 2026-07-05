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
ImuTask(ImuSensorDriver& aps_Imu, LoggerDriver& aps_Logger, ComProtocol& aps_Com)
: mps_Imu(aps_Imu), ms_Logger(aps_Logger), ms_Com(aps_Com)
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

  bool lb_StreamSensorData = false;

  ms_Logger.info("Performing Gyro calibration.");
  ImuDataGyroBias ls_GyroBias = mps_Imu.calibrateGyro(gu16_CalibrationSampleCount, gu8_CalibrationTimeInterval);
  ms_Logger.info("Gyro calibration complete.");

  ComProtocol::CommandPacket ls_ComPacket;

  while(1)
  {
    if (lb_StreamSensorData)
    {
      if (mps_Imu.readSensorData(ls_DataRaw)) // read the sensor data
      {
        mps_Imu.convertRawValuesToScaledValues(ls_DataRaw, ls_DataScaled);  // convert to desired units, g and deg/sec

        mps_Imu.removeGyroBias(ls_DataScaled, ls_GyroBias); // remove bias from scaled data

        mps_Imu.integrateGyro(ls_DataScaled, gf_ImuTaskDt);

        float lf_PitchAcc = mps_Imu.calculatePitchFromAccel(ls_DataScaled);
        float lf_RollAcc = mps_Imu.calculateRollFromAccel(ls_DataScaled);

        mps_Imu.applyComplimentaryFilter(ls_DataScaled, lf_PitchAcc, lf_RollAcc);

        ms_Logger.info(
          "IMU AX=%.02f, AY=%.02f, AZ=%.02f, GX=%.02f, GY=%.02f, GZ=%.02f, Pitch=%.02f, Roll=%.02f", 
          ls_DataScaled.f_AxG, ls_DataScaled.f_AyG, ls_DataScaled.f_AzG,
          ls_DataScaled.f_GxDps, ls_DataScaled.f_GyDps, ls_DataScaled.f_GzDps,
          ls_DataScaled.f_PitchDeg, ls_DataScaled.f_RollDeg
        );
      }
    }

    if (ms_Com.getNextCommand(ls_ComPacket))
    {
      switch (ls_ComPacket.command)
      {
        case ComProtocol::Command::GET_STATUS:
          ms_Com.processResponse(ComProtocol::Response::STATUS, NULL, 0);
          ms_Logger.info("Received command: Get status.");
          break;
        
        case ComProtocol::Command::GET_ORIENTATION:
          if (mps_Imu.readSensorData(ls_DataRaw)) // read the sensor data
          {
            ms_Com.processResponse(ComProtocol::Response::ORIENTATION, NULL, 0);

            mps_Imu.convertRawValuesToScaledValues(ls_DataRaw, ls_DataScaled);  // convert to desired units, g and deg/sec

            mps_Imu.removeGyroBias(ls_DataScaled, ls_GyroBias); // remove bias from scaled data

            mps_Imu.integrateGyro(ls_DataScaled, gf_ImuTaskDt);

            float lf_PitchAcc = mps_Imu.calculatePitchFromAccel(ls_DataScaled);
            float lf_RollAcc = mps_Imu.calculateRollFromAccel(ls_DataScaled);

            mps_Imu.applyComplimentaryFilter(ls_DataScaled, lf_PitchAcc, lf_RollAcc);

            ms_Logger.info(
              "Orientation : Pitch=%.02f, Roll=%.02f",
              ls_DataScaled.f_PitchDeg, ls_DataScaled.f_RollDeg
            );
          }
          break;
        
        case ComProtocol::Command::START_STREAM:
          lb_StreamSensorData = true;
          ms_Com.processResponse(ComProtocol::Response::ACK, NULL, 0);
          ms_Logger.info("Starting sensor data stream.");
          break;
        
        case ComProtocol::Command::STOP_STREAM:
          lb_StreamSensorData = false;
          ms_Com.processResponse(ComProtocol::Response::ACK, NULL, 0);
          ms_Logger.info("Stopped streaming.");
          break;
        default:
          ms_Logger.error("Invalid command !!");
      }
    }

    vTaskDelayUntil(&lu_LastWake, gu16_ImuTaskDelay);
  }
}
