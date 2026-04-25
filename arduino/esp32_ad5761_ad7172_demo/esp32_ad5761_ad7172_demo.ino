#include "../../include/demo_app.hpp"
#include "../../include/platform_abstraction.hpp"

DemoApp app;

void setup() {
  if (!app.begin()) {
    platform::logError("Arduino", "application init failed");
    while (true) {
      platform::delayMs(1000);
    }
  }
}

void loop() {
  app.runOnce();
}
