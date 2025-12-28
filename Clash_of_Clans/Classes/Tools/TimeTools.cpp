#include "Tools/TimeTools.h"

#include <chrono>

int64_t TimeTools::getCurrentTimeMs() {
  auto now = std::chrono::system_clock::now();
  auto duration = now.time_since_epoch();
  return std::chrono::duration_cast<std::chrono::milliseconds>(duration)
      .count();
}

int64_t TimeTools::getTimeDiffInSeconds(int64_t startMs, int64_t endMs) {
  if (endMs < startMs) return 0;
  return (endMs - startMs) / 1000;
}