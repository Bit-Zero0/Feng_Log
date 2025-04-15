#pragma once

#include <string>
#include <vector>
#include <cstdarg>
#include <mutex>
#include <type_traits>


#include "util.hpp"
#include "level.hpp"
#include "sink.hpp"
#include "formatter.hpp"

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
            msg = "格式化日志信息失败";
        }
        else
        {
            msg.assign(buffer , len);
            free(buffer);
        }



    }

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
         std::cout << LogLevel::to_string(limit_level) << " 同步日志器: " << logger_name << "创建成功...\n";
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

 enum class LoggerType{
    LOGGER_SYNC,
    LOGGER_ASYNC
};

class LoggerBuilder{
protected:
    LoggerType _logger_type;
    std::string _logger_name;
    LogLevel::value _limit_level;
    Formatter::ptr _formatter;
    std::vector<LogSink::ptr> _sinks;

public:
    virtual Logger::ptr build() = 0;

    LoggerBuilder(std::shared_ptr<Formatter> formatter = std::make_shared<Formatter>())
        :_logger_type(LoggerType::LOGGER_SYNC)
        ,_limit_level(LogLevel::value::DEBUG)
        ,_formatter(formatter)
    {}

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
            std::cout << "日志器名称不能为空！！";
            abort();
        }

         if (_sinks.empty()) {
            std::cout << "当前日志器：" << _logger_name << " 未检测到落地方向，默认设置为标准输出!\n";
            _sinks.push_back(std::make_shared<StdOutSink>());
        }

        if(_logger_type == LoggerType::LOGGER_SYNC)
        {
            return std::make_shared<SyncLogger>(_logger_name , _formatter  , _sinks , _limit_level);
        }
    }
}; 


};