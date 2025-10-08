#pragma once

#include <Arduino.h>

namespace DataAcquisition {
  class DataHandler {
    public:
      template <class T> T getData();
  };
}