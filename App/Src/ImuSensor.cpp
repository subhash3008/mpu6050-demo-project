/**
 * @file ImuSensor.cpp
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
#include "ImuSensor.hpp"

/***************************************************
* Private Variables
***************************************************/
static constexpr uint8_t REG_WHO_AM_I = 0x75;
static constexpr uint8_t REG_PWR_MGMT_1 = 0x6B;
static constexpr uint8_t REG_DATA = 0x3B;

static constexpr uint8_t gu8_DataLen = 14U;

/***************************************************
* Member Functions
***************************************************/
/**
 * @brief Constructor for ImuSensor that initializes the object's
 *        mps_HandleI2C member with the provided I2C handle reference.
 */
ImuSensor::
ImuSensor(I2C_HandleTypeDef& aps_HandleI2C)
: mps_HandleI2C(aps_HandleI2C)
{}

/**
 * @brief Initializes the sensor
 * 
 * @return true   if the sensor is correctly initialized
 * @return false  if any error occured during initialization
 */
bool ImuSensor::
init()
{
  bool lb_RetVal = false;
  // Wakeup sensor from sleep
  lb_RetVal = writeReg(REG_PWR_MGMT_1, 0x00);
  
  return lb_RetVal;
}

/**
 * @brief Reads Who Am I register from the MPU6050 sensor using HAL api
 * 
 * @param au8_Id  Reference for updating the read value
 * @return true   if read is successful
 * @return false  if read is not successful
 */
bool ImuSensor::
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
bool ImuSensor::
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
bool ImuSensor::
readReg(ImuData& aps_Data)
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
  aps_Data.i16_AX = to16(lu8_RawData[0], lu8_RawData[1]);
  aps_Data.i16_AY = to16(lu8_RawData[2], lu8_RawData[3]);
  aps_Data.i16_AZ = to16(lu8_RawData[4], lu8_RawData[5]);

  aps_Data.i16_GX = to16(lu8_RawData[8], lu8_RawData[9]);
  aps_Data.i16_GY = to16(lu8_RawData[10], lu8_RawData[11]);
  aps_Data.i16_GZ = to16(lu8_RawData[12], lu8_RawData[13]);

  return true;
}
