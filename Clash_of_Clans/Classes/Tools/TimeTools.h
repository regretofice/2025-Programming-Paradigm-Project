#ifndef __TIME_TOOLS_H__
#define __TIME_TOOLS_H__

#include <cstdint>  // 用于int64_t类型,比longlong兼容性强

class TimeTools {
 public:
  // 获取当前时间（毫秒级，自1970年1月1日起）
  static int64_t getCurrentTimeMs();

  // 计算两个时间点的差值（秒）
  static int64_t getTimeDiffInSeconds(int64_t startMs, int64_t endMs);
};

#endif  // __TIME_TOOLS_H__