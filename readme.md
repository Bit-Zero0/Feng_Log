# FengLog - 一个C++高性能异步日志库

FengLog 是一个轻量级、高性能、线程安全的C++日志库。它支持同步和异步两种日志模式，并提供了灵活的日志格式化、多种日志输出目的地以及简单的构建器模式，可以轻松集成到你的项目中。

## ✨ 功能特性

  - **同步/异步模式**: 支持同步写入和高性能的异步写入模式。
  - **线程安全**: 所有日志记录操作都是线程安全的。
  - **多种日志级别**: 支持 `DEBUG`, `INFO`, `WARN`, `ERROR`, `FATAL` 五种日志级别。
  - **灵活的格式化**: 用户可以通过类似 `log4j` 的模式字符串 (`%d{%H:%M:%S}[%p]%T%m%n`) 自定义日志格式。
  - **多种输出地 (Sink)**:
      - `StdOutSink`: 输出到控制台。
      - `FileSink`: 输出到指定文件。
      - `RollSink`: 按文件大小滚动记录日志。
      - `TimeRollSink`: 按时间（天/小时/分钟/秒）滚动记录日志。
      - 支持用户**自定义**日志的输出目的地。
  - **可扩展的 Sink**: 支持用户自定义日志的输出目的地。
  - **构建器模式 (Builder)**: 通过 `LoggerBuilder` 链式调用，轻松构建和配置日志器。
  - **全局日志器管理**: `LoggerManager` 单例管理项目中所有日志器，方便随时获取。
  - **性能测试**: 附带 `benchmark` 模块，方便测试和比较不同模式下的性能。

## 📁 项目结构
Feng_Log/
├── bench/
│   ├── bench.cpp           # 性能测试程序，用于评测同步和异步日志在不同负载下的表现
│   └── makefile            # 用于编译 bench.cpp 性能测试程序的 makefile 文件
├── logs/
│   ├── buffer.hpp          # 异步日志的缓冲区模块，用于暂存日志数据
│   ├── feng_log.h          # 日志库的主头文件，供外部用户直接引用，并定义了便捷的宏
│   ├── formatter.hpp       # 日志格式化器，负责将日志消息按指定模式转换成字符串
│   ├── level.hpp           # 定义日志级别 (DEBUG, INFO, WARN, ERROR, FATAL)
│   ├── logger.hpp          # 日志库核心，定义了日志器基类、同步/异步日志器、构建器和管理器
│   ├── looper.hpp          # 异步日志的核心引擎，管理后台工作线程和缓冲区的数据交换
│   ├── message.hpp         # 定义日志消息结构体(LogMsg)，封装单条日志的所有信息
│   ├── sink.hpp            # 定义日志的输出地(Sink)，如控制台、文件、滚动文件等
│   └── util.hpp            # 工具类，提供文件操作、时间获取等辅助功能
└── test/
    ├── bufferTest.hpp      # 针对 buffer.hpp 的单元测试
    ├── formatTest.hpp      # 针对 formatter.hpp 的单元测试
    ├── loggerTest.hpp      # 针对 logger.hpp 的单元测试
    ├── makefile            # 用于编译 test.cpp 单元测试程序的 makefile 文件
    ├── sinkTest.hpp        # 针对 sink.hpp 的单元测试
    └── test.cpp            # 单元测试的主入口文件，调用并执行所有测试用例

## 项目架构图

![项目架构图](asserts/FengLog_Flowchart.drawio)


## 🛠️ 核心组件解析
  - **Logger (`logger.hpp`)**: 日志器的基类，定义了核心的日志记录接口。
      - `SyncLogger`: 同步日志器，将日志直接写入到 Sink。
      - `AsyncLogger`: 异步日志器，将日志消息放入缓冲区，由后台线程负责写入。
  - **LogSink (`sink.hpp`)**: 定义日志输出目标的抽象基类。你可以轻松地扩展自己的 Sink。
  - **Formatter (`formatter.hpp`)**: 格式化器，根据用户定义的模式字符串将 `LogMsg` 对象转换成最终的日志文本。
  - **AsyncLooper (`looper.hpp`)**: 异步日志的核心，采用“生产者-消费者”模型，管理后台的日志写入线程和缓冲区。
  - **LoggerBuilder (`logger.hpp`)**: 构建器，提供了链式API来配置和创建 `Logger` 实例。
      - `LocalLoggerBuilder`: 创建一个局部的 `Logger` 实例。
      - `GlobalLoggerBuilder`: 创建 `Logger` 实例并将其注册到全局的 `LoggerManager` 中。
  - **LoggerManager (`logger.hpp`)**: 单例模式的日志器管理器，负责维护项目中的所有日志器实例。

## 🚀 如何构建和运行

项目提供了 `makefile` 用于快速编译。

### 编译和运行测试

```bash
cd test/
make
./test.exe
```

### 编译和运行性能测试

```bash
cd bench/
make
./bench
```

好的，没问题。直接使用宏是您日志库的一大亮点，确实应该在 "如何使用" 部分突出展示。这能让新用户立即上手。

我将为您重写这部分内容，增加一个专门介绍如何使用宏的章节，并将其作为推荐方式。

-----

## 📝 如何使用

### 1\. 引入头文件

最简单的方式是直接引入 `feng_log.h`，它包含了所有需要使用的接口。

```cpp
#include "../logs/feng_log.h"
```

### 2\. 使用宏直接记录日志

