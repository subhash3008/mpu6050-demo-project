#include "Led.hpp"

Led::
Led(GPIO_TypeDef* aps_Port, uint16_t au16_Pin)
: mps_Port(aps_Port), mu16_Pin(au16_Pin)
{}

void Led::
on()
{
  HAL_GPIO_WritePin(mps_Port, mu16_Pin, GPIO_PIN_SET);
}

void Led::
off()
{
  HAL_GPIO_WritePin(mps_Port, mu16_Pin, GPIO_PIN_RESET);
}

void Led::
toggle()
{
  HAL_GPIO_TogglePin(mps_Port, mu16_Pin);
}
