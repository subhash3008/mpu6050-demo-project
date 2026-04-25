#ifndef LOG_MESSAGE_HPP
#define LOG_MESSAGE_HPP

#include <cstdint>

enum class LogLevel: uint8_t
{
  Debug,
  Info,
  Warn,
  Error
};

struct LogMessage
{
  uint32_t u32_Tick;
  LogLevel e_Level;
  char c_Text[96];
};


#endif // LOG_MESSAGE_HPP
