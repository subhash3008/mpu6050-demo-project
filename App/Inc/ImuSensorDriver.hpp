/**
 * @file ImuSensorDriver.hpp
 * @author Subhash Chandra
 * @brief Header for driver for sensor MPU-6050
 * @version 0.1
 * @date 2026-04-26
 * 
 * @copyright Copyright (c) 2026
 * 
 */

#ifndef IMU_SENSOR_HPP
#define IMU_SENSOR_HPP

#include "ImuData.hpp"

#ifdef __cplusplus
extern "C"
{
#endif

#include "stm32f4xx_hal.h"

#ifdef __cplusplus
}
#endif

/**
 * @brief Driver class for MPU6050 Sensor
 * 
 */
class ImuSensorDriver
{
public:
  // Constructor that explicitly initializes an ImuSensorDriver instance
  // using a reference to an I2C handle.
  explicit ImuSensorDriver(I2C_HandleTypeDef& aps_HandleI2C);

  // Initialize the sensor functionalities
  bool init();
  // Checks the sensor identity
  bool whoAmI(uint8_t& au8_Id);
  // Read data from mpu6050 sensor
  bool readSensorData(ImuDataRaw& aps_Data);

  // convert raw sensor data to g and deg/sec for accelerometer and gyrometer
  void convertRawValuesToScaledValues(const ImuDataRaw& aps_DataRaw, ImuDataScaled& aps_DataScaled);

  // Calibrate the gyro sensor
  ImuDataGyroBias calibrateGyro(const uint16_t au16_SampleCount, const uint8_t au8_TimeInterval);

  // Remove bias from the gyro data
  void removeGyroBias(ImuDataScaled& as_Data, const ImuDataGyroBias as_GyroBias);

  // Integrate gyro rates into angles
  void integrateGyro(ImuDataScaled& as_Data, const float af_Dt);

  // Calculate pitch from accelerometer data
  float calculatePitchFromAccel(const ImuDataScaled& as_Data);

  // Calculate roll from accelerometer data
  float calculateRollFromAccel(const ImuDataScaled& as_Data);
  
  // Apply fiter for compensating gyro drift and accelerometer noise
  void applyComplimentaryFilter(ImuDataScaled& as_Data, const float af_PitchAcc, const float af_RollAcc);

protected:
  // Writes data to mpu6050 sensor
  bool writeReg(uint8_t au8_Reg, uint8_t au8_Val);

private:
  I2C_HandleTypeDef& mps_HandleI2C; // i2c handle for communicating with sensor

  static constexpr uint8_t mu8_Addr7 = 0x68U; // base address
  static constexpr uint8_t mu8_Addr = mu8_Addr7 << 1U;  // hal needs shifted address
};

#endif // IMU_SENSOR_HPP
