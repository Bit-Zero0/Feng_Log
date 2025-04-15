#pragma once
#include <sys/stat.h>
#include <ctime>
#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include <cassert>
#include <memory>


namespace FengLog{
    namespace Util{
        class Date{
        public:
            static size_t get_time()
            {
                return (size_t)time(nullptr);
            }
        };

        class File{
        public:
            static bool exists(const std::string& name){
                struct stat st;
                return stat(name.c_str() , &st) == 0;
            }

            static std::string path(std::string& name)
            {
                if(name.empty()) return ".";
                int pos = name.find_last_of("/\\");
                if(pos == std::string::npos) return ".";
                return name.substr(0 , pos + 1);
            }

            static void create_directory(const std::string& path)
            {
                if(exists(path))return ;
                if(path.empty()) return ;

                int pos = 0 , index = 0;
                while(index < path.size())
                {
                    pos = path.find_first_of("/\\" , index);
                    if(pos == std::string::npos)
                    {
                        mkdir(path.c_str() , 755);
                        return;
                    }

                    if(index == pos)
                    {
                        index = pos + 1;
                        continue;
                    }
                    std::string parent_path = path.substr(0 , pos);
                    if(parent_path == "." || parent_path == "..")
                    {
                        index = pos + 1;
                        continue;
                    }
                    if(exists(parent_path))
                    {
                        index = pos + 1;
                        continue;
                    }
                    mkdir(parent_path.c_str() , 0755);
                    index = pos + 1;
                }

            }
        };
    }
}