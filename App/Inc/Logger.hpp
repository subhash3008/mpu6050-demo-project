/**
 * @file Logger.hpp
 * @author Subhash Chandra
 * @brief Header file for logger driver
 * @version 0.1
 * @date 2026-04-26
 * 
 * @copyright Copyright (c) 2026
 * 
 */
#ifndef LOGGER_HPP
#define LOGGER_HPP

#include "Queue.hpp"
#include "LogMessage.hpp"

#include <cstdarg>

/**
 * @brief Logging utility that sends formatted log messages at various severity
 *        levels (info, warn, error, debug) into a Queue<LogMessage, 16> provided
 *        at construction, with a separate infoFromISR method for ISR-safe logging
 *        and a private send helper that formats the payload.
 */
class Logger
{
public:
  // Constructor that explicitly initializes a Logger instance with a reference to
  // a LogMessage Queue.
  explicit Logger(Queue<LogMessage, 16>& aps_Queue);

  // Function to process info messages
  void info(const char* apc_Fmt, ...);
  // Function to process warning messages
  void warn(const char* apc_Fmt, ...);
  // Function to process error messages
  void error(const char* apc_Fmt, ...);
  // Function to process debug messages
  void debug(const char* apc_Fmt, ...);

  // Function to process messages from ISR
  void infoFromISR(const char* apc_Text);

private:
  // formats and sends a log entry to the queue
  void send(LogLevel ae_Level,
            const char* apc_Fmt,
            va_list args);

  Queue<LogMessage, 16>& ms_Queue; // Queue for storing messages

};

#endif // LOGGER_HPP
