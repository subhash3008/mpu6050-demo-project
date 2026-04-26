/**
 * @file App.hpp
 * @author Subhash Chandra
 * @brief Header file for defining application essentials
 * @version 0.1
 * @date 2026-04-26
 * 
 * @copyright Copyright (c) 2026
 * 
 */

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

/**
 * @brief Application Class is the overarching class for implementing
 *        complete application for Inertial Measurement Unit.
 */
class Application
{
public:
  // Constructor for Application that initializes a new Application
  // instance and sets up its private local state.
  Application();

  // Begins the application’s execution or startup sequence without returning a value.
  void start();

private:
  // Initializes the application’s driver subsystem by setting up any required
  // hardware or software drivers before use.
  void initDrivers();

  // Performs self-checks or diagnostic routines for the Application
  // instance and returns true if the diagnostics succeed, otherwise false.
  bool runDiagnostics();

  // Loads configuration settings for the Application instance, initializing
  // or refreshing its runtime configuration state.
  void loadConfigurations();

  // Initiates all of the application's freeRTOS tasks.
  void startTasks();

  // Switches the application into its normal operating mode.
  void enterNormalMode();

  // Puts the application into a safe mode state.
  void safeMode();

  Led ms_Led; // Led instance to control user led operations
  BlinkTask ms_BlinkTask; // Task used to blink user led
  Queue<LogMessage, 16> ms_LogQueue;  // Message Queue for logging the data over UART
  Logger ms_Logger; // Driver for handling the incoming logs from tasks
  LoggerTask ms_LoggerTask; // Task to process the log from the message queue
  ImuSensor ms_Imu; // Driver instance for IMU sensor
  ImuTask ms_ImuTask; // Task to process the data from IMU driver
};


#endif // APP_HPP
