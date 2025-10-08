#pragma once

#include <Arduino.h>

namespace AbstractModule {
  class AbstractHandler {
    public:
      virtual void initialization();
  };
}