/**
 * @file MotionDetector.cpp
 * @author Subhash Chandra
 * @brief Handle the Motion Detection functionality for the application
 * @version 0.1
 * @date 2026-07-17
 * 
 * @copyright Copyright (c) 2026
 * 
 */

#include "MotionDetector.hpp"

#include <math.h>

/***************************************************
* MEMBER FUNCTIONS
***************************************************/

MotionDetector::MotionDetector()
{
  m_CurrentState = State::IDLE;
  m_PrevState = State::IDLE;

  m_Head = 0U;
  m_Tail = 0U;

  m_MotionCounter = 0U;
  m_StillCounter = 0U;
  m_ShakeCounter = 0U;
}

void MotionDetector::
update(const ImuDataScaled& as_ImuData)
{
  float lf_Omega = sqrt((as_ImuData.f_GxDps * as_ImuData.f_GxDps) +
                              (as_ImuData.f_GyDps * as_ImuData.f_GyDps) + 
                              (as_ImuData.f_GzDps * as_ImuData.f_GzDps));
  float lf_AccelM = sqrt((as_ImuData.f_AxG * as_ImuData.f_AxG) +
                               (as_ImuData.f_AyG * as_ImuData.f_AyG) +
                               (as_ImuData.f_AzG * as_ImuData.f_AzG));

  m_PrevState = m_CurrentState;

  detectState(as_ImuData.f_PitchDeg, as_ImuData.f_RollDeg, lf_Omega, lf_AccelM);

  if (m_CurrentState == m_PrevState) // Check if there is no state change
  {
    return;
  }

  switch (m_CurrentState)
  {
    case State::MOVING:
      pushEvent(Event::MOTION_STARTED);
      break;

    case State::IDLE:
      if (State::MOVING == m_PrevState)
      {
        pushEvent(Event::MOTION_STOPPED);
      }

      if ((State::TILT_LEFT == m_PrevState) ||
          (State::TILT_RIGHT == m_PrevState))
      {
        pushEvent(Event::RETURNED_LEVEL);
      }
      break;

    case State::TILT_LEFT:
      pushEvent(Event::TILT_LEFT_ENTERED);
      break;

    case State::TILT_RIGHT:
      pushEvent(Event::TILT_RIGHT_ENTERED);
      break;

    case State::SHAKE:
      pushEvent(Event::SHAKE_DETECTED);
      break;

    default:
      pushEvent(Event::INVALID_EVENT);
  }
}

void MotionDetector::
detectState(float af_Pitch, float af_Roll, float af_GyroMagnitude, float af_AccelMagnitude)
{
  constexpr float MOTION_THRESHOLD = 10.0f;
  constexpr float SHAKE_THRESHOLD = 120.0f;
  constexpr float TILT_RIGHT_THRESHOLD = 20.0f;
  constexpr float TILT_LEFT_THRESHOLD = TILT_RIGHT_THRESHOLD * -1;

  constexpr uint16_t SHAKE_COUNTER_THRESHOLD = 3U;
  constexpr uint16_t MOTION_COUNTER_THRESHOLD = 3U;
  constexpr uint16_t IDEAL_COUNTER_THRESHOLD = 5U;

  if (af_GyroMagnitude > SHAKE_THRESHOLD)
  {
    m_ShakeCounter++;
    if (m_ShakeCounter > SHAKE_COUNTER_THRESHOLD) // check if actual shake is performed
    {
      m_CurrentState = State::SHAKE;
      return;
    }
  }
  else
  {
    m_ShakeCounter = 0U; // reset the shake counter
  }

  if (af_Roll > TILT_RIGHT_THRESHOLD)
  {
    m_CurrentState = State::TILT_RIGHT;
    return;
  }

  if (af_Roll < TILT_LEFT_THRESHOLD)
  {
    m_CurrentState = State::TILT_LEFT;
    return;
  }

  if (af_GyroMagnitude > MOTION_THRESHOLD)
  {
    m_MotionCounter++;
    m_StillCounter = 0U;

    if (m_MotionCounter > MOTION_COUNTER_THRESHOLD)
    {
      m_CurrentState = State::MOVING;
      return;
    }
  }
  else
  {
    m_MotionCounter = 0U;
    m_StillCounter++;
  }

  // ideal state would be when only gravity is acting on the accelerometer, 
  // the value of the sensor magnitude would be 1.0f as only value 1 for z axis would be there 
  if ((fabsf(af_AccelMagnitude - 1.0f) < 0.4f) &&
      (af_GyroMagnitude < 3.0f) &&
      (m_StillCounter > IDEAL_COUNTER_THRESHOLD))
  {
    m_CurrentState = State::IDLE;
  }
}

bool MotionDetector::
getEvent(Event& ae_Event)
{
  if (m_Head == m_Tail)
  {
    return false;
  }

  ae_Event = m_EventQueue[m_Tail];

  m_Tail = (m_Tail + 1) % EVENT_QUEUE_SIZE; // Update current event pointer

  return true;
}

void MotionDetector::
pushEvent(Event ae_Event)
{
  uint8_t lu8_Next = (m_Head + 1) % EVENT_QUEUE_SIZE;

  if (lu8_Next == m_Tail)
  {
    return; // Queue full
  }

  m_EventQueue[lu8_Next] = ae_Event;
  m_Head = lu8_Next;
}

MotionDetector::State MotionDetector::
getState() const
{
  return m_CurrentState;
}
