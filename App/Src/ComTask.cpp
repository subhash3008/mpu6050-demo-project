/**
 * @file ComTask.cpp
 * @author Subhash Chandra
 * @brief Implements command communication task member and associated functions
 * @version 0.1
 * @date 2026-07-04
 * 
 * @copyright Copyright (c) 2026
 * 
 */

/***************************************************
* INCLUDES
***************************************************/
#include "ComTask.hpp"
#include <cstring>
#include <cstdio>
#include <stdint.h>
#include "WatchdogManager.hpp"

#ifdef __cplusplus
extern "C"
{
#endif

#include "main.h"
#include "stm32f4xx_hal_uart.h"

/***************************************************
* External Variables
***************************************************/
extern UART_HandleTypeDef huart2;

#ifdef __cplusplus
}
#endif

/***************************************************
* Private Variables
***************************************************/
static ComTask* gps_ComTaskRef = nullptr;

static uint8_t gu8_RxByte = 0u;

/***************************************************
* Local Helper Functions
***************************************************/

/***************************************************
* Member Functions
***************************************************/

/**
 * @brief Constructor for initializing the member message queue
 *        with provided message queue reference (from application)
 * 
 * @param aps_Queue Message Queue for processing the incoming commands
 * @return ComTask:: 
 */
ComTask::
ComTask(ComProtocol& aps_Com, LoggerDriver& aps_Logger)
: ms_Com(aps_Com),
  ms_Logger(aps_Logger)
{
  gps_ComTaskRef = this; // Hold reference of ComTask for UART Rx Callback
}

/**
 * @brief Task entry function to process the commands from the member 
 *        message queue
 */
void ComTask::
run()
{
  const TickType_t lu8_TaskDelay = 5u; // in ms
  HAL_UART_Receive_IT(&huart2, &gu8_RxByte, 1); // Await reception of the byte

  WatchdogManager::registerTask(WatchdogManager::TaskId::COM_TASK);

  while (1)
  {
    ms_Com.process();

    if (ms_Com.getRespReady())
    {
      ms_Logger.info(ms_Com.getResponse());
      ms_Com.resetResp();
    }

    WatchdogManager::alive(WatchdogManager::TaskId::COM_TASK);
    vTaskDelay(pdMS_TO_TICKS(lu8_TaskDelay));
  }
}

void ComTask::
handleRxByte()
{
  ms_Com.pushRxByte(gu8_RxByte);
}

// UART CALLBACK
extern "C" void
HAL_UART_RxCpltCallback(UART_HandleTypeDef* huart)
{
  if (USART2 == huart->Instance)
  {
    if (gps_ComTaskRef) // check if reference to ComTask exists
    {
      gps_ComTaskRef->handleRxByte(); // push the incoming byte to buffer
    }
    HAL_UART_Receive_IT(&huart2, &gu8_RxByte, 1); // Await reception of the byte
  }
}
