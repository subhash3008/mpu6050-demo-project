#include "ImuSensor.hpp"

static constexpr uint8_t REG_WHO_AM_I = 0x75;
static constexpr uint8_t REG_PWR_MGMT_1 = 0x6B;

ImuSensor::
ImuSensor(I2C_HandleTypeDef& aps_HandleI2C)
: mps_HandleI2C(aps_HandleI2C)
{}

bool ImuSensor::
init()
{
  bool lb_RetVal = false;
  // Wakeup sensor from sleep
  lb_RetVal = writeReg(REG_PWR_MGMT_1, 0x00);
  
  return lb_RetVal;
}

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

bool ImuSensor::
writeReg(uint8_t au8_Reg, uint8_t au8_Val)
{
  return HAL_I2C_Mem_Write(
    &mps_HandleI2C,
    mu8_Addr,
    au8_Reg,
    I2C_MEMADD_SIZE_8BIT,
    &au8_Val,
    1,
    100
  ) == HAL_OK;
}

bool ImuSensor::
readReg(ImuData& aps_Data)
{
  uint8_t lu8_RawData[14];
  HAL_StatusTypeDef lu8_HalRetVal = HAL_I2C_Mem_Read(
    &mps_HandleI2C,
    mu8_Addr,
    0x3B,
    I2C_MEMADD_SIZE_8BIT,
    lu8_RawData,
    14,
    100
  );
  
  if (HAL_OK != lu8_HalRetVal)
  {
    return false;
  }

  auto to16 = [](uint8_t au8_Msb, uint8_t au8_Lsb)
  {
    return static_cast<int16_t>((au8_Msb << 8) | au8_Lsb);
  };

  aps_Data.i16_AX = to16(lu8_RawData[0], lu8_RawData[1]);
  aps_Data.i16_AY = to16(lu8_RawData[2], lu8_RawData[3]);
  aps_Data.i16_AZ = to16(lu8_RawData[4], lu8_RawData[5]);

  aps_Data.i16_GX = to16(lu8_RawData[8], lu8_RawData[9]);
  aps_Data.i16_GY = to16(lu8_RawData[10], lu8_RawData[11]);
  aps_Data.i16_GZ = to16(lu8_RawData[12], lu8_RawData[13]);

  return true;
}
