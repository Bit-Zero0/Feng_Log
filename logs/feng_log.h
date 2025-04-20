#pragma once

#include "logger.hpp"

namespace FengLog{

    // 提供获取指定日志器的全局接口
    Logger::ptr get_logger(const std::string& logger_name)
    {   
        return LoggerManager::get_instance().get_logger(logger_name);
    }

    // 提供获取默认日志器的全局接口
    Logger::ptr get_root_logger()
    {
        return LoggerManager::get_instance().get_root_logger();
    }
    
    // 使用宏函数对日志器接口进行代理
    #define debug(fmt, ...) debug(__FILE__, __LINE__, fmt, ##__VA_ARGS__)
    #define info(fmt, ...) info(__FILE__, __LINE__, fmt, ##__VA_ARGS__)
    #define warn(fmt, ...) warn(__FILE__, __LINE__, fmt, ##__VA_ARGS__)
    #define error(fmt, ...) error(__FILE__, __LINE__, fmt, ##__VA_ARGS__)
    #define fatal(fmt, ...) fatal(__FILE__, __LINE__, fmt, ##__VA_ARGS__)

    //提供宏函数, 直接通过默认日志器对日志进行标准输出打印 (不用获取日志器了)
    #define DEBUG(fmt , ...) FengLog::get_root_logger()->debug(fmt, ##__VA_ARGS__)
    #define INFO(fmt , ...) FengLog::get_root_logger()->info(fmt, ##__VA_ARGS__)
    #define WARN(fmt , ...) FengLog::get_root_logger()->warn(fmt, ##__VA_ARGS__)
    #define ERROR(fmt , ...) FengLog::get_root_logger()->error(fmt, ##__VA_ARGS__)
    #define FATAL(fmt , ...) FengLog::get_root_logger()->fatal(fmt, ##__VA_ARGS__)
}


