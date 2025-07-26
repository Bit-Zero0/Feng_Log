#pragma once

#include <string>
#include <vector>
#include <cstdarg>
#include <mutex>
#include <type_traits>
#include <unordered_map>
#include <thread>
#include <cstring>


#include "util.hpp"
#include "level.hpp"
#include "sink.hpp"
#include "formatter.hpp"
#include "looper.hpp"

namespace FengLog{

class Logger{
protected:
    std::string _logger_name;
    LogLevel::value _limit_level;
    std::vector<LogSink::ptr> _sinks;
    Formatter::ptr _formatter;
    std::mutex _mutex;

protected:
    virtual void log(const char* data , size_t len) = 0;

public:
    using ptr = std::shared_ptr<Logger>;

    Logger(const std::string& logger_name  ,
        Formatter::ptr formatter,
        std::vector<LogSink::ptr>& sinks , 
        LogLevel::value level = LogLevel::value::DEBUG 
    )
        :_logger_name(logger_name) 
        ,_limit_level(level)
        ,_formatter(formatter)
        ,_sinks(sinks.begin() , sinks.end())
    {};

    std::string name() const
    {
        return _logger_name;
    }


    LogLevel::value loggerLevel() const
    {
        return _limit_level;
    }

    void debug(const char* file , size_t line , const std::string& fmt , ...)
    {
        if(false == isEnable(LogLevel::value::DEBUG))
            return ;

        va_list al;
        va_start(al , fmt);
        char* buffer = nullptr;
        int len = vasprintf(&buffer , fmt.c_str() , al);
        if(len < 0)
        {
            std::cout << "vasprintf failed" << std::endl;
            return ;
        }
        va_end(al);

        serialize(LogLevel::value::DEBUG , file , line , buffer);
        free(buffer);
    }

    void info(const char* file , size_t line , const std::string& fmt , ...)
    {
        if(false == isEnable(LogLevel::value::INFO))
            return ;

        va_list al;
        va_start(al , fmt);
        char* buffer = nullptr;
        int len = vasprintf(&buffer , fmt.c_str() , al);
        if(len < 0)
        {
            std::cout << "vasprintf failed" << std::endl;
            return ;
        }
        va_end(al);

        serialize(LogLevel::value::INFO , file , line , buffer);
        free(buffer);
    }

    void warn(const char* file , size_t line , const std::string& fmt , ...)
    {
        if(false == isEnable(LogLevel::value::WARN))
            return ;

        va_list al;
        va_start(al , fmt);
        char* buffer = nullptr;
        int len = vasprintf(&buffer , fmt.c_str() , al);
        if(len < 0)
        {
            std::cout << "vasprintf failed" << std::endl;
            return ;
        }
        va_end(al);

        serialize(LogLevel::value::WARN , file , line , buffer);
        free(buffer);
    }


    void error(const char* file , size_t line , const std::string& fmt , ...)
    {
        if(false == isEnable(LogLevel::value::ERROR))
            return ;

        va_list al;
        va_start(al , fmt);
        char* buffer = nullptr;
        int len = vasprintf(&buffer , fmt.c_str() , al);
        if(len < 0)
        {
            std::cout << "vasprintf failed" << std::endl;
            return ;
        }
        va_end(al);

        serialize(LogLevel::value::ERROR , file , line , buffer);
        free(buffer);
    }

    void fatal(const char* file , size_t line , const std::string& fmt , ...)
    {
        if(false == isEnable(LogLevel::value::FATAL))
            return ;

        va_list al;
        va_start(al , fmt);
        char* buffer = nullptr;
        int len = vasprintf(&buffer , fmt.c_str() , al);
        if(len < 0)
        {
            std::cout << "vasprintf failed" << std::endl;
            return ;
        }
        va_end(al);

        serialize(LogLevel::value::FATAL , file , line , buffer);
        free(buffer);
    }


protected:
    void serialize(LogLevel::value limit_level , const std::string& file , size_t line , char* str)
    {
        LogMsg log_msg(limit_level , line , file , _logger_name  , str);

        std::stringstream ss;
        _formatter->format(ss , log_msg);

        log(ss.str().c_str() , ss.str().size());
    }

