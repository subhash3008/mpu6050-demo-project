
#include "App.hpp"

static Application app;

extern "C" void App_Start(void)
{
  app.start();
}
