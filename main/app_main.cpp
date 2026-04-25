#include "demo_app.hpp"
#include "platform_abstraction.hpp"

extern "C" void app_main(void) {
  static DemoApp app;
  if (!app.begin()) {
    platform::logError("app_main", "application init failed");
    while (true) {
      platform::delayMs(1000);
    }
  }

  while (true) {
    app.runOnce();
  }
}
