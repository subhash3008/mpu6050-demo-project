/**
 * @file LoggerTask.cpp
 * @author Subhash Chandra
 * @brief Implements logger task member and associated functions
 * @version 0.1
 * @date 2026-04-26
 * 
 * @copyright Copyright (c) 2026
 * 
 */

/***************************************************
* INCLUDES
***************************************************/
#include "LoggerTask.hpp"
#include <cstring>
#include <cstdio>
#include <stdint.h>

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
static LoggerTask* gps_LoggerTaskRef = nullptr;

const uint16_t g_LoggerTaskDelay = 60'000u;

/***************************************************
* Local Helper Functions
***************************************************/

/**
 * @brief Converts Message severity level from enum to text
 * 
 * @returns const char*   Text for the severity level
 */
static const char* 
levelTag(const LogLevel au8_Level)
{ 
  switch(au8_Level)
  {
    case LogLevel::Info:  return "INF";
    case LogLevel::Warn:  return "WRN";
    case LogLevel::Debug: return "DBG";
    default:              return "ERR";
  }
}

/***************************************************
* Member Functions
***************************************************/

/**
 * @brief Constructor for initializing the member message queue
 *        with provided message queue reference (from application)
 * 
 * @param aps_Queue Message Queue for processing the messages
 * @return LoggerTask:: 
 */
LoggerTask::
LoggerTask(Queue<LogMessage, 16>& aps_Queue)
: ms_Queue(aps_Queue)
{
  gps_LoggerTaskRef = this; // Hold reference of LoggerTask for UART Tx Callback
}

/**
 * @brief Task entry function to process the messages from the member 
 *        message queue
 */
void LoggerTask::
run()
{

  LogMessage ls_Msg;

  while (1)
  {
    if (ms_Queue.receive(ls_Msg))
    {
      while (mb_Busy) // Keep the task busy if a message is being processed
      {
        vTaskDelay(1);
      }

      // format the message
      int li_MsgSize = snprintf(
        mc_TxBuf,
        sizeof(mc_TxBuf),
        "[%lu][%s] %s\r\n",
        ls_Msg.u32_Tick,
        levelTag(ls_Msg.e_Level),
        ls_Msg.c_Text
      );
      
      mb_Busy = true; // trigger the start of processing of a message

      HAL_UART_Transmit_IT(
        &huart2,
        (uint8_t *)mc_TxBuf,
        li_MsgSize
      );
    }
  }
}

/**
 * @brief Runs on the completion of message transmission
 *        It would be triggered from the UART Tx Callback
 * 
 */
void LoggerTask::
onTxComplete()
{
  mb_Busy = false; // Marks the completion of the message transmission for the task
}

// UART CALLBACK
extern "C" void
HAL_UART_TxCpltCallback(UART_HandleTypeDef* huart)
{
  if (USART2 == huart->Instance)
  {
    if (gps_LoggerTaskRef) // check if reference to LoggerTask exists
    {
      gps_LoggerTaskRef->onTxComplete(); // trigger the completion function in the task
    }
  }
}
