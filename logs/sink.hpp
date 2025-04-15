#pragma once

#include "util.hpp"


namespace FengLog{
    
class LogSink{
public:
    using ptr = std::shared_ptr<LogSink>;

    LogSink(){}
    virtual ~LogSink(){}
    virtual void log(const char* data , size_t len) = 0;

};

class StdOutSink : public LogSink
{
public:
    using ptr = std::shared_ptr<StdOutSink>;
    StdOutSink() = default;
    void log(const char* data , size_t len){
        std::cout.write(data , len);
    }
};

class FileSink :public LogSink
{
public:
    using ptr = std::shared_ptr<FileSink>;

    FileSink(const std::string& filename)
        :_filename(filename)
    {
        Util::File::create_directory(Util::File::path(_filename));
        _ofs.open(_filename , std::ios::binary | std::ios::app);
        assert(_ofs.is_open());
    }

    void log(const char* data , size_t len)
    {
        _ofs.write(data , len);
        if(_ofs.good() == false)
        {
            std::cout << "日志输出文件 \n";
        }
    }

    const std::string& file()
    {
        return _filename;
    }

private:
    std::string _filename;
    std::ofstream _ofs;
};

class RollSink : public LogSink
{
public:
    RollSink(const std::string& basename , size_t max_file_size)
        :_basename(basename)
        ,_max_file_size(max_file_size)
        ,_cur_file_size(0)
        ,_file_index(0)
    {
        Util::File::create_directory(Util::File::path(_basename));
    }

    void log(const char* data , size_t len)
    {
        if( !_ofs.is_open() || _cur_file_size >= _max_file_size)
        {
            _ofs.close();
            std::string name = createFilename();
            _ofs.open(name , std::ios::binary | std::ios::app);
            assert(_ofs.is_open());
            _cur_file_size = 0;
        }

        _ofs.write(data , len);
        if(!_ofs.good())
        {
            std::cout << "日志输出失败!!" << std::endl;
        }
        _cur_file_size += len;
    }

private:

    std::string createFilename()
    {
        time_t t = time(nullptr);
        struct tm lt;
        localtime_r(&t , &lt);
        std::stringstream ss;
        ss << _basename;
        ss << lt.tm_year + 1900;
        ss << lt.tm_mon + 1;
        ss << lt.tm_mday;
        ss << lt.tm_hour;
        ss << lt.tm_min;
        ss << lt.tm_sec;
        ss << "_" << _file_index++;
        ss << ".log";
        return ss.str();
    }
    

private:
    size_t _file_index;
    std::string _basename;
    std::ofstream _ofs;
    size_t _max_file_size;
    size_t _cur_file_size;
};

enum class TimeGap{
    GAP_DAY,
    GAP_HOUR,
    GAP_MINUTE,
    GAP_SECOND
};

class TimeRollSink : public LogSink
{
public:
    TimeRollSink(const std::string& basename , TimeGap gap)
        :_basename(basename)
        ,_file_index(0)
    {
        switch(gap)
        {
            case TimeGap::GAP_DAY: _gap_size = 3600 * 24; break;
            case TimeGap::GAP_HOUR: _gap_size = 3600; break;
            case TimeGap::GAP_MINUTE: _gap_size = 60; break;
            case TimeGap::GAP_SECOND: _gap_size = 1; break;
        }

        _cur_gap_size = _gap_size == 1 ? Util::Date::get_time() : Util::Date::get_time()  % _gap_size; //获取当前是哪个时间间隔
        std::string filename = createFilename();
        FengLog::Util::File::create_directory(FengLog::Util::File::path(filename));
        _ofs.open(filename , std::ios::binary | std::ios::app);
        assert(_ofs.is_open());
    }

    void log(const char* data , size_t len)
    {
        time_t cur = Util::Date::get_time();
        time_t cur_gap = _gap_size == 1 ? cur : cur % _gap_size;  // 修正时间间隔计算
        
        if(_ofs.is_open() && cur_gap != _cur_gap_size)
        {
            _ofs.close();
            std::string filename = createFilename();
            _ofs.open(filename , std::ios::binary | std::ios::app);
            assert(_ofs.is_open());
            _cur_gap_size = cur_gap;
        }

        _ofs.write(data, len);
        assert(_ofs.good());
    }

private:
    std::string createFilename()
    {
        time_t t = time(nullptr);
        struct tm lt;
        localtime_r(&t , &lt);
        std::stringstream ss;
        ss << _basename;
        ss << lt.tm_year + 1900;
        ss << lt.tm_mon + 1;
        ss << lt.tm_mday;
        ss << lt.tm_hour;
        ss << lt.tm_min;
        ss << lt.tm_sec;
        ss << "_" << _file_index++;
        ss << ".log";
        return ss.str();
    }
    
private:
    std::string _basename;
    size_t _cur_gap_size; //当前是哪个时间间隔
    size_t _gap_size; //时间间隔大小
    std::ofstream _ofs;
    size_t _file_index;
};



class SinkFactory
{
public:
    template<typename SinkType , typename ...Args>
    static LogSink::ptr createSink(Args&& ...args )
    {
        return std::make_shared<SinkType>(std::forward<Args>(args)...);
    }
};


}