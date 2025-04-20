#include "../logs/feng_log.h"

#include <chrono>

using namespace FengLog;
void bench(const std::string& logger_name , size_t thread_num , size_t msg_num , size_t msg_size)
{
    Logger::ptr logger = get_logger(logger_name);
    
    if(logger == nullptr)
    {
        std::cout << "logger is nullptr" << std::endl;
        return;
    }
    
    std::cout << "输入线程数量: " << thread_num <<"条" <<std::endl;
    std::cout << "输出日志数量: " << msg_num << std::endl;
    std::cout << "输出日志大小: " << msg_num * msg_size / 1024  << "KB" << std::endl;

    std::string msg(msg_size , 'a');
    std::vector<std::thread> threads;
    std::vector<double> costs(thread_num); // 预先分配空间
    size_t msg_per_thread = msg_num / thread_num; // 每个线程的消息数量
    for(int i = 0; i < thread_num; i++)
    {
        threads.emplace_back([& , i]()
        {
            auto start = std::chrono::high_resolution_clock::now();
            for(int j = 0; j < msg_per_thread; j++)
            {
                logger->debug("%s" , msg.c_str());
            }

            auto end = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double> duration = end - start;
            costs[i] = duration.count();
            std::cout << "thread:" << i << "   输出数量:" << msg_per_thread <<"   cost " << duration.count() << " s" << std::endl;
        });
    }

    for(auto& thread : threads)
    {
        thread.join();
    }

    // 计算总耗时
    double max_cost = costs[0];
    for (int i = 0; i < thread_num; i++) 
    {
        max_cost = max_cost < costs[i] ? costs[i] : max_cost; 
    }
    size_t msg_per_sec = msg_num / max_cost;
    size_t size_per_sec = (msg_num * msg_size) / (max_cost * 1024); // 8:进行输出打印

    std::cout << "最大耗时:" << max_cost << "s" << std::endl;
    std::cout<< "每秒输出曰志数量:" <<msg_per_sec<< "条\n" ;
    std::cout <<"每秒输出曰志大小:"<< size_per_sec << "KB\n";
}

void sync_bench()
{
    std::unique_ptr<LoggerBuilder> builder(new GlobalLoggerBuilder());
    builder->buildLoggerName("sync_logger"); // 设置日志器名称
    builder->buildLimitLevel(FengLog::LogLevel::value::DEBUG);
    builder->buildFormatter("%d{%h:%m:%s}[%c][%f:%l][%p]%T %m%n");
    //builder->buildSink<StdOutSink>();
    builder->buildSink<FileSink>("sync_logger.log");

    builder->buildLoggerType(LoggerType::LOGGER_SYNC); // 设置日志器类型为同步

    builder->build();

    bench("sync_logger" , 1 , 1000000 , 100); //4.68497 s
}


void async_bench()
{
    std::unique_ptr<LoggerBuilder> builder(new GlobalLoggerBuilder());
    builder->buildLoggerName("async_logger"); // 设置日志器名称
    builder->buildLimitLevel(FengLog::LogLevel::value::DEBUG);
    builder->buildFormatter("%d{%h:%m:%s}[%c][%f:%l][%p]%T %m%n");
    //builder->buildSink<StdOutSink>();
    builder->buildSink<FileSink>("async_logger.log");
    builder->buildLoggerType(LoggerType::LOGGER_ASYNC); // 设置日志器类型为同步
    builder->setUnsafeAsync(); // 设置异步类型为不安全 , 缓冲区会无限扩容

    builder->build();

    //bench("async_logger" , 1 , 1000000 , 100); //sofe mode:6.68257s    6.44771 s
    bench("async_logger" , 4 , 1000000 , 100); //sofe mode:6.68257s    6.44771 s
}


int main()
{
    //sync_bench();
    async_bench();
}
