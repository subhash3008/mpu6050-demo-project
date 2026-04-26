/**
 * @file Logger.cpp
 * @author Subhash Chandra
 * @brief Implements Logger utility member and associated functions
 * @version 0.1
 * @date 2026-04-26
 * 
 * @copyright Copyright (c) 2026
 * 
 */
#include "Logger.hpp"
#include <cstdio>
#include <string.h>

/***************************************************
* MEMBER FUNCTIONS
***************************************************/
/**
 * @brief Constructor that initializes a Logger instance by storing reference
 *        to the provided LogMessage Queue in the member queue variaböe.
 */
Logger::
Logger(Queue<LogMessage, 16>& aps_Queue)
: ms_Queue(aps_Queue)
{}

/**
 * @brief Formats the incoming message and enqueues it via member Queue variable.
 * 
 * @param ae_Level  Severity Level
 * @param apc_Fmt   Format from variadic function
 * @param args      Arguments for formatting
 */
void Logger::
send(LogLevel ae_Level,
      const char* apc_Fmt,
      va_list args)
{
  LogMessage ls_Msg;
  ls_Msg.u32_Tick = xTaskGetTickCount();  // Timestamp
  ls_Msg.e_Level = ae_Level;

  // Prepares the message
  vsnprintf(ls_Msg.c_Text, sizeof(ls_Msg.c_Text), apc_Fmt, args);

  ms_Queue.send(ls_Msg); // send the message to queue
}

/**
 * @brief Logger member function for processing info severity level
 * 
 * @param apc_Fmt printf-style format string
 * @param ...     variadic arguments
 */
void Logger::
info(const char* apc_Fmt, ...)
{
  va_list ls_Args;
  va_start(ls_Args, apc_Fmt);
  send(LogLevel::Info, apc_Fmt, ls_Args);
  va_end(ls_Args);
}

/**
 * @brief Logger member function for processing warning severity level
 * 
 * @param apc_Fmt printf-style format string
 * @param ...     variadic arguments
 */
void Logger::
warn(const char* apc_Fmt, ...)
{
  va_list ls_Args;
  va_start(ls_Args, apc_Fmt);
  send(LogLevel::Warn, apc_Fmt, ls_Args);
  va_end(ls_Args);
}

/**
 * @brief Logger member function for processing error severity level
 * 
 * @param apc_Fmt printf-style format string
 * @param ...     variadic arguments
 */
void Logger::
error(const char* apc_Fmt, ...)
{
  va_list ls_Args;
  va_start(ls_Args, apc_Fmt);
  send(LogLevel::Error, apc_Fmt, ls_Args);
  va_end(ls_Args);
}

/**
 * @brief Logger member function for processing debug severity level
 * 
 * @param apc_Fmt printf-style format string
 * @param ...     variadic arguments
 */
void Logger::
debug(const char* apc_Fmt, ...)
{
  va_list ls_Args;
  va_start(ls_Args, apc_Fmt);
  send(LogLevel::Debug, apc_Fmt, ls_Args);
  va_end(ls_Args);
}

/**
 * @brief Enqueues an info-level log message from an ISR context
 * 
 * @param apc_Text  Message for logging
 */
void Logger::
infoFromISR(const char* apc_Text)
{
  LogMessage ls_Msg;
  ls_Msg.u32_Tick = xTaskGetTickCountFromISR(); // Timestamp
  ls_Msg.e_Level = LogLevel::Info;  // Always info severity level

  // Format the message
  strncpy(ls_Msg.c_Text, apc_Text, sizeof(ls_Msg.c_Text) - 1);

  BaseType_t lu32_Hpw = pdFALSE; // hpw = High Priority Woken
  xQueueSendFromISR(ms_Queue.native(), &ls_Msg, &lu32_Hpw);
  portYIELD_FROM_ISR(lu32_Hpw); // triggers context switch, if needed
}
