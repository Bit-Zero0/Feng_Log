#include "../logs/feng_log.h"

#include <chrono>
using namespace FengLog;

void bench(const std::string &loger_name, size_t thread_num, size_t msglen, size_t msg_count)
{
    Logger::ptr lp = get_logger(loger_name);
    if (lp.get() == nullptr) return;
    std::string msg(msglen, '1');
    size_t msg_count_per_thread = msg_count / thread_num;
    std::vector<double> cost_time(thread_num);
    std::vector<std::thread> threads;
    std::cout << "输入线程数量: " << thread_num << std::endl;
    std::cout << "输出日志数量: " << msg_count << std::endl;
    std::cout << "输出日志大小: " << msglen * msg_count / 1024  << "KB" << std::endl;
    for (int i = 0; i < thread_num; i++) {
        threads.emplace_back([&, i](){
            auto start = std::chrono::high_resolution_clock::now();
            for(size_t j = 0; j < msg_count_per_thread; j++) {
                lp->fatal("%s", msg.c_str());
            }
            auto end = std::chrono::high_resolution_clock::now();
            auto cost = std::chrono::duration_cast<std::chrono::duration<double>>(end - start);
            cost_time[i] = cost.count();
            auto avg = msg_count_per_thread / cost_time[i];
            std::cout << "线程" << i << "耗时: " << cost.count() << "s" << " 平均：" << (size_t)avg << "/s\n";
        });
    }
    for(auto &thr : threads) {
        thr.join();
    }
    double max_cost = 0;
    for (auto cost : cost_time) max_cost = max_cost < cost ? cost : max_cost;
    std::cout << "总消耗时间: " << max_cost << std::endl;
    std::cout << "平均每秒输出: " << (size_t)(msg_count / max_cost) << "条日志" << std::endl;
    std::cout << "平均每秒输出: " << (size_t)(msglen * msg_count / max_cost / 1024 / 1024) << "MB" << std::endl;
    std::cout << std::endl;
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

    bench("sync_logger" , 1 , 1000000 , 1000); //3.28715s
    bench("sync_logger" , 4 , 1000000 , 1000); // 1.1265s

    // bench("sync_logger" , 1 , 100 , 1000000); //4.68497 s
    // bench("sync_logger" , 4 , 100 , 1000000); //6.44771 s

}

void async_bench()
{
    std::unique_ptr<GlobalLoggerBuilder> builder(new GlobalLoggerBuilder());
    builder->buildLoggerName("async_logger"); // 设置日志器名称
    builder->buildLimitLevel(FengLog::LogLevel::value::DEBUG);
    builder->buildFormatter("%d{%h:%m:%s}[%c][%f:%l][%p]%T %m%n");
    //builder->buildSink<StdOutSink>();
    builder->buildSink<FileSink>("async_logger.log");
    builder->buildLoggerType(LoggerType::LOGGER_ASYNC); // 设置日志器类型为同步
    //builder->setUnsafeAsync(); // 设置异步类型为不安全 , 缓冲区会无限扩容

    builder->build();


    // bench("async_logger" , 1 , 100 , 1000000); //6.84956 s
    // bench("async_logger" , 4 , 100 , 1000000); //6.68257s  

    bench("async_logger" , 1 , 1000000 , 1000); //4.68497 s
    bench("async_logger" , 4 , 1000000 , 1000); //unsofe mode:1.13709s   safam ode 1.14632 s
}


int main()
{
    //sync_bench();
    async_bench();
}
