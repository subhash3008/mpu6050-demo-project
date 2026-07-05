/**
 * @file ComProtocol.hpp
 * @author Subhash Chandra
 * @brief Header for Communication protocol for controlling application
 * @version 0.1
 * @date 2026-04-26
 * 
 * @copyright Copyright (c) 2026
 * 
 */

#ifndef COM_PROTOCOL_HPP
#define COM_PROTOCOL_HPP

#include <stdint.h>
#include "Queue.hpp"
#include "LoggerDriver.hpp"

#ifdef __cplusplus
extern "C"
{
#endif

#include "stm32f4xx_hal.h"

#ifdef __cplusplus
}
#endif


class ComProtocol
{
public:
  static constexpr uint8_t HEADER1_VALID_VAL = 0xAAU;
  static constexpr uint8_t HEADER2_VALID_VAL = 0x55U;

  static constexpr uint16_t RX_BUFFER_SIZE = 128U;
  static constexpr uint8_t MAX_PAYLOAD_SIZE = 32U;

  static constexpr uint8_t COMMAND_QUEUE_LENGTH = 8U;

  static constexpr uint8_t MAX_RESPONSE_SIZE = 64U;

  // List of possible incoming commands
  enum class Command: uint8_t
  {
    GET_STATUS          = 0x01U,
    GET_ORIENTATION     = 0x02U,
    START_STREAM        = 0x03U,
    STOP_STREAM         = 0x04U,
    CALIBRATE_SENSORS   = 0x05U,  // Not implemented
    SET_SAMPLE_RATE     = 0x06U,  // Not implemented
    PING                = 0x07U   // Not implemented
  };

  // List of possible respose codes0
  enum class Response: uint8_t
  {
    ACK         = 0x80U,
    STATUS      = 0x81U,
    ORIENTATION = 0x82U,
    ERROR       = 0xFFU
  };

  // Packet for communicating command to other modules
  struct CommandPacket
  {
    Command command;
    uint8_t payload[MAX_PAYLOAD_SIZE];
    uint8_t payloadLength;
  };

  explicit ComProtocol(Queue<CommandPacket, COMMAND_QUEUE_LENGTH>& aps_Queue);

  // Pushed incoming data for processing
  void pushRxByte(uint8_t au8_Byte);

  // Processes the incoming command
  void process();

  bool getNextCommand(CommandPacket& as_Packet);

  // Processes the response for before being sent
  void processResponse(Response ae_Rsp,
                    const uint8_t* apu8_Payload,
                    const uint8_t au8_PayloadLen);

  // Provides the response for sending
  char* getResponse();
  
  bool getRespReady();

  void resetResp();

private:

  // Possible states for ComProtocol
  enum class ComRxState : uint8_t
  {
    STATE_WAIT_HEAD1,
    STATE_WAIT_HEAD2,
    STATE_WAIT_CMD,
    STATE_WAIT_LEN,
    STATE_WAIT_PAYLOAD,
    STATE_WAIT_CRC
  };

  void resetParser();

  bool popByte(uint8_t& au8_Byte);
  
  // Decodes incoming uart command
  void decodePacket();

  bool tryReadByte(uint8_t& au8_Byte);

  uint8_t calculateCrc(uint8_t au8_Len, uint8_t au8_Command, const uint8_t* apu8_Payload);

  void sendRaw(const uint8_t* apu8_Data, uint8_t au8_Len);

  Queue<CommandPacket, COMMAND_QUEUE_LENGTH>& ms_CommandQueue;

private:
  uint8_t mpu8_RxBuffer[RX_BUFFER_SIZE];

  volatile uint16_t mu16_Head;
  volatile uint16_t mu16_Tail;

  ComRxState me_CurrentState;
  uint8_t mu8_Len;
  uint8_t mu8_Command;
  uint8_t mpu8_RxPayload[MAX_PAYLOAD_SIZE];
  uint8_t mu8_PayloadIndex;
  uint8_t mu8_Crc;

  uint8_t mpu8_RespTx[MAX_RESPONSE_SIZE];
  // uint8_t mu8_RespLen;

  bool mb_RespReady;

};

#endif // COM_PROTOCOL_HPP