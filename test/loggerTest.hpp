#include "../logs/logger.hpp"



void test_sync_logger()
{
    using namespace FengLog;
    std::string logger_name = "sync_logger";
    LogLevel::value limit_level = FengLog::LogLevel::value::WARN; // 设置限制等级
    Formatter::ptr fmt(new Formatter("%d{%h:%m:%s}[%c][%f:%l][%p]%T %m%n"));
    LogSink::ptr stdout_sink =  SinkFactory::createSink<StdOutSink>();
    LogSink::ptr file_sink = SinkFactory::createSink<FileSink>("test_logs/sync_logger.log");

    std::vector<LogSink::ptr> sinks = {stdout_sink , file_sink};
    Logger::ptr logger(new SyncLogger(logger_name , fmt , sinks , limit_level));

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

void test_Local_builder_logger()
{
    using namespace FengLog;


    std::unique_ptr<LoggerBuilder> builder(new LocalLoggerBuilder());
    builder->buildLoggerName("sync_logger");
    builder->buildLimitLevel(FengLog::LogLevel::value::WARN);
    builder->buildFormatter("%d{%h:%m:%s}[%c][%f:%l][%p]%T %m%n");
    builder->buildSink<StdOutSink>();
    builder->buildSink<FileSink>("test_logs/sync_logger.log");

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