**这是最简单、最推荐的日志记录方式。** 你的库提供了一组预定义的宏 (`DEBUG`, `INFO`, `WARN`, `ERROR`, `FATAL`)，它们会自动使用默认的`root`日志器，并为你自动填充文件名和行号。

**优点:**

  * **无需配置**：开箱即用，直接输出到控制台。
  * **代码简洁**：一行代码即可完成日志记录。
  * **信息丰富**：自动包含代码位置信息，便于调试。

**代码示例:**

```cpp
#include "logs/feng_log.h"

int main() {
    // 无需任何初始化和配置，直接调用宏
    INFO("This is an info message. The program is starting.");
    WARN("The configuration file is missing, using default settings.");
    
    int user_id = 12345;
    ERROR("Failed to process request for user_id: %d.", user_id);

    FATAL("Database connection lost. The application cannot continue.");

    return 0;
}
```

**输出示例:**

```
[15:30:10][thread_id][INFO][root][main.cpp:5] This is an info message. The program is starting.
[15:30:10][thread_id][WARN][root][main.cpp:6] The configuration file is missing, using default settings.
[15:30:10][thread_id][ERROR][root][main.cpp:9] Failed to process request for user_id: 12345.
[15:30:10][thread_id][FATAL][root][main.cpp:11] Database connection lost. The application cannot continue.
```

### 3\. (可选) 配置自定义日志器

如果默认的控制台输出不能满足你的需求，你可以构建一个或多个自定义的日志器。下面的例子展示了如何创建一个名为 `my_logger` 的异步日志器，它会同时将 `WARN` 及以上级别的日志输出到控制台和文件 `my_logger.log`。

```cpp
#include "logs/feng_log.h"

void setup_logger() {
    using namespace FengLog;
    // 使用全局构建器
    std::unique_ptr<LoggerBuilder> builder(new GlobalLoggerBuilder());

    // 链式调用进行配置
    builder->buildLoggerName("my_logger");                            // 日志器名称
    builder->buildLimitLevel(LogLevel::value::WARN);                   // 只记录WARN及以上级别的日志
    builder->buildFormatter("%d{%Y-%m-%d %H:%M:%S}[%c][%p] %m%n");      // 设置日志格式
    builder->buildSink<StdOutSink>();                                  // 添加控制台输出
    builder->buildSink<FileSink>("logs/my_logger.log");                // 添加文件输出
    builder->buildLoggerType(LoggerType::LOGGER_ASYNC);                // 设置为异步模式

    // 构建日志器, 它将被自动添加到LoggerManager中
    builder->build();
}
```

### 4\. 获取并使用日志器对象

对于已经配置好的日志器，你可以从 `LoggerManager` 中获取其实例来进行日志记录。

```cpp
#include "logs/feng_log.h"

void log_with_specific_logger() {
    // 1. 确保日志器已经配置
    setup_logger();

    // 2. 从管理器中获取日志器
    FengLog::Logger::ptr logger = FengLog::get_logger("my_logger");

    // 3. 记录日志 (只有WARN, ERROR, FATAL会被记录)
    if (logger) {
        logger->warn("Something needs attention."); // 会被记录
        logger->error("An error occurred: %s", "File not found"); // 会被记录
    }
}
```



## 🔩 自定义输出地 (Custom Sink)

FengLog 的设计是高度可扩展的，你可以轻松创建自己的日志输出目标（Sink），比如将日志发送到网络、数据库或第三方服务。

只需遵循以下两个步骤：

1.  **继承 `FengLog::LogSink` 类。**
2.  **实现纯虚函数 `virtual void log(const char* data, size_t len)`。**

所有经过格式化后的日志字符串最终都会通过 `log` 方法传递给你，你可以在这个方法中实现你自己的逻辑。

#### 示例：创建一个输出到 `stderr` 的 Sink

下面是一个简单的例子，我们创建一个 `MyCustomSink`，它会将所有日志输出到标准错误流 (`stderr`)，并为每条日志添加一个 "[CUSTOM] " 前缀。

```cpp
#include "logs/sink.hpp"
#include <iostream>

// 1. 继承 LogSink
class MyCustomSink : public FengLog::LogSink {
public:
    // 2. 实现 log 方法
    void log(const char* data, size_t len) override {
        // 在这里实现你的自定义逻辑
        // 例如，我们输出到 stderr 并添加前缀
        std::cerr.write("[CUSTOM] ", 9);
        std::cerr.write(data, len);
    }
};
```

#### 如何使用自定义 Sink

在构建日志器时，你可以像使用内置 Sink 一样使用你的自定义 Sink。

```cpp
#include "logs/feng_log.h"
#include "MyCustomSink.h" // 假设你把它放在了这个头文件中

void setup_custom_logger() {
    using namespace FengLog;
    std::unique_ptr<LoggerBuilder> builder(new GlobalLoggerBuilder());

    builder->buildLoggerName("custom_sink_logger");
    builder->buildLimitLevel(LogLevel::value::INFO);
    builder->buildFormatter("[%p] %m%n");

    // 添加内置的 Sink
    builder->buildSink<StdOutSink>();

    // 添加你的自定义 Sink
    builder->buildSink<MyCustomSink>();

    builder->build();
}

int main() {
    setup_custom_logger();

    Logger::ptr logger = get_logger("custom_sink_logger");

    // 这条日志会同时输出到 stdout 和 stderr (带 [CUSTOM] 前缀)
    logger->info("Testing custom sink!"); 

    return 0;
}
```