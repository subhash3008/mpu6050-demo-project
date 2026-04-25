#include "ImuTask.hpp"

ImuTask::
// ImuTask(ImuSensor& aps_Imu, Queue<ImuData, 16>& aps_DataQueue, )
// : mps_Imu(aps_Imu), mps_DataQueue(aps_DataQueue)
ImuTask(ImuSensor& aps_Imu, Logger& aps_Logger)
: mps_Imu(aps_Imu), ms_Logger(aps_Logger)
{}

void ImuTask::
run()
{
  TickType_t lu_LastWake = xTaskGetTickCount();
  uint16_t lu16_ImuTaskDelay = 500U;
  ImuData ls_Data;

  while(1)
  {
    if (mps_Imu.readReg(ls_Data))
    {
      // mps_DataQueue.send(ls_Data);
      ms_Logger.info(
        "IMU AX=%d, AY=%d, AZ=%d, GX=%d, GY=%d, GZ=%d",
        ls_Data.i16_AX, ls_Data.i16_AY, ls_Data.i16_AZ,
        ls_Data.i16_GX, ls_Data.i16_GY, ls_Data.i16_GZ
      );
    }

    vTaskDelayUntil(&lu_LastWake, lu16_ImuTaskDelay);
  }
}
