/**
 * @file Led.cpp
 * @author Subhash Chandra
 * @brief Implements Led driver class member and associated functions
 * @version 0.1
 * @date 2026-04-26
 * 
 * @copyright Copyright (c) 2026
 * 
 */

/***************************************************
* INCLUDES
***************************************************/
#include "Led.hpp"

/***************************************************
* Member Functions
***************************************************/

/**
 * @brief  constructs an Led object
 * @param aps_Port  GPIO Port for LED
 * @param au16_Pin  Pin for LED
 */
Led::
Led(GPIO_TypeDef* aps_Port, uint16_t au16_Pin)
: mps_Port(aps_Port), mu16_Pin(au16_Pin)
{}

/**
 * @brief Sets the LED’s GPIO pin, turning the associated LED on.
 */
void Led::
on()
{
  HAL_GPIO_WritePin(mps_Port, mu16_Pin, GPIO_PIN_SET);
}

/**
 * @brief Resets the LED’s GPIO pin, turning the associated LED off.
 */
void Led::
off()
{
  HAL_GPIO_WritePin(mps_Port, mu16_Pin, GPIO_PIN_RESET);
}

/**
 * @brief Toggles the LED’s GPIO pin, turning the associated LED off if on, on if off.
 */
void Led::
toggle()
{
  HAL_GPIO_TogglePin(mps_Port, mu16_Pin);
}
