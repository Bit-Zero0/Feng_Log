
#include "../logs/util.hpp"
#include "../logs/message.hpp"
#include "../logs/formatter.hpp"
#include "../logs/sink.hpp"
#include "../logs/logger.hpp"

#include "formatTest.hpp"
#include "sinkTest.hpp"
#include "loggerTest.hpp"
// 文件操作测试
void UiltFileTest()
{
    std::string path = "./a/b/c/d.txt";


    if(FengLog::Util::File::exists(path))
    {
        std::cout << "this filepath exist" << std::endl; 
    }
    else{
        std::cout << "this filepath is not exist" << std::endl;
    }
    std::string isPath = FengLog::Util::File::path(path);
    std::cout << isPath << std::endl;

    std::string path2 = "./a/b/d.txt";
    std::string path3 = "./a/d.txt";


    FengLog::Util::File::create_directory(path);
    FengLog::Util::File::create_directory(path2);
    FengLog::Util::File::create_directory(path3);

}


void FormatterTest()
{
        //UiltFileTest();
    //FormatterTest();

    // 添加基本测试
    //BasicFormatterTest();
    
    // 测试各个FormatItem子类
    //FormatItemTest();
    
    // 测试不同格式模式的Formatter
    //FormatterPatternTest();
    
    // 测试格式解析错误情况
    FormatterErrorTest();
}


// 运行所有 Sink 测试
void SinkTests() {
    std::cout << "开始运行 Sink 模块测试..." << std::endl;
    
    // testStdOutSink();
    // testFileSink();
    // testRollSink();
    testTimeRollSink();

    std::cout << "Sink 模块测试完成..." << std::endl;

}


void LoggerTest()
{
    //test_sync_logger();

    test_Local_builder_logger();
}


int main()
{
    //FormatterTest();
    //SinkTests();
    LoggerTest();
    return 0;
}
