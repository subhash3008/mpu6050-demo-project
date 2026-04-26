/**
 * @file AppStart.cpp
 * @author Subhash Chandra
 * @brief Glue component between C and C++
 * @version 0.1
 * @date 2026-04-26
 * 
 * @copyright Copyright (c) 2026
 * 
 */
#include "App.hpp"

// Application instance
static Application app;

// Called in main.c for starting the application
extern "C" void App_Start(void)
{
  app.start();
}