    bool isEnable(LogLevel::value level)
    {
        return level >= _limit_level;
    }

    void log(LogLevel::value level , const char* file , size_t line , const char* fmt , va_list al)
    {
        char* buffer = nullptr;
        std::string msg;
        int len = vasprintf(&buffer , fmt , al);
        if(len < 0)
        {
            msg = "Failed to format log information";
        }
        else
        {
            msg.assign(buffer , len);
            free(buffer);
        }
    }

};

 enum class LoggerType{
    LOGGER_SYNC,
    LOGGER_ASYNC
};

class SyncLogger : public Logger{
public:
    using ptr = std::shared_ptr<SyncLogger>;

    SyncLogger(const std::string& logger_name ,
            Formatter::ptr formatter ,
            std::vector<LogSink::ptr>& sinks ,
            LogLevel::value limit_level = LogLevel::value::DEBUG) 
            : Logger(logger_name , formatter , sinks , limit_level)
    {
         std::cout << LogLevel::to_string(limit_level) << " sync logger: " << logger_name << "created successfully \n";
    }

private:
    virtual void log(const char* data , size_t len) override
    {
        std::unique_lock<std::mutex> lock(_mutex);
        
        if(_sinks.empty()) return ;

        for(auto& sink : _sinks)
        {
            sink->log(data , len);
        }
    }

};



class AsyncLogger : public Logger
{
private:
    AsyncLooper::ptr _looper;  // 保存所有创建的 AsyncLooper
    AsyncType _looper_type;

public:
    AsyncLogger(const std::string& logger_name ,
            Formatter::ptr formatter ,
            std::vector<LogSink::ptr>& sinks ,
            LogLevel::value limit_level = LogLevel::value::DEBUG ,
            AsyncType looper_type = AsyncType::ASYNC_SAFE) 
            : Logger(logger_name , formatter , sinks , limit_level)
            ,_looper(std::make_shared<AsyncLooper>(std::bind(&AsyncLogger::real_log , this, std::placeholders::_1)))
            ,_looper_type(looper_type)
    {
        std::cout << LogLevel::to_string(limit_level) << " async logger: " << logger_name << "created successfully \n";
    }

public:
    //将日志信息推送到缓存区
    void log(const char* data , size_t len) override
    {
        _looper->push(data , len);
    }



    void real_log(Buffer& buffer)
    {
        if(_sinks.empty()) return ;
        for(auto& sink : _sinks)
        {
            sink->log(buffer.begin() , buffer.readable_size());
        }
    }
};



class LoggerBuilder{
protected:
    LoggerType _logger_type;
    std::string _logger_name;
    LogLevel::value _limit_level;
    Formatter::ptr _formatter;
    std::vector<LogSink::ptr> _sinks;
    AsyncType _looper_type;
public:
    virtual Logger::ptr build() = 0;

    LoggerBuilder(std::shared_ptr<Formatter> formatter = std::make_shared<Formatter>() )
        :_logger_type(LoggerType::LOGGER_SYNC)
        ,_limit_level(LogLevel::value::DEBUG)
        ,_formatter(formatter)
        ,_looper_type(AsyncType::ASYNC_SAFE)
    {}

    void buildLooperType(AsyncType type)
    {_looper_type = type;}

    void setUnsafeAsync()
    {_looper_type = AsyncType::ASYNC_UNSAFE;}
    
    void buildLoggerType(LoggerType type)
    {_logger_type = type;}

    void buildLoggerName(const std::string& name)
    {_logger_name = name;}

    void buildLimitLevel(LogLevel::value level)
    {_limit_level = level;}

    void buildFormatter(const Formatter::ptr& formatter)
    {_formatter = formatter;}

    void buildFormatter(const std::string& pattern)
    {_formatter = std::make_shared<Formatter>(pattern);}

