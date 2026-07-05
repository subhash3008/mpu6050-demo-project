/**
 * @file ComProtocol.cpp
 * @author Subhash Chandra
 * @brief Implements communication protocol member functions
 * @version 0.1
 * @date 2026-07-04
 * 
 * @copyright Copyright (c) 2026
 * 
 */


/***************************************************
* INCLUDES
***************************************************/
#include "ComProtocol.hpp"

#ifdef __cplusplus
extern "C"
{
#endif

#include "main.h"
#include "stm32f4xx_hal_uart.h"

#ifdef __cplusplus
}
#endif

/***************************************************
* External Variables
***************************************************/

#ifdef __cplusplus
extern "C"
{
#endif

extern UART_HandleTypeDef huart2;

#ifdef __cplusplus
}
#endif

/***************************************************
* Constants
***************************************************/



/***************************************************
* Member Functions
***************************************************/
ComProtocol::ComProtocol(Queue<CommandPacket, COMMAND_QUEUE_LENGTH>& aps_Queue)
: mb_RespReady(false),
  mu16_Head(0U),
  mu16_Tail(0U),
  me_CurrentState(ComRxState::STATE_WAIT_HEAD1),
  mu8_Len(0U),
  mu8_Command(0U),
  mu8_PayloadIndex(0U),
  mu8_Crc(0U),
  // mu8_RespLen(0U),
  ms_CommandQueue(aps_Queue)
{
  resetParser();
}

void ComProtocol::
pushRxByte(uint8_t au8_Byte)
{
  uint16_t lu16_NextHead = (mu16_Head + 1) % RX_BUFFER_SIZE;

  // Drop the byte if the buffer is full
  if (lu16_NextHead == mu16_Tail)
  {
    return;
  }

  mpu8_RxBuffer[mu16_Head] = au8_Byte;
  mu16_Head = lu16_NextHead;
}

bool ComProtocol::
popByte(uint8_t& au8_Byte)
{
  if (mu16_Head == mu16_Tail)
  {
    return false;
  }

  au8_Byte = mpu8_RxBuffer[mu16_Tail];
  mu16_Tail = (mu16_Tail + 1) % RX_BUFFER_SIZE;

  return true;
}

void ComProtocol::
process()
{
  uint8_t lu8_Byte;

  while (popByte(lu8_Byte))
  {
    switch(me_CurrentState)
    {
      // Awaiting first header byte
      case ComRxState::STATE_WAIT_HEAD1:
        if (HEADER1_VALID_VAL == lu8_Byte)
        {
          me_CurrentState = ComRxState::STATE_WAIT_HEAD2;
        }
        break;
      // Awaiting second header byte
      case ComRxState::STATE_WAIT_HEAD2:
        if (HEADER2_VALID_VAL == lu8_Byte)
        {
          me_CurrentState = ComRxState::STATE_WAIT_LEN;
        }
        else
        {
          resetParser();
        }
        break;
      // Awaiting Length of the payload
      case ComRxState::STATE_WAIT_LEN:
        if ((0U == lu8_Byte) || (lu8_Byte > (MAX_PAYLOAD_SIZE + 1U)))
        {
          resetParser();
          break;
        }

        mu8_Len = lu8_Byte;
        me_CurrentState = ComRxState::STATE_WAIT_CMD;
        break;
      // Awaing a command to process
      case ComRxState::STATE_WAIT_CMD:
        mu8_Command = lu8_Byte;
        mu8_PayloadIndex = 0U;

        if (mu8_Len == 1U)
        {
          me_CurrentState = ComRxState::STATE_WAIT_CRC;
        }
        else
        {
          me_CurrentState = ComRxState::STATE_WAIT_PAYLOAD;
        }

        break;
      // Awaiting the data payload
      case ComRxState::STATE_WAIT_PAYLOAD:
        mpu8_RxPayload[mu8_PayloadIndex] = lu8_Byte;
        mu8_PayloadIndex++; // increament the index for next position

        if (mu8_PayloadIndex >= (mu8_Len - 1))
        {
          me_CurrentState = ComRxState::STATE_WAIT_CRC;
        }
        break;
      // Awaiting the checksum value
      case ComRxState::STATE_WAIT_CRC:
        mu8_Crc = lu8_Byte;
        if (calculateCrc(mu8_Len, mu8_Command, mpu8_RxPayload) == mu8_Crc)
        {
          decodePacket();
        }
        resetParser();
        break;
    }
  }
}

void ComProtocol::
decodePacket()
{
  CommandPacket ls_Packet;

  ls_Packet.command = static_cast<Command>(mu8_Command);
  ls_Packet.payloadLength = mu8_Len - 1U; // -1 to exclude Crc byte

  for (uint8_t i = 0U; i < ls_Packet.payloadLength; ++i)
  {
    ls_Packet.payload[i] = mpu8_RxPayload[i];
  }

  ms_CommandQueue.send(ls_Packet);
}

bool ComProtocol::
getNextCommand(CommandPacket& as_Packet)
{
  return ms_CommandQueue.receive(as_Packet, 0);
}

void ComProtocol::
processResponse(Response ae_Rsp, const uint8_t* apu8_Payload, const uint8_t au8_PayloadLen)
{
  uint8_t lu8_Index = 0U;

  mpu8_RespTx[lu8_Index++] = HEADER1_VALID_VAL;
  mpu8_RespTx[lu8_Index++] = HEADER2_VALID_VAL;

  uint8_t lu8_Len = au8_PayloadLen + 1U;

  mpu8_RespTx[lu8_Index++] = lu8_Len;
  mpu8_RespTx[lu8_Index++] = static_cast<uint8_t>(ae_Rsp);

  for (uint8_t i = 0; i < au8_PayloadLen; ++i)
  {
    mpu8_RespTx[lu8_Index++] = apu8_Payload[i];
  }

  mpu8_RespTx[lu8_Index++] = calculateCrc(lu8_Len, static_cast<uint8_t>(ae_Rsp), apu8_Payload);

  mb_RespReady = true;
}

char* ComProtocol::
getResponse()
{
  return reinterpret_cast<char *>(mpu8_RespTx);
}

uint8_t ComProtocol::
calculateCrc(uint8_t au8_Len, uint8_t au8_Command, const uint8_t* apu8_Payload)
{
  uint8_t lu8_Crc = 0U;

  // Currently using XOR method for crc, later add proper crypto methods
  lu8_Crc ^= au8_Len;
  lu8_Crc ^= au8_Command;
  for (uint8_t i = 0; i < (au8_Len - 1); ++i)
  {
    lu8_Crc ^= apu8_Payload[i];
  }

  return lu8_Crc;
}

void ComProtocol::
resetParser()
{
  me_CurrentState = ComRxState::STATE_WAIT_HEAD1;

  mu8_Len = 0u;
  mu8_Command = 0u;
  mu8_PayloadIndex = 0u;
  mu8_Crc = 0u;
}


bool ComProtocol::
getRespReady()
{
  return mb_RespReady;
}

void ComProtocol::
resetResp()
{
  mb_RespReady = false;

  for (uint8_t i = 0; i < MAX_RESPONSE_SIZE; ++i)
  {
    mpu8_RespTx[i] = 0U;
  }
}
