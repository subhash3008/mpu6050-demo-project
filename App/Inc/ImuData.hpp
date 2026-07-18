/**
 * @file ImuData.hpp
 * @author Subhash Chandra
 * @brief Header file for data structure for data from MPU-6050 Sensor
 * @version 0.1
 * @date 2026-04-26
 * 
 * @copyright Copyright (c) 2026
 * 
 */

#ifndef IMUDATA_HPP
#define IMUDATA_HPP

#include <stdint.h>

/**
 * @brief Defines the data structure for fetching data from
 *        MPU-6050 sensor. 6 raw values, 3 from accelerometer
 *        and 3 from gyrometer. 
 */
struct ImuDataRaw
{
  int16_t i16_Ax; // Accelerometer X-Axis.
  int16_t i16_Ay; // Accelerometer Y-Axis.
  int16_t i16_Az; // Accelerometer Z-Axis.

  int16_t i16_Gx; // Gyrometer X-Axis.
  int16_t i16_Gy; // Gyrometer Y-Axis.
  int16_t i16_Gz; // Gyrometer Z-Axis.
};

/**
 * @brief Defines data structure for processing the raw fetched data 
 *        from the sensor. Float data types are needed to handle
 *        values of accelerometer in g and values from gyrometer
 *        as angular velocity in degree/second
 */
struct ImuDataScaled
{
  float f_AxG; // Accelerometer X-Axis.
  float f_AyG; // Accelerometer Y-Axis.
  float f_AzG; // Accelerometer Z-Axis.

  float f_GxDps; // Gyrometer X-Axis.
  float f_GyDps; // Gyrometer Y-Axis.
  float f_GzDps; // Gyrometer Z-Axis.

  float f_PitchDeg; // for definitions, read readme
  float f_RollDeg;  // for definitions, read readme
};

/**
 * @brief Data structure for holding the bias for gyrometer callibration.
 *        With time, the gyro data seems to be drifting. To handle this,
 *        the gyro data needs to be corrected with callibrated bias.
 */
struct ImuDataGyroBias
{
  float f_GxBias;
  float f_GyBias;
  float f_GzBias;

  float f_AxBias;
  float f_AyBias;
  float f_AzBias;
};

#endif // IMUDATA_HPP
