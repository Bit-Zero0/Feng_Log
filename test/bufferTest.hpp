#include <fstream>
#include <iostream>
#include <string>
#include <cerrno>
#include <cstring>
#include <unistd.h>

#include "../logs/buffer.hpp"
#include "../logs/util.hpp"


void test_buffer()
{
    // 获取当前工作目录
    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        std::cout << "当前工作目录: " << cwd << std::endl;
    }

    // 确保目录存在
    std::string test_dir = "./test_logs";
    FengLog::Util::File::create_directory(test_dir);
    
    // 先创建并写入测试文件
    std::string test_file = test_dir + "/bufferTest.log";
    {
        std::ofstream create_file(test_file, std::ios::binary);
        if(create_file.is_open()) {
            create_file << "测试数据123456789" << std::endl;
            create_file.close();
            std::cout << "成功创建测试文件: " << test_file << std::endl;
        } else {
            std::cout << "创建测试文件失败: " << test_file << " - " << strerror(errno) << std::endl;
            return;
        }
    }
    
    // 读取文件
    std::ifstream ifs(test_file, std::ios::binary);

    if(!ifs.is_open())
    {
        std::cout << "文件打开失败: " << test_file << " - " << strerror(errno) << std::endl;
        return;
    }

    ifs.seekg(0, std::ios::end);
    size_t size = ifs.tellg();
    ifs.seekg(0, std::ios::beg);

    if (size == 0) {
        std::cout << "文件为空" << std::endl;
        return;
    }

    std::cout << "文件大小: " << size << " 字节" << std::endl;

    std::string data(size, '\0');
    ifs.read(&data[0], size);
    if(!ifs.good())
    {
        std::cout << "文件读取失败" << std::endl;
        return;
    }
    ifs.close();

    FengLog::Buffer buffer;
    for(int i = 0; i < data.size(); i++)
    {
        buffer.push(&data[i], 1);
    }
    std::cout << "Buffer 可读大小: " << buffer.readable_size() << std::endl;

    std::ofstream ofs(test_dir + "/bufferTemp.log", std::ios::binary);
    if (!ofs.is_open()) {
        std::cout << "输出文件打开失败: " << strerror(errno) << std::endl;
        return;
    }
    
    int buffer_size = buffer.readable_size();

    // 逐字节写入
    // for(int i = 0; i < buffer_size; i++)
    // {
    //     ofs.write(buffer.begin(), 1);
    //     if(!ofs.good())
    //     {
    //         std::cout << "文件写入失败" << std::endl;
    //         return;
    //     }
    //     buffer.move_read_index(1);
    // }

    // 直接写入
    ofs.write(buffer.begin() , data.size());

    ofs.close();
    std::cout << "成功完成测试" << std::endl;
}