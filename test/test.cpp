
#include "../logs/util.hpp"
#include "../logs/message.hpp"
#include "../logs/formatter.hpp"
#include "../logs/sink.hpp"
#include "../logs/logger.hpp"
#include "../logs/feng_log.h"   


#include "formatTest.hpp"
#include "sinkTest.hpp"
#include "loggerTest.hpp"
#include "bufferTest.hpp"



// 文件操作测试
void UiltFileTest()
{
    std::string path = "./a/b/c/d.txt";


    if(FengLog::Util::File::exists(path))
    {
        std::cout << "this filepath exist" << std::endl; 
    }
    else{
        std::cout << "this filepath is not exist" << std::endl;
    }
    std::string isPath = FengLog::Util::File::path(path);
    std::cout << isPath << std::endl;

    std::string path2 = "./a/b/d.txt";
    std::string path3 = "./a/d.txt";


    FengLog::Util::File::create_directory(path);
    FengLog::Util::File::create_directory(path2);
    FengLog::Util::File::create_directory(path3);

}


// 格式化器测试
void FormatterTest()
{
        //UiltFileTest();
    //FormatterTest();

    // 添加基本测试
    //BasicFormatterTest();
    
    // 测试各个FormatItem子类
    //FormatItemTest();
    
    // 测试不同格式模式的Formatter
    //FormatterPatternTest();
    
    // 测试格式解析错误情况
    FormatterErrorTest();
}


// 运行所有 Sink 测试
void SinkTests() {
    std::cout << "开始运行 Sink 模块测试..." << std::endl;
    
    // testStdOutSink();
    // testFileSink();
    // testRollSink();
    testTimeRollSink();

    std::cout << "Sink 模块测试完成..." << std::endl;

}

// 缓冲区测试
void BufferTest()
{
    test_buffer();
}


// 日志器与构建器测试
void LoggerTest()
{
    //test_sync_logger();

    //test_Local_builder_logger();

    //test_async_logger();

    test_global_builder_logger();
}


// 测试FengLog
void FengLogTest()
{
    using namespace FengLog;
    std::unique_ptr<LoggerBuilder> builder(new GlobalLoggerBuilder());
    builder->buildLoggerName("async_logger"); // 设置日志名称
    builder->buildLimitLevel(FengLog::LogLevel::value::WARN); // 设置日志等级
    builder->buildFormatter("[%c][%f:%l][%p]%T %m%n"); // 设置日志格式
    builder->buildSink<StdOutSink>(); // 设置日志输出到标准输出
    builder->buildSink<FileSink>("test_logs/feng_logger.log"); // 设置日志输出到文件
    builder->buildLoggerType(LoggerType::LOGGER_ASYNC); // 设置日志类型为异步 
    //builder->buildLooperType(AsyncType::ASYNC_UNSAFE); // 设置异步日志的缓冲区类型为不安全
    builder->build();

    //指定日志器进行输出
    FengLog::Logger::ptr logger = FengLog::LoggerManager::get_instance().get_logger("async_logger");

    logger->debug("%s", "测试日志");
    logger->info("%s" , "测试日志");
    logger->warn("%s" , "测试日志");
    logger->error("%s" , "测试日志");
    logger->fatal("%s" , "测试日志");

    //使用默认root日志器进行输出
    // DEBUG("测试日志");
    // INFO("测试日志");
    // WARN("测试日志");
    // ERROR("测试日志");
    // FATAL("测试日志");


    size_t  count = 1;
    while(count < 50)
    {
        logger->fatal("测试日志-%d" , count++);
    }

}


int main()
{
    //FormatterTest();
    
    //SinkTests();
    
    //BufferTest();
    
    //LoggerTest();

    FengLogTest();

    return 0;
}
