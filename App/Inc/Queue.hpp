
#ifndef QUEUE_HPP
#define QUEUE_HPP

#ifdef __cplusplus
extern "C"
{
#endif

#include "FreeRTOS.h"
#include "queue.h"

#ifdef __cplusplus
}
#endif

template <typename T, size_t N>
class Queue
{
public:
  Queue()
  {
    ms_QueueHandle = xQueueCreate(N, sizeof(T));
  }

  bool send(const T& at_Item, TickType_t au_Timeout = 0)
  {
    return xQueueSend(ms_QueueHandle, &at_Item, au_Timeout) == pdPASS;
  }

  bool receive(T& at_Item, TickType_t au_Timeout = portMAX_DELAY)
  {
    return xQueueReceive(ms_QueueHandle, &at_Item, au_Timeout) == pdPASS;
  }

  QueueHandle_t native()
  {
    return ms_QueueHandle;
  }

private:
  QueueHandle_t ms_QueueHandle;
};


#endif // QUEUE_HPP
