### 多线程异步日志库

### Highlights
**日志格式**：[LOG_LEVEL] 20990101 12:10:01.123456 hello - File.cpp:21

**特点：**

1. 时间戳，使用了标准库std::chrono库，精度能达到纳秒级别

2. 更改了默认的 整数浮点数转字符串的算法

   具体： snprintf 替換成領域最新的算法，速度快数倍

3. 高精度的时间间隔计算，最高精确到纳秒($1e^{-9}s$), 时间戳目前使用微秒（够用）

4. 避免使用time_t，防止在2038年发生溢出

5. 支持彩色输出日志（例如对于不同的日志级别采用不同的颜色来显示日志级别[INFO]）

### Usage
```c++
#include "Logging.h"

// log to console
LOG_INFO << "your log message\n";

//TODO: log to file
```