    template<typename SinkType , typename ...Args>
    void buildSink(Args&& ...args)
    {
        auto sink = SinkFactory::createSink<SinkType>(std::forward<Args>(args)...);
        _sinks.push_back(sink);
    }

};

class LocalLoggerBuilder : public LoggerBuilder
{
public:
    virtual Logger::ptr build() override
    {
        if (_logger_name.empty()) {
            std::cout << "The logger name cannot be empty!" << std::endl;
            abort();
        }

        if(_formatter.get() == nullptr)
        {
            std::cout << "The current logger: " << _logger_name << "did not detect the log format, default setting is [%d{%H:%M:%S}][%t][%p][%c][%f:%l] %m%n" << std::endl;
            _formatter = std::make_shared<Formatter>();
        }

        if(_sinks.empty())
        {
            std::cout << "L The current logger: " << _logger_name << " did not detect the log destination, default setting is standard output!" << std::endl;
            _sinks.push_back(std::make_shared<StdOutSink>());
        }

        Logger::ptr logger;
        if(_logger_type == LoggerType::LOGGER_SYNC)
        {
            logger =  std::make_shared<SyncLogger>(_logger_name , _formatter  , _sinks , _limit_level);
        }
        else{
            logger =   std::make_shared<AsyncLogger>(_logger_name , _formatter , _sinks , _limit_level , _looper_type);
        }

        return logger;
    }
}; 


// 日志器管理器
// 负责管理所有日志器，并提供单例模式获取实例
class LoggerManager
{
private:
    std::mutex _mutex;
    Logger::ptr _root_ptr; // 默认日志器
    std::unordered_map<std::string , Logger::ptr> _loggers; 

private:
    LoggerManager()
    {
        std::unique_ptr<LocalLoggerBuilder> local_logger_builder(new LocalLoggerBuilder());
        local_logger_builder->buildLoggerName("root");
        local_logger_builder->buildLoggerType(LoggerType::LOGGER_SYNC);
        _root_ptr = local_logger_builder->build();
        _loggers.insert(std::make_pair("root" , _root_ptr));
    }

    LoggerManager(const LoggerManager&) = delete;
    LoggerManager& operator=(const LoggerManager&) = delete;

public:
    // 单例模式获取实例
    static LoggerManager& get_instance()
    {
        static LoggerManager instance;
        return instance;
    }

    // 判断是否存在指定名称的日志器
    bool has_logger(const std::string& logger_name)
    {
        std::unique_lock<std::mutex> lock(_mutex);
        auto it = _loggers.find(logger_name);
        return it != _loggers.end();
    }

    // 添加日志器
    void add_logger(Logger::ptr& logger)
    {
        std::unique_lock<std::mutex> lock(_mutex);
        _loggers[logger->name()] = logger;
    }

    // 获取指定名称的日志器
    Logger::ptr get_logger(const std::string& logger_name)
    {
        std::unique_lock<std::mutex> lock(_mutex);
        auto it = _loggers.find(logger_name);
    
        if(it == _loggers.end())
            return nullptr;
        return it->second;
    }

    // 获取根日志器
    Logger::ptr get_root_logger()
    {
        std::unique_lock<std::mutex> lock(_mutex);
        return _root_ptr;
    }
};

// 全局日志器构建器
// 在局部构建器的基础上, 将日志器添加到单例对象中
class GlobalLoggerBuilder : public LoggerBuilder
{
public:
    virtual Logger::ptr build() override
    {
        if(_logger_name.empty())
        {
            std::cout << "The logger name cannot be empty!" << std::endl;
            abort();
        }

        assert(LoggerManager::get_instance().has_logger(_logger_name) == false);

        if(_formatter.get() == nullptr)
        {
            std::cout << "The current logger: " << _logger_name << "did not detect the log format, default setting is [%d{%H:%M:%S}][%t][%p][%c][%f:%l] %m%n" << std::endl;
            _formatter = std::make_shared<Formatter>();
        }

        if(_sinks.empty())
        {
            std::cout << "G The current logger: " << _logger_name << " did not detect the log destination, default setting is standard output!" << std::endl;
            _sinks.push_back(std::make_shared<StdOutSink>());
        }

        Logger::ptr logger;
        if(_logger_type == LoggerType::LOGGER_SYNC)
        {
            logger = std::make_shared<SyncLogger>(_logger_name , _formatter , _sinks ,_limit_level);
        }
        else{
            logger = std::make_shared<AsyncLogger>(_logger_name , _formatter , _sinks ,_limit_level , _looper_type);
        }

        LoggerManager::get_instance().add_logger(logger);
        return logger;
    }
};


};