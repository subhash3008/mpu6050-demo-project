/**
 * @file LogMessage.hpp
 * @author Subhash Chandra
 * @brief Header file for structuring the message for logging
 * @version 0.1
 * @date 2026-04-26
 * 
 * @copyright Copyright (c) 2026
 * 
 */
#ifndef LOG_MESSAGE_HPP
#define LOG_MESSAGE_HPP

#include <cstdint>

/**
 * @brief Scoped enum defining four severity levels: Debug, Info, Warn, and Error.
 */
enum class LogLevel: uint8_t
{
  Debug,
  Info,
  Warn,
  Error
};

/**
 * @brief stores a timestamp tick count, a log severity level, and a
 *        fixed-size 96-byte text buffer for the message content.
 */
struct LogMessage
{
  uint32_t u32_Tick;  // timestamp
  LogLevel e_Level;   // severity level
  char c_Text[96];    // message content
};


#endif // LOG_MESSAGE_HPP
