/**
 * @file Queue.hpp
 * @author Subhash Chandra
 * @brief Wrapper for FreeRTOS Queue
 * @version 0.1
 * @date 2026-04-26
 * 
 * @copyright Copyright (c) 2026
 * 
 */
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

/**
 * @brief Fixed-size templated wrapper for a FreeRTOS queue holding N elements of type T,
 *        providing construction, send, receive, and native handle access.
 * 
 * @tparam T Type for Queue
 * @tparam N Count of max elements in Queue
 */
template <typename T, size_t N>
class Queue
{
public:
  // Constructor for initializing a fixed-capacity queue instance for element type T and size N.
  Queue()
  {
    ms_QueueHandle = xQueueCreate(N, sizeof(T));
  }

  /**
   * @brief Sends the element in Queue
   * 
   * @param at_Item     Item to be be enqueued
   * @param au_Timeout  Timeout for the send operation
   * @return true   if item is enqueued
   * @return false  if item is not enqueued
   */
  bool send(const T& at_Item, TickType_t au_Timeout = 0)
  {
    return xQueueSend(ms_QueueHandle, &at_Item, au_Timeout) == pdPASS;
  }

  /**
   * @brief Receives the data from the queue
   * 
   * @param at_Item     Item reference to stored the dequeued item
   * @param au_Timeout  Timeout for the fetch operation
   * @return true   if the element is dequeued
   * @return false  if the element is not dequeued
   */
  bool receive(T& at_Item, TickType_t au_Timeout = portMAX_DELAY)
  {
    return xQueueReceive(ms_QueueHandle, &at_Item, au_Timeout) == pdPASS;
  }

  /**
   * @brief To make the queue handle public
   * 
   * @return QueueHandle_t
   */
  QueueHandle_t native()
  {
    return ms_QueueHandle;
  }

private:
  QueueHandle_t ms_QueueHandle; // Queue Handle
};


#endif // QUEUE_HPP
