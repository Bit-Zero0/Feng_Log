#pragma once
#include <string>

namespace FengLog{
    class LogLevel
    {
    public:
        enum class value{
            UNKNOW = 0,
            DEBUG,
            INFO,
            WARN,
            ERROR,
            FATAL
        };

        static std::string to_string(LogLevel::value val)
        {
            switch(val)
            {
                case LogLevel::value::DEBUG : return "DEBUG";
                case LogLevel::value::INFO : return "INFO";
                case LogLevel::value::WARN : return "WARN";
                case LogLevel::value::ERROR : return "ERROR";
                case LogLevel::value::FATAL : return "FATAL";
            }
            return "UNKNOW";
        }
    };
};