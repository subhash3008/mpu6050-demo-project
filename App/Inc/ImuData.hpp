#ifndef IMUDATA_HPP
#define IMUDATA_HPP

#include <stdint.h>

struct ImuData
{
  int16_t i16_AX;
  int16_t i16_AY;
  int16_t i16_AZ;

  int16_t i16_GX;
  int16_t i16_GY;
  int16_t i16_GZ;
};

#endif // IMUDATA_HPP
