
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

extern I2C_HandleTypeDef hi2c1;

const uint16_t g_SafeModeDelay = 150U;

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

void Application::
initDrivers()
{
	ms_Led.off();
	ms_Imu.init();
}

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
	
	// Check if watchdog reset is present

	return ok;
}

void Application::
loadConfigurations()
{
	// 
}

void Application::
startTasks()
{
	ms_Logger.info("Starting system.");
	ms_ImuTask.start("IMU", 512, 3);
	ms_BlinkTask.start("BLINK", 256, 2);
	ms_LoggerTask.start("LOGGER", 512, 1);
}

void Application::
enterNormalMode()
{
	ms_Led.on();
}

void Application::
safeMode()
{
	while(1)
	{
		ms_Led.toggle();
		HAL_Delay(g_SafeModeDelay);
	}
}

