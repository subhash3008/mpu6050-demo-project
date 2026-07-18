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
#include <cmath>

/***************************************************
* Constants
***************************************************/

// used for conversion between rad and degree for angles
static constexpr float RAD_TO_DEG = 57.2957f;

// Tuning weight used for complimentary filter
static constexpr float ALPHA = 0.96f;

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
  aps_Data.i16_Az = to16(lu8_RawData[4], lu8_RawData[5]);

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
calibrateGyro(const uint16_t au16_SampleCount, const uint8_t au8_TimeInterval)
{

  HAL_Delay(1000u); // Allow for sensor stabilization

  ImuDataRaw ls_Data;
  ImuDataGyroBias ls_Bias;
  int32_t li32_GxAggregate = 0u;
  int32_t li32_GyAggregate = 0u;
  int32_t li32_GzAggregate = 0u;
  int32_t li32_AxAggregate = 0u;
  int32_t li32_AyAggregate = 0u;
  int32_t li32_AzAggregate = 0u;
  uint16_t counter; // Used in bias calculation, in case if the read is not successfull 100% of the time

  // Aggregate gyro data
  for (counter = 0; counter < au16_SampleCount; ++counter)
  {
    if (readSensorData(ls_Data))
    {
      li32_GxAggregate += ls_Data.i16_Gx;
      li32_GyAggregate += ls_Data.i16_Gy;
      li32_GzAggregate += ls_Data.i16_Gz;

      li32_AxAggregate += ls_Data.i16_Ax;
      li32_AyAggregate += ls_Data.i16_Ay;
      li32_AzAggregate += (ls_Data.i16_Az - (int16_t)ACC_SCALE_2G);

    }
    HAL_Delay(au8_TimeInterval);
  }

  // average and scale the data for bias calculation
  ls_Bias.f_GxBias = (static_cast<float>(li32_GxAggregate) / static_cast<float>(counter)) / GYRO_SCALE_250;
  ls_Bias.f_GyBias = (static_cast<float>(li32_GyAggregate) / static_cast<float>(counter)) / GYRO_SCALE_250;
  ls_Bias.f_GzBias = (static_cast<float>(li32_GzAggregate) / static_cast<float>(counter)) / GYRO_SCALE_250;

  ls_Bias.f_AxBias = (static_cast<float>(li32_AxAggregate) / static_cast<float>(counter)) / ACC_SCALE_2G;
  ls_Bias.f_AyBias = (static_cast<float>(li32_AyAggregate) / static_cast<float>(counter)) / ACC_SCALE_2G;
  ls_Bias.f_AzBias = (static_cast<float>(li32_AzAggregate) / static_cast<float>(counter)) / ACC_SCALE_2G;

  return ls_Bias;

}


/**
 * @brief Removes the bias from the gyrometer measuements
 * 
 * @param as_Data       Measurement data
 * @param as_GyroBias   bias from calibration to be removed
 */
void ImuSensorDriver::
removeGyroBias(ImuDataScaled& as_Data, const ImuDataGyroBias as_GyroBias)
{
  as_Data.f_GxDps -= as_GyroBias.f_GxBias;
  as_Data.f_GyDps -= as_GyroBias.f_GyBias;
  as_Data.f_GzDps -= as_GyroBias.f_GzBias;

  as_Data.f_AxG -= as_GyroBias.f_AxBias;
  as_Data.f_AyG -= as_GyroBias.f_AyBias;
  as_Data.f_AzG -= as_GyroBias.f_AzBias;
}

/**
 * @brief Integrate gyro rates to get gyro angles
 *        Angle = Angle + Angle Change
 *        Angle Change = Rate * Time
 * 
 * @param as_Data Measurement Data
 * @param af_Dt   Time difference for task run
 */
void ImuSensorDriver::
integrateGyro(ImuDataScaled& as_Data, const float af_Dt)
{
  // Assumption: gx rotates pitch axis, gy rotates roll axis
  as_Data.f_PitchDeg +=  as_Data.f_GxDps * af_Dt;
  as_Data.f_RollDeg +=  as_Data.f_GyDps * af_Dt;
}

/**
 * @brief Calculate pitch from accelerometer data
 * @details Pitch is calculated as below
 *          pitch = atan2(-X, sqrt(Y*Y + Z*Z))
 * 
 *          Magnitude of YZ plane i.e. sqrt(Y*Y + Z*Z) improves
 *          stability when board moves
 * 
 * @param as_Data Measurement data 
 * @return float  Calculated pitch value
 */
float ImuSensorDriver::
calculatePitchFromAccel(const ImuDataScaled& as_Data)
{
  float lf_Denominator = std::sqrt((as_Data.f_AyG * as_Data.f_AyG) + (as_Data.f_AzG * as_Data.f_AzG));
  float lf_AngleRad = std::atan2(-as_Data.f_AxG, lf_Denominator);

  return lf_AngleRad * RAD_TO_DEG; // convert angle to degrees
}

/**
 * @brief Calculate roll from accelerometer data
 *        Roll = atan2(Y, Z)
 * 
 * @param as_Data 
 * @return float 
 */
float ImuSensorDriver::
calculateRollFromAccel(const ImuDataScaled& as_Data)
{
  float lf_AngleRad = std::atan2(as_Data.f_AyG, as_Data.f_AzG);
  
  return lf_AngleRad * RAD_TO_DEG; // convert angle to degrees
}


/**
 * @brief Apply fiter for compensating gyro drift and accelerometer noise, refer readme file
 *        Fused Data = Alpha * Accumulated Data + (1 - Alpha) * Current Data
 * 
 * @param as_Data 
 * @param af_PitchAcc 
 * @param af_RollAcc 
 */
void ImuSensorDriver::
applyComplimentaryFilter(ImuDataScaled& as_Data, const float af_PitchAcc, const float af_RollAcc)
{
  as_Data.f_PitchDeg = ALPHA * as_Data.f_PitchDeg + (1.0f - ALPHA) * af_PitchAcc;
  as_Data.f_RollDeg = ALPHA * as_Data.f_RollDeg + (1.0f - ALPHA) * af_RollAcc;

}
