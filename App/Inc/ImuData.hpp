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
 * @brief Defines the data structure for processing data from
 *        MPU-6050 sensor. 6 raw values, 3 from accelerometer
 *        and 3 from gyrometer. 
 */
struct ImuData
{
  int16_t i16_AX; // Accelerometer X-Axis.
  int16_t i16_AY; // Accelerometer Y-Axis.
  int16_t i16_AZ; // Accelerometer Z-Axis.

  int16_t i16_GX; // Gyrometer X-Axis.
  int16_t i16_GY; // Gyrometer Y-Axis.
  int16_t i16_GZ; // Gyrometer Z-Axis.
};

#endif // IMUDATA_HPP
