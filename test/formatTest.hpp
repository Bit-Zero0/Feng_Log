#include "../logs/formatter.hpp"

//测试各个FormatItem子类
void FormatItemTest() {
    std::cout << "===== 开始测试各个FormatItem子类 =====" << std::endl;
    
    // 创建一个测试用的LogMsg对象
    FengLog::LogMsg msg(
        FengLog::LogLevel::value::INFO,
        123,
        "test_file.cpp",
        "TestLogger",
        "这是一条测试消息"
    );
    
    std::stringstream ss;
    
    // 测试LevelFormatItem
    {
        FengLog::LevelFormatItem item;
        ss.str("");
        item.format(ss, msg);
        std::cout << "LevelFormatItem: " << ss.str() << std::endl;
    }
    
    // 测试MsgFormatItem
    {
        FengLog::MsgFormatItem item;
        ss.str("");
        item.format(ss, msg);
        std::cout << "MsgFormatItem: " << ss.str() << std::endl;
    }
    
    // 测试NameFormatItem
    {
        FengLog::NameFormatItem item;
        ss.str("");
        item.format(ss, msg);
        std::cout << "NameFormatItem: " << ss.str() << std::endl;
    }
    
    // 测试ThreadFormatItem
    {
        FengLog::ThreadFormatItem item;
        ss.str("");
        item.format(ss, msg);
        std::cout << "ThreadFormatItem: " << ss.str() << std::endl;
    }
    
    // 测试LineFormatItem
    {
        FengLog::LineFormatItem item;
        ss.str("");
        item.format(ss, msg);
        std::cout << "LineFormatItem: " << ss.str() << std::endl;
    }
    
    // 测试TimeFormatItem（默认格式）
    {
        FengLog::TimeFormatItem item;
        ss.str("");
        item.format(ss, msg);
        std::cout << "TimeFormatItem(默认): " << ss.str() << std::endl;
    }
    
    // 测试TimeFormatItem（自定义格式）
    {
        FengLog::TimeFormatItem item("%Y-%m-%d %H:%M:%S");
        ss.str("");
        item.format(ss, msg);
        std::cout << "TimeFormatItem(自定义): " << ss.str() << std::endl;
    }
    
    // 测试CFileFormatItem
    {
        FengLog::CFileFormatItem item;
        ss.str("");
        item.format(ss, msg);
        std::cout << "CFileFormatItem: " << ss.str() << std::endl;
    }
    
    // 测试TabFormatItem
    {
        FengLog::TabFormatItem item;
        ss.str("");
        item.format(ss, msg);
        std::cout << "TabFormatItem: '" << ss.str() << "'" << std::endl;
    }
    
    // 测试NewLineFormatItem
    {
        FengLog::NewLineFormatItem item;
        ss.str("");
        item.format(ss, msg);
        std::cout << "NewLineFormatItem: '" << ss.str() << "'" << std::endl;
    }
    
    // 测试OtherFormatItem
    {
        FengLog::OtherFormatItem item("自定义文本");
        ss.str("");
        item.format(ss, msg);
        std::cout << "OtherFormatItem: " << ss.str() << std::endl;
    }
    
    std::cout << "===== FormatItem子类测试完成 =====\n\n" << std::endl;
}

// 测试不同格式模式的Formatter
void FormatterPatternTest() {
    std::cout << "\n===== 开始测试不同格式模式的Formatter =====" << std::endl;
    
    // 创建一个测试用的LogMsg对象
    FengLog::LogMsg msg(
        FengLog::LogLevel::value::DEBUG,
        456,
        "formatter_test.cpp",
        "PatternLogger",
        "测试不同的格式模式"
    );
    
    // 测试默认格式
    {
        FengLog::Formatter fmt;
        std::string result = fmt.format(msg);
        std::cout << "默认格式: " << result << std::endl;
    }
    
    // 测试简单格式
    {
        FengLog::Formatter fmt("[%p] %m%n");
        std::string result = fmt.format(msg);
        std::cout << "简单格式 [%p] %m%n: " << result << std::endl;
    }
    
    // 测试复杂格式
    {
        FengLog::Formatter fmt("[时间:%d{%Y-%m-%d %H:%M:%S}] [线程:%t] [级别:%p] [日志器:%c] [文件:%f:%l]%n%T消息内容: %m%n");
        std::string result = fmt.format(msg);
        std::cout << "复杂格式: " << result << std::endl;
    }
    
    // 测试重复格式项
    {
        FengLog::Formatter fmt("%p - %m - %p%n");
        std::string result = fmt.format(msg);
        std::cout << "重复格式项 %p - %m - %p%n: " << result << std::endl;
    }
    
    // 测试转义字符 %%
    {
        FengLog::Formatter fmt("百分比符号 %% [%p] %m");
        std::string result = fmt.format(msg);
        std::cout << "转义字符 %% [%p] %m: " << result << std::endl;
    }
    
    std::cout << "===== Formatter格式模式测试完成 =====\n\n" << std::endl;
}



// 添加一个简单的测试，确保基本功能正常
void BasicFormatterTest() {
    std::cout << "\n===== 基本格式化测试 =====" << std::endl;
    
    FengLog::LogMsg msg(FengLog::LogLevel::value::INFO, 66, "main.c", "root", "Formatter格式化功能测试");
    FengLog::Formatter fmt;
    std::string str = fmt.format(msg);
    
    std::cout << "默认格式输出: " << str << std::endl;
    
    // 测试一个简单的自定义格式
    FengLog::Formatter customFmt("[%p] %m");
    std::string customStr = customFmt.format(msg);
    
    std::cout << "自定义格式 [%p] %m 输出: " << customStr << std::endl;
    
    std::cout << "===== 基本格式化测试完成 =====\n" << std::endl;
}

// 测试格式解析错误情况
void FormatterErrorTest() {
    std::cout << "\n===== 开始测试Formatter错误处理 =====\n\n" << std::endl;
    
    // 测试无效的格式说明符
    {
        std::cout << "尝试使用无效格式说明符 %z:" << std::endl;
        FengLog::Formatter fmt("%z");  // z不是有效的格式说明符
        
        // 尝试使用这个格式化器
        FengLog::LogMsg msg(FengLog::LogLevel::value::INFO, 1, "test.cpp", "test", "test");
        std::string result = fmt.format(msg);
        std::cout << "  格式化结果: " << result <<"\n\n" << std::endl;
    }
    
    // 测试未闭合的大括号
    {
        std::cout << "尝试使用未闭合的大括号 %d{YYYY-MM-DD:" << std::endl;
        FengLog::Formatter fmt("%d{YYYY-MM-DD");  // 缺少闭合的 }
        
        // 检查是否有错误        
        // 尝试使用这个格式化器
        FengLog::LogMsg msg(FengLog::LogLevel::value::INFO, 1, "test.cpp", "test", "test");
        std::string result = fmt.format(msg);
        std::cout << "  格式化结果: " << result << std::endl;
    }
    
    std::cout << "===== Formatter错误处理测试完成 ====\n\n=" << std::endl;
}