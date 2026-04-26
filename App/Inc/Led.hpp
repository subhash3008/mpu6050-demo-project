/**
 * @file Led.hpp
 * @author Subhash Chandra
 * @brief Header for Led driver
 * @version 0.1
 * @date 2026-04-26
 * 
 * @copyright Copyright (c) 2026
 * 
 */
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

/**
 * @brief Represents an LED connected to a GPIO port and pin,
 *        providing methods to turn it on, turn it off, and toggle its state.
 * 
 */
class Led
{
public:
  // Constructor that initializes an LED object with a pointer to a hardware port
  // and a 16-bit pin identifier. It stores the provided port reference and pin
  // number for later LED control.
  Led(GPIO_TypeDef* aps_Port, uint16_t au16_Pin);

  // Function to turn on the led
  void on();
  // Function to turn off the led
  void off();
  // Function to toggle the led
  void toggle();

private:
  GPIO_TypeDef* mps_Port; // GPIO Port for User LED
  uint16_t mu16_Pin;  // Pin for User LED
};

#endif // LED_HPP
