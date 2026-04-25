#ifndef IMU_SENSOR_HPP
#define IMU_SENSOR_HPP

// Sensor used in the project is MPU6050

#include "ImuData.hpp"

#ifdef __cplusplus
extern "C"
{
#endif

#include "stm32f4xx_hal.h"

#ifdef __cplusplus
}
#endif

class ImuSensor
{
public:
  explicit ImuSensor(I2C_HandleTypeDef& aps_HandleI2C);

  bool init();
  bool whoAmI(uint8_t& au8_Id);
  bool readReg(ImuData& aps_Data);

protected:
  bool writeReg(uint8_t au8_Reg, uint8_t au8_Val);

private:
  I2C_HandleTypeDef& mps_HandleI2C;

  static constexpr uint8_t mu8_Addr7 = 0x68U;
  static constexpr uint8_t mu8_Addr = mu8_Addr7 << 1U;
};

#endif // IMU_SENSOR_HPP
