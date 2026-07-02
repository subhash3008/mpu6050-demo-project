/**
 * @file ImuSensorDriver.cpp
 * @author Subhash Chandra
 * @brief Implements driver class member and associated functions
 * @version 0.1
 * @date 2026-04-26
 * 
 * @copyright Copyright (c) 2026
 * 
 */

/***************************************************
* INCLUDES
***************************************************/
#include "ImuSensorDriver.hpp"

/***************************************************
* Constants
***************************************************/

// accelerometer sensitivity (for +-2g) from MPU6050 datasheet: 16384 = 1g
static constexpr float ACC_SCALE_2G = 16384.0f;

// gyrometer sensitivity (for +-250 rad/s) from the datasheet: 131
static constexpr float GYRO_SCALE_250 = 131.0f;

// Registers
static constexpr uint8_t REG_WHO_AM_I = 0x75;
static constexpr uint8_t REG_PWR_MGMT_1 = 0x6B;
static constexpr uint8_t REG_GYRO_CONFIG = 0x1B;
static constexpr uint8_t REG_ACC_CONFIG = 0x1C;
static constexpr uint8_t REG_DATA = 0x3B;

// Length of incoming data from sensor
static constexpr uint8_t gu8_DataLen = 14U;

/***************************************************
* Member Functions
***************************************************/
/**
 * @brief Constructor for ImuSensorDriver that initializes the object's
 *        mps_HandleI2C member with the provided I2C handle reference.
 */
ImuSensorDriver::
ImuSensorDriver(I2C_HandleTypeDef& aps_HandleI2C)
: mps_HandleI2C(aps_HandleI2C)
{}

/**
 * @brief Initializes the sensor
 * 
 * @return true   if the sensor is correctly initialized
 * @return false  if any error occured during initialization
 */
bool ImuSensorDriver::
init()
{
  bool lb_RetVal = false;
  // Wakeup sensor from sleep
  lb_RetVal = writeReg(REG_PWR_MGMT_1, 0x00);
  // configure gyroscope
  lb_RetVal = writeReg(REG_GYRO_CONFIG, 0x00);
  // configure accelerometer
  lb_RetVal = writeReg(REG_ACC_CONFIG, 0x00);
  
  return lb_RetVal;
}

/**
 * @brief Reads Who Am I register from the MPU6050 sensor using HAL api
 * 
 * @param au8_Id  Reference for updating the read value
 * @return true   if read is successful
 * @return false  if read is not successful
 */
bool ImuSensorDriver::
whoAmI(uint8_t& au8_Id)
{
  HAL_StatusTypeDef lu8_RetVal = HAL_I2C_Mem_Read(
    &mps_HandleI2C,
    mu8_Addr,
    REG_WHO_AM_I,
    I2C_MEMADD_SIZE_8BIT,
    &au8_Id,
    1,
    100
  );

  return HAL_OK == lu8_RetVal;
}

/**
 * @brief Writes one byte to MPU6050 using polling HAL api 
 * 
 * @param au8_Reg Register to be written
 * @param au8_Val Value to be written
 * @return true   if write operation successful
 * @return false  if write operation is not successful
 */
bool ImuSensorDriver::
writeReg(uint8_t au8_Reg, uint8_t au8_Val)
{
  HAL_StatusTypeDef lu8_RetVal = HAL_I2C_Mem_Write(
    &mps_HandleI2C,
    mu8_Addr,
    au8_Reg,
    I2C_MEMADD_SIZE_8BIT,
    &au8_Val,
    1,
    100
  );
  
  return HAL_OK == lu8_RetVal;
}

/**
 * @brief Read sensor data values from MPU6050
 * 
 * @param aps_Data  Reference for updating incoming data
 * @return true     if read operation is successful
 * @return false    if read operation is not successful
 */
