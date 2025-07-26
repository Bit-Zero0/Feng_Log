#pragma once
#include <string>
#include <thread>
#include <ctime>
#include "level.hpp"
#include "util.hpp"
namespace FengLog{


    struct LogMsg{
        //int _line;
        size_t _line;
        size_t _time;
        std::thread::id _tid;
        std::string _logger; 
        std::string _filename;
        std::string _payload;
        LogLevel::value _level;

        LogMsg(LogLevel::value level ,size_t line, std::string filename , std::string logger , std::string msg)
            :_line(line)
            ,_time(Util::Date::get_time()) 
            , _tid(std::this_thread::get_id())
            ,_logger(logger)
            ,_filename(filename)
            ,_payload(msg)
            ,_level(level)
        {}
    };
}