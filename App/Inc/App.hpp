#ifndef APP_HPP
#define APP_HPP

#include "Led.hpp"
#include "BlinkTask.hpp"
#include "LogMessage.hpp"
#include "Logger.hpp"
#include "LoggerTask.hpp"
#include "ImuTask.hpp"
#include "Queue.hpp"
#include "ImuSensor.hpp"

class Application
{
public:
  Application();

  void start();

private:
  void initDrivers();
  bool runDiagnostics();
  void loadConfigurations();
  void startTasks();
  void enterNormalMode();
  void safeMode();

  Led ms_Led;
  BlinkTask ms_BlinkTask;
  Queue<LogMessage, 16> ms_LogQueue;
  Logger ms_Logger;
  LoggerTask ms_LoggerTask;
  ImuSensor ms_Imu;
  ImuTask ms_ImuTask;
};


#endif // APP_HPP
