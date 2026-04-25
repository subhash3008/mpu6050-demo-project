#ifndef LOGGER_HPP
#define LOGGER_HPP

#include "Queue.hpp"
#include "LogMessage.hpp"

#include <cstdarg>

class Logger
{
public:
  explicit Logger(Queue<LogMessage, 16>& aps_Queue);

  void info(const char* apc_Fmt, ...);
  void warn(const char* apc_Fmt, ...);
  void error(const char* apc_Fmt, ...);
  void debug(const char* apc_Fmt, ...);

  void infoFromISR(const char* apc_Text);

private:
  void send(LogLevel ae_Level,
            const char* apc_Fmt,
            va_list args);

  Queue<LogMessage, 16>& ms_Queue;

};

#endif // LOGGER_HPP
