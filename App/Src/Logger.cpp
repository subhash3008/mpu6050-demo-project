#include "Logger.hpp"
#include <cstdio>
#include <string.h>

Logger::
Logger(Queue<LogMessage, 16>& aps_Queue)
: ms_Queue(aps_Queue)
{}

void Logger::
send(LogLevel ae_Level,
      const char* apc_Fmt,
      va_list args)
{
  LogMessage ls_Msg;
  ls_Msg.u32_Tick = xTaskGetTickCount();
  ls_Msg.e_Level = ae_Level;

  vsnprintf(ls_Msg.c_Text, sizeof(ls_Msg.c_Text), apc_Fmt, args);

  ms_Queue.send(ls_Msg);
}

void Logger::
info(const char* apc_Fmt, ...)
{
  va_list ls_Args;
  va_start(ls_Args, apc_Fmt);
  send(LogLevel::Info, apc_Fmt, ls_Args);
  va_end(ls_Args);
}

void Logger::
warn(const char* apc_Fmt, ...)
{
  va_list ls_Args;
  va_start(ls_Args, apc_Fmt);
  send(LogLevel::Warn, apc_Fmt, ls_Args);
  va_end(ls_Args);
}

void Logger::
error(const char* apc_Fmt, ...)
{
  va_list ls_Args;
  va_start(ls_Args, apc_Fmt);
  send(LogLevel::Error, apc_Fmt, ls_Args);
  va_end(ls_Args);
}

void Logger::
debug(const char* apc_Fmt, ...)
{
  va_list ls_Args;
  va_start(ls_Args, apc_Fmt);
  send(LogLevel::Debug, apc_Fmt, ls_Args);
  va_end(ls_Args);
}

void Logger::
infoFromISR(const char* apc_Text)
{
  LogMessage ls_Msg;
  ls_Msg.u32_Tick = xTaskGetTickCountFromISR();
  ls_Msg.e_Level = LogLevel::Info;

  strncpy(ls_Msg.c_Text, apc_Text, sizeof(ls_Msg.c_Text) - 1);

  BaseType_t lu32_Hpw = pdFALSE;
  xQueueSendFromISR(ms_Queue.native(), &ls_Msg, &lu32_Hpw);
  portYIELD_FROM_ISR(lu32_Hpw);
}
