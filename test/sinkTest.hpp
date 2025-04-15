#pragma once

#include "../logs/sink.hpp"
#include "../logs/formatter.hpp"
#include <cstring>
#include <unistd.h>

// 测试 StdOutSink
void testStdOutSink() {
    std::cout << "\n===== 测试 StdOutSink =====" << std::endl;
    
    // 创建 StdOutSink 实例
    FengLog::StdOutSink::ptr sink = std::make_shared<FengLog::StdOutSink>();
    
    // 测试日志输出
    const char* testMsg = "这是一条测试日志消息\n";
    std::cout << "输出测试消息: ";
    sink->log(testMsg, strlen(testMsg));
    
    std::cout << "===== StdOutSink 测试完成 =====" << std::endl;
}

// 测试 FileSink
void testFileSink() {
    std::cout << "\n===== 测试 FileSink =====" << std::endl;
    
    // 创建测试文件路径
    std::string testFilePath = "./test_logs/file_sink_test.log";
    
    // 确保测试目录存在
    FengLog::Util::File::create_directory(FengLog::Util::File::path(testFilePath));
    
    // 删除可能存在的旧测试文件
    // if (FengLog::Util::File::exists(testFilePath)) {
    //     std::remove(testFilePath.c_str());
    // }
    
    // 创建 FileSink 实例
    FengLog::FileSink::ptr sink = std::make_shared<FengLog::FileSink>(testFilePath);
    
    // 测试日志输出
    const char* testMsg = "这是写入文件的测试日志消息\n";
    sink->log(testMsg, strlen(testMsg));
    
    // 验证文件是否创建并包含正确内容
    std::ifstream file(testFilePath);
    if (file.is_open()) {
        std::string content;
        std::getline(file, content);
        file.close();
        
        std::cout << "文件创建成功: " << testFilePath << std::endl;
        std::cout << "文件内容: " << content << std::endl;
    } else {
        std::cout << "错误: 无法打开文件进行验证" << std::endl;
    }
    
    std::cout << "===== FileSink 测试完成 =====" << std::endl;
}

// 测试 RollSink
void testRollSink() {
    std::cout << "\n===== 测试 RollSink =====" << std::endl;
    
    FengLog::LogMsg msg(FengLog::LogLevel::value::INFO, 66 , "main.cpp" , "root" , "this is a SinkTest log");
    FengLog::Formatter fmt;
    std::string str = fmt.format(msg);

    FengLog::LogSink::ptr rollSink = FengLog::SinkFactory::createSink<FengLog::RollSink>("./test_logs/roll-", 1024*1024);
    rollSink->log(str.c_str() , str.size());

    size_t curSize = 0;
    size_t count = 0;
    while(curSize < 1024 * 1024 *10)
    {
        std::string temp = str + std::to_string(count++);
        rollSink->log(temp.c_str() , temp.size());
        curSize += temp.size();
    }
    std::cout << "===== RollSink 测试完成 =====" << std::endl;
}


void testTimeRollSink()
{
    FengLog::LogMsg msg(FengLog::LogLevel::value::INFO, 66 , "main.cpp" , "root" , "this is a TimeRollSink log");

    FengLog::Formatter fmt;
    std::string str = fmt.format(msg);

    FengLog::LogSink::ptr timeRollSink = FengLog::SinkFactory::createSink<FengLog::TimeRollSink>("./test_logs/time_roll-" , FengLog::TimeGap::GAP_SECOND);

    time_t old_time = FengLog::Util::Date::get_time();
    while(FengLog::Util::Date::get_time() < old_time + 5)
    {
        timeRollSink->log(str.c_str() , str.size() );
        usleep(1000);
    }

}




