#ifndef LED_HPP
#define LED_HPP

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

#include "stm32f4xx_hal.h"

#ifdef __cplusplus
}
#endif

class Led
{
public:
  Led(GPIO_TypeDef* aps_Port, uint16_t au16_Pin);

  void on();
  void off();
  void toggle();

private:
  GPIO_TypeDef* mps_Port;
  uint16_t mu16_Pin;
};

#endif // LED_HPP
