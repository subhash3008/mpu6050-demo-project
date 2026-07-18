#ifndef MOTION_DETECTOR_HPP
#define MOTION_DETECTOR_HPP

#include "ImuData.hpp"

#include <string>

class MotionDetector
{
public:
  enum class State : uint8_t
  {
    IDLE,
    MOVING,
    TILT_LEFT,
    TILT_RIGHT,
    SHAKE,
    INVALID_STATE
  };

  enum class Event : uint8_t
  {
    NONE,
    MOTION_STARTED,
    MOTION_STOPPED,
    TILT_LEFT_ENTERED,
    TILT_RIGHT_ENTERED,
    RETURNED_LEVEL,
    SHAKE_DETECTED,
    INVALID_EVENT
  };

  MotionDetector();

  void update(const ImuDataScaled& as_ImuData);

  State getState() const;

  bool getEvent(Event& ae_Event);

  void pushEvent(Event ae_Event);

private:
  void detectState(float af_Pitch, float af_Roll, float af_GyroMagnitude, float af_AccelMagnitude);

private:
  State m_CurrentState;
  State m_PrevState;
  
  static constexpr uint8_t EVENT_QUEUE_SIZE = 8U;

  Event m_EventQueue[EVENT_QUEUE_SIZE];

  uint8_t m_Head;
  uint8_t m_Tail;

  uint16_t m_MotionCounter;
  uint16_t m_StillCounter;
  uint16_t m_ShakeCounter;
};

#endif // MOTION_DETECTOR_HPP