bool ImuSensorDriver::
readSensorData(ImuDataRaw& aps_Data)
{
  uint8_t lu8_RawData[gu8_DataLen]; // 6 bytes for accelerometer + 2 bytes for temperature + 6 bytes for gyrometer

  // Accelerometer -> 0x3B to 0x40
  // Temperature -> 0x41 to 0x42
  // Gyrometer -> 0x43 to 0x48
  HAL_StatusTypeDef lu8_HalRetVal = HAL_I2C_Mem_Read(
    &mps_HandleI2C,
    mu8_Addr,
    REG_DATA,
    I2C_MEMADD_SIZE_8BIT,
    lu8_RawData,
    gu8_DataLen,
    100
  );
  
  if (HAL_OK != lu8_HalRetVal)
  {
    return false; // return if read fails
  }

  // Lambda function to combine two uint8 bytes to int16
  auto to16 = [](uint8_t au8_Msb, uint8_t au8_Lsb)
  {
    return static_cast<int16_t>((au8_Msb << 8) | au8_Lsb);
  };

  // Format the raw bytes to ImuData structure
  aps_Data.i16_Ax = to16(lu8_RawData[0], lu8_RawData[1]);
  aps_Data.i16_Ay = to16(lu8_RawData[2], lu8_RawData[3]);
  aps_Data.i16_Ay = to16(lu8_RawData[4], lu8_RawData[5]);

  aps_Data.i16_Gx = to16(lu8_RawData[8], lu8_RawData[9]);
  aps_Data.i16_Gy = to16(lu8_RawData[10], lu8_RawData[11]);
  aps_Data.i16_Gz = to16(lu8_RawData[12], lu8_RawData[13]);

  return true;
}


/**
 * @brief Processes the raw data from the MPU6050 sensor for accelerometer
 *        and gyrometer. For accelerometer, changes the data to g units and
 *        for gyrometer, changes the data to deg/sec
 * 
 * @param aps_DataRaw     Raw data for processing
 * @param aps_DataScaled  Output for processed data
 */
void ImuSensorDriver::
convertRawValuesToScaledValues(const ImuDataRaw& aps_DataRaw, ImuDataScaled& aps_DataScaled)
{
  // accelerometer data
  aps_DataScaled.f_AxG = static_cast<float>(aps_DataRaw.i16_Ax) / ACC_SCALE_2G;
  aps_DataScaled.f_AyG = static_cast<float>(aps_DataRaw.i16_Ay) / ACC_SCALE_2G;
  aps_DataScaled.f_AzG = static_cast<float>(aps_DataRaw.i16_Az) / ACC_SCALE_2G;

  // gyrometer data
  aps_DataScaled.f_GxDps = static_cast<float>(aps_DataRaw.i16_Gx) / GYRO_SCALE_250;
  aps_DataScaled.f_GyDps = static_cast<float>(aps_DataRaw.i16_Gy) / GYRO_SCALE_250;
  aps_DataScaled.f_GzDps = static_cast<float>(aps_DataRaw.i16_Gz) / GYRO_SCALE_250;
}

/**
 * @brief This function is called once on the startup for calibration of the 
 *        gyrometer data. Data is aggregated over a provided sample size and
 *        then averaged and converted into deg/sec unit
 *
 * @param au16_SampleCount  Count for data aggregation from the sensor
 * @return ImuDataGyroBias  calculated bias
 */
ImuDataGyroBias ImuSensorDriver::
calibrateGyro(const uint16_t au16_SampleCount)
{
  ImuDataRaw ls_Data;
  ImuDataGyroBias ls_Bias;
  int32_t li32_GxAggregate = 0u;
  int32_t li32_GyAggregate = 0u;
  int32_t li32_GzAggregate = 0u;
  uint16_t counter; // Used in bias calculation, in case if the read is not successfull 100% of the time

  // Aggregate gyro data
  for (counter = 0; counter < au16_SampleCount; ++counter)
  {
    if (readSensorData(ls_Data))
    {
      li32_GxAggregate += ls_Data.i16_Gx;
      li32_GyAggregate += ls_Data.i16_Gy;
      li32_GzAggregate += ls_Data.i16_Gz;
    }
    HAL_Delay(10);
  }

  // average and scale the data for bias calculation
  ls_Bias.f_GxBias = (static_cast<float>(li32_GxAggregate) / static_cast<float>(counter)) / GYRO_SCALE_250;
  ls_Bias.f_GyBias = (static_cast<float>(li32_GyAggregate) / static_cast<float>(counter)) / GYRO_SCALE_250;
  ls_Bias.f_GzBias = (static_cast<float>(li32_GzAggregate) / static_cast<float>(counter)) / GYRO_SCALE_250;

  return ls_Bias;

}
