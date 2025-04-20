#pragma once

#include <condition_variable>
#include <functional>
#include <mutex>
#include <atomic>
#include <thread>

#include "buffer.hpp"

namespace FengLog{

enum class AsyncType{
    ASYNC_SAFE, // 安全状态, 表示缓冲区满了则阻塞
    ASYNC_UNSAFE // 不安全状态, 表示缓冲区满了则扩容, 用来测试极限性能
};


class AsyncLooper{

public:
    using Task = std::function<void(Buffer&)>;
    using ptr = std::shared_ptr<AsyncLooper>;

    AsyncLooper(const Task& task , AsyncType looper_type = AsyncType::ASYNC_SAFE)
        :_task(task)
        ,_running(true)
        ,_looper_type(looper_type)
        ,_thread(std::thread(&AsyncLooper::work_loop , this))
    {}

    ~AsyncLooper()
    {
        if(_running)
        {
            stop();
        } 
    }

    void stop()
    {
        _running = false;
        _con_cond.notify_all();
        _thread.join();
    }

    void push(const char * data , size_t len)
    {
        std::unique_lock<std::mutex> lock(_mutex);

        // 若缓冲区剩余空间不足，则等待
        if(_looper_type == AsyncType::ASYNC_SAFE)
            _pro_cond.wait(lock , [&](){return _pro_buffer.writable_size() >= len;});
        _pro_buffer.push(data , len);
        _con_cond.notify_one(); //唤醒消费缓存区进行数据处理
    }

private: 
    // 线程入口函数, 对消费缓存区_con_buffer进行消费, 处理完毕后, 将消费缓存区_con_buffer与生产缓存区_pro_buffer进行交换
    void work_loop()
    {
        while(1)
        {
            {
                std::unique_lock<std::mutex> lock(_mutex);

                // 如果线程停止, 并且生产缓存区为空, 则退出, 否则可能会有数据残留在生产缓存区中
                if( !_running && _pro_buffer.empty() )
                    return;

                // 等待生产缓存区有数据, 或者线程停止
                _con_cond.wait(lock , [&]{return !_pro_buffer.empty() || !_running;});

                // 交换生产缓存区与消费缓存区
                _pro_buffer.swap(_con_buffer);

                // 通知生产者, 消费缓存区有数据了
                if(_looper_type == AsyncType::ASYNC_SAFE)
                    _pro_cond.notify_all();
            }

            _task(_con_buffer);
            _con_buffer.reset();
        }
        return ;
    }


private:
    Task _task;
    std::atomic<bool> _running;
    AsyncType _looper_type;
    Buffer _pro_buffer;
    Buffer _con_buffer;
    std::condition_variable _pro_cond;
    std::condition_variable _con_cond;
    std::thread _thread;
    std::mutex _mutex;
};

}