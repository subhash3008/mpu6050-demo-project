#include "LoggerTask.hpp"

#ifdef __cplusplus
extern "C"
{
#endif

#include "main.h"
#include "stm32f4xx_hal_uart.h"

extern UART_HandleTypeDef huart2;

#ifdef __cplusplus
}
#endif

#include <cstring>
#include <cstdio>
#include <stdint.h>

static LoggerTask* gps_Self = nullptr;

const uint16_t g_LoggerTaskDelay = 60'000u;

static const char* levelTag(const LogLevel au8_Level)
{ 
  switch(au8_Level)
  {
    case LogLevel::Info:  return "INF";
    case LogLevel::Warn:  return "WRN";
    case LogLevel::Debug: return "DBG";
    default:              return "ERR";
  }
}

LoggerTask::
LoggerTask(Queue<LogMessage, 16>& aps_Queue)
: ms_Queue(aps_Queue)
{
  gps_Self = this;
}

void LoggerTask::
run()
{

  LogMessage ls_Msg;

  while (1)
  {
    if (ms_Queue.receive(ls_Msg))
    {
      while (mb_Busy)
      {
        vTaskDelay(1);
      }

      int li_MsgSize = snprintf(
        mc_TxBuf,
        sizeof(mc_TxBuf),
        "[%lu][%s] %s\r\n",
        ls_Msg.u32_Tick,
        levelTag(ls_Msg.e_Level),
        ls_Msg.c_Text
      );
      
      mb_Busy = true;

      HAL_UART_Transmit_IT(
        &huart2,
        (uint8_t *)mc_TxBuf,
        li_MsgSize
      );
    }
  }
}

void LoggerTask::
onTxComplete()
{
  mb_Busy = false;
}

// UART CALLBACK
extern "C" void
HAL_UART_TxCpltCallback(UART_HandleTypeDef* huart)
{
  if (USART2 == huart->Instance)
  {
    if (gps_Self)
    {
      gps_Self->onTxComplete();
    }
  }
}
