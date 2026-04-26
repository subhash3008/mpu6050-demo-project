/**
 * @file App.cpp
 * @author Subhash Chandra
 * @brief Implements Application class member and associated functions
 * @version 0.1
 * @date 2026-04-26
 * 
 * @copyright Copyright (c) 2026
 * 
 */

/***************************************************
* INCLUDES
***************************************************/
#include <stdint.h>
#include "App.hpp"

#ifdef __cplusplus
extern "C"
{
#endif

#include "main.h"

#ifdef __cplusplus
}
#endif

/***************************************************
* External Variables
***************************************************/

extern I2C_HandleTypeDef hi2c1;

/***************************************************
* Private Variables
***************************************************/
const uint16_t g_SafeModeDelay = 150U;

/***************************************************
* Member Functions
***************************************************/

/**
 * @brief Constructor to initializes the application object by constructing its LED
 * 				with Pin PA5, blink task, log queue, logger, logger task, IMU, and IMU task
 * 				members with their required dependencies.
 */
Application::
Application()
: ms_Led(LD2_GPIO_Port, LD2_Pin),
	ms_BlinkTask(ms_Led),
	ms_LogQueue(),
	ms_Logger(ms_LogQueue),
	ms_LoggerTask(ms_LogQueue),
	ms_Imu(hi2c1),
	ms_ImuTask(ms_Imu, ms_Logger)
{}

/**
 * @brief Initializes drivers, runs diagnostics, and if diagnostics fail logs an error
 * 				and enters safe mode; otherwise it loads configurations, starts tasks,
 * 				and enters normal mode.
 */
void Application::
start()
{
	initDrivers();

	if (!runDiagnostics())
	{
		ms_Logger.error("Something went wrong!!");
		safeMode();
		return;
	}

	loadConfigurations();
	startTasks();
	enterNormalMode();
}

/**
 * @brief Initializes the drivers. For LED, turn it off. For IMU, calls
 * 				the init function of the Imu driver instance.
 */
void Application::
initDrivers()
{
	ms_Led.off();
	ms_Imu.init();
}

/**
 * @brief  Performs an IMU diagnostics sequence by verifying sensor
 * 				 identity via ms_Imu.whoAmI(), checking that the device ID
 * 				 is 0x68, optionally reading sensor data into ImuData if the
 * 				 identity check passes, and logging success or failure while
 * 				 returning the overall status.
 * @return true		if all diagnostic operations are successful
 * @return false 	if any diagnostic operations fails
 */
bool Application::
runDiagnostics()
{
	bool ok = true;

	// Inertial Measurement Unit Sensor Identity Check
	ms_Logger.info("Checking Sensor Identity.");
	uint8_t lu8_Id = 0;
	if(!ms_Imu.whoAmI(lu8_Id))
	{
		ms_Logger.error("Communication to sensor failed.");
		ok = false;
	}
	else if (0x68 != lu8_Id)
	{
		ms_Logger.error("Wrong device or address issue.");
		ok = false;
	}

	// Test sensor data once here
	ms_Logger.info("Testing sensor...");
	if (true == ok)
	{
		ImuData ls_Data{};
		ok = ms_Imu.readReg(ls_Data);
	}
	
	// TODO: Check if watchdog reset is present

	return ok;
}

void Application::
loadConfigurations()
{
	// 
}

/**
 * @brief Creates the IMU, blink, and logger freeRTOS tasks with their
 * 				configured stack sizes and priorities.
 */
void Application::
startTasks()
{
	ms_Logger.info("Starting system.");
	ms_ImuTask.start("IMU", 512, 3); // Highest priority for IMU task
	ms_BlinkTask.start("BLINK", 256, 2);
	ms_LoggerTask.start("LOGGER", 512, 1);
}

/**
 * @brief Switches the application into normal mode by turning on the ms_Led indicator.
 */
void Application::
enterNormalMode()
{
	ms_Led.on();
}

/**
 * @brief Enters an infinite loop that repeatedly toggles ms_Led and waits for g_SafeModeDelay,
 * 				indicating the application has entered a safe or fault mode.
 * 				Accesses the LED driver directly instead of BlinkTask.
 */
void Application::
safeMode()
{
	while(1)
	{
		ms_Led.toggle();
		HAL_Delay(g_SafeModeDelay);
	}
}

