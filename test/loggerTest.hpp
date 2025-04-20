#include "../logs/logger.hpp"
#include "../logs/looper.hpp"

using namespace FengLog;


// 测试同步日志器
void test_sync_logger()
{
    std::string logger_name = "sync_logger";
    LogLevel::value limit_level = FengLog::LogLevel::value::WARN; // 设置限制等级
    Formatter::ptr fmt(new Formatter("%d{%h:%m:%s}[%c][%f:%l][%p]%T %m%n"));
    LogSink::ptr stdout_sink =  SinkFactory::createSink<StdOutSink>();
    LogSink::ptr file_sink = SinkFactory::createSink<FileSink>("test_logs/sync_logger.log");

    std::vector<LogSink::ptr> sinks = {stdout_sink , file_sink};
    Logger::ptr logger(new SyncLogger(logger_name , fmt , sinks , limit_level));

    logger->debug(__FILE__ , __LINE__ , "%s" , "测试日志");
    logger->info(__FILE__ , __LINE__ ,"%s" , "测试日志");
    logger->warn(__FILE__ , __LINE__ ,"%s" , "测试日志");
    logger->error(__FILE__ , __LINE__ ,"%s" , "测试日志");
    logger->fatal(__FILE__ , __LINE__ ,"%s" , "测试日志");

    size_t cursize = 0  , count = 0;
    while(cursize < 1000000)
    {
        logger->fatal(__FILE__ , __LINE__ , "测试日志-%d" , count++);
        cursize += 100;
    }
}

// 测试局部构建器
void test_Local_builder_logger()
{
    std::unique_ptr<LoggerBuilder> builder(new LocalLoggerBuilder());
    builder->buildLoggerName("sync_logger");
    builder->buildLimitLevel(FengLog::LogLevel::value::WARN);
    builder->buildFormatter("%d{%h:%m:%s}[%c][%f:%l][%p]%T %m%n");
    builder->buildSink<StdOutSink>();
    builder->buildSink<FileSink>("test_logs/async_logger.log");

    Logger::ptr logger = builder->build();



    logger->debug(__FILE__ , __LINE__ ,"%s" , "测试日志");
    logger->info(__FILE__ , __LINE__ ,"%s" , "测试日志");
    logger->warn(__FILE__ , __LINE__ ,"%s" , "测试日志");
    logger->error(__FILE__ , __LINE__ ,"%s" , "测试日志");
    logger->fatal(__FILE__ , __LINE__ ,"%s" , "测试日志");

    size_t cursize = 0  , count = 0;
    while(cursize < 1000000)
    {
        logger->fatal(__FILE__ , __LINE__ , "测试日志-%d" , count++);
        cursize += 100;
    }
}


// 测试异步日志器
void test_async_logger()
{
    std::unique_ptr<LoggerBuilder> builder(new LocalLoggerBuilder());
    builder->buildLoggerName("async_logger"); // 设置日志名称
    builder->buildLimitLevel(FengLog::LogLevel::value::WARN); // 设置日志等级
    builder->buildFormatter("%d{%h:%m:%s}[%c][%f:%l][%p]%T %m%n"); // 设置日志格式
    builder->buildSink<StdOutSink>(); // 设置日志输出到标准输出
    builder->buildSink<FileSink>("test_logs/sync_logger.log"); // 设置日志输出到文件
    builder->buildLoggerType(LoggerType::LOGGER_ASYNC); // 设置日志类型为异步 
    //builder->buildLooperType(AsyncType::ASYNC_UNSAFE); // 设置异步日志的缓冲区类型为不安全
    Logger::ptr logger = builder->build();

    logger->debug(__FILE__ , __LINE__ ,"%s" , "测试日志");
    logger->info(__FILE__ , __LINE__ ,"%s" , "测试日志");
    logger->warn(__FILE__ , __LINE__ ,"%s" , "测试日志");
    logger->error(__FILE__ , __LINE__ ,"%s" , "测试日志");
    logger->fatal(__FILE__ , __LINE__ ,"%s" , "测试日志");

    // size_t cursize = 0  , count = 1;
    // while(cursize < 1000000)
    // {
    //     logger->fatal(__FILE__ , __LINE__ , "测试日志-%d" , count++);
    //     cursize += 100;
    // }

    size_t  count = 1;
    while(count < 50000)
    {
        logger->fatal(__FILE__ , __LINE__ , "测试日志-%d" , count++);
    }
}

// 测试全局构建器
void test_global_builder_logger()
{
    std::unique_ptr<LoggerBuilder> builder(new GlobalLoggerBuilder());
    //builder->buildLoggerName("global_sync_logger"); // 设置日志器名称
    builder->buildLoggerName("global_async_logger");

    builder->buildLimitLevel(FengLog::LogLevel::value::DEBUG);
    builder->buildFormatter("%d{%h:%m:%s}[%c][%f:%l][%p]%T %m%n");
    builder->buildSink<StdOutSink>();
    builder->buildSink<FileSink>("test_logs/global_sync_logger.log");

    // builder->buildLoggerType(LoggerType::LOGGER_SYNC); // 设置日志器类型为同步
    builder->buildLoggerType(LoggerType::LOGGER_ASYNC); // 设置日志器类型为异步

    Logger::ptr logger = builder->build();

    // logger->debug(__FILE__ , __LINE__ ,"%s" , "测试日志");
    // logger->info(__FILE__ , __LINE__ ,"%s" , "测试日志");
    // logger->warn(__FILE__ , __LINE__ ,"%s" , "测试日志");
    // logger->error(__FILE__ , __LINE__ ,"%s" , "测试日志");
    // logger->fatal(__FILE__ , __LINE__ ,"%s" , "测试日志");

    logger->debug("%s" , "测试日志");
    logger->info("%s" , "测试日志");
    logger->warn("%s" , "测试日志");
    logger->error("%s" , "测试日志");
    logger->fatal("%s" , "测试日志");

    

    size_t  count = 1;
    while(count <= 50)
    {
        logger->fatal("测试日志-%d" , count++);
    }
    
}
