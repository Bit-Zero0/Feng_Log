#pragma once

#include "message.hpp"
#include "level.hpp"
#include "util.hpp"

#include <tuple>
#include <vector>
#include <iostream>
#include <sstream>


namespace FengLog{

class FormatItem{
public:
    using ptr = std::shared_ptr<FormatItem>;

    virtual ~FormatItem(){}

    virtual void format(std::ostream& os , const LogMsg& msg) = 0;

};

class LevelFormatItem : public FormatItem{
public:
    LevelFormatItem(const std::string& str = ""){
    }
    void format(std::ostream& os , const LogMsg& msg) override{
        os << LogLevel::to_string(msg._level);
    }
};

class MsgFormatItem : public FormatItem{
public:
    MsgFormatItem(const std::string& str = ""){}

    void format(std::ostream& os , const LogMsg& msg) override{
        os << msg._payload;
    }
};

class NameFormatItem : public FormatItem{
public:
    NameFormatItem(const std::string& str = ""){}
    void format(std::ostream& os , const LogMsg& msg) override{
        os << msg._logger;
    }
};

class ThreadFormatItem : public FormatItem{
public:
    ThreadFormatItem(const std::string& str = ""){}
    void format(std::ostream& os , const LogMsg& msg) override{
        os << msg._tid;
    }
};

class LineFormatItem : public FormatItem{
public:
    LineFormatItem(const std::string& str = ""){}
    virtual void format(std::ostream& os , const LogMsg& msg) override{
        os << msg._line;
    }
};

class TimeFormatItem : public FormatItem{
private:
    std::string _TimeFormat;
public:
    TimeFormatItem(const std::string& TimeFormat = "%H:%M:%S"):_TimeFormat(TimeFormat)
    {
        if(TimeFormat.empty()){
            _TimeFormat = "%H:%M:%S";
        }
    }

    void format(std::ostream& os , const LogMsg& msg) override{
        time_t t = msg._time;
        struct tm* localTime = localtime(&t);
        char buf[128];
        strftime(buf , sizeof(buf) , _TimeFormat.c_str() , localTime);
        os << buf;
    }
};

class CFileFormatItem : public FormatItem{
public:
    CFileFormatItem(const std::string& str = ""){}

    void format(std::ostream& os , const LogMsg& msg) override{
        os << msg._filename;
    }
};


class TabFormatItem : public FormatItem{
public:
    TabFormatItem(const std::string& str = ""){}

    void format(std::ostream& os , const LogMsg& msg) override{
        os << '\t';
    }
};



class NewLineFormatItem : public FormatItem{
public:
    NewLineFormatItem(const std::string& str = ""){}
    void format(std::ostream& os , const LogMsg& msg) override{
        os << '\n';
    }
};

class OtherFormatItem : public FormatItem{
private:
    std::string _str;
public:
    OtherFormatItem(const std::string& str = "") : _str(str){}

    void format(std::ostream& os , const LogMsg& msg) override{
        os << _str;
    }
};

class Formatter{
private:
    std::string _pattern;
    std::vector<FormatItem::ptr> _items;
    bool _error;  // 添加错误标志

public: 
    using ptr = std::shared_ptr<Formatter>;
public:
    Formatter(const std::string& pattern = "[%d{%H:%M:%S}][%t][%p][%c][%f:%l] %m%n")
        :_pattern(pattern), _error(false)
    {
        if (!parsePattern()) {
            _error = true;
            // 使用一个简单的默认格式作为回退
            _items.clear();
            _items.push_back(FormatItem::ptr(new OtherFormatItem("格式错误: ")));
            _items.push_back(FormatItem::ptr(new OtherFormatItem(_pattern)));
        }
    }



    FormatItem::ptr createFormatItem(const std::string fc , const std::string& subFmt)
    {
        if (fc == "m") return FormatItem::ptr(new MsgFormatItem(subFmt));
        if (fc == "p") return FormatItem::ptr(new LevelFormatItem(subFmt));
        if (fc == "c") return FormatItem::ptr(new NameFormatItem(subFmt));
        if (fc == "t") return FormatItem::ptr(new ThreadFormatItem(subFmt));
        if (fc == "n") return FormatItem::ptr(new NewLineFormatItem(subFmt));
        if (fc == "d") return FormatItem::ptr(new TimeFormatItem(subFmt));
        if (fc == "f") return FormatItem::ptr(new CFileFormatItem(subFmt));
        if (fc == "l") return FormatItem::ptr(new LineFormatItem(subFmt));
        if (fc == "T") return FormatItem::ptr(new TabFormatItem(subFmt));
        return FormatItem::ptr();
    }


    const std::string format(const LogMsg& msg)
    {
        std::stringstream ss;
        for(auto& it : _items)
        {
            it->format(ss , msg);
        }
        return ss.str();
    }

    std::ostream& format(std::ostream& os ,  const LogMsg& msg)
    {
        for(auto &it : _items)
        {
            it->format(os , msg);
        }
        return os;
    }

private:
    bool parsePattern() 
    {
        //std::string _pattern = "sg{}fsg%d{%H:%M:%S}%Tsdf%t%T[%p]%T[%c]%T%f:%l%T%m%n  ";
        //std::cout << _pattern << std::endl;
        //每个要素分为三部分：
        //  格式化字符  : %d  %T  %p...
        //  对应的输出子格式 ： {%H:%M:%S}
        //  对应数据的类型 ： 0-表示原始字符串，也就是非格式化字符，1-表示格式化数据类型
        //  默认格式 "%d{%H:%M:%S}%T%t%T[%p]%T[%c]%T%f:%l%T%m%n"
        std::vector<std::tuple<std::string, std::string, int>> arry;
        std::string format_key;//存放%后的格式化字符
        std::string format_val;//存放格式化字符后边 {} 中的子格式字符串
        std::string string_row;//存放原始的非格式化字符
        bool sub_format_error = false;
        int pos = 0;
        while (pos < _pattern.size()) {
            if (_pattern[pos] != '%') {
                string_row.append(1, _pattern[pos++]);
                continue;
            }
            if (pos+1 < _pattern.size() && _pattern[pos+1] == '%') {
                string_row.append(1, '%');
                pos += 2;
                continue;
            }
            if (string_row.empty() == false) {
                arry.push_back(std::make_tuple(string_row, "", 0));
                string_row.clear();
            }
            //当前位置是%字符位置
            pos += 1;//pos指向格式化字符位置
            if (pos < _pattern.size() && isalpha(_pattern[pos])) {
                format_key = _pattern[pos];//保存格式化字符
            }else {
                std::cout << &_pattern[pos-1] << "位置附近格式错误！\n";
                return false;
            }

            pos += 1;//pos指向格式化字符的下一个位置，判断是否包含有子格式 %d{%Y-%m-%d}
            if (pos < _pattern.size() && _pattern[pos] == '{') {
                sub_format_error = true;
                pos += 1;//pos指向花括号下一个字符处
                while(pos < _pattern.size()) {
                    if (_pattern[pos] == '}') {
                        sub_format_error = false;
                        pos += 1;//让pos指向}的下一个字符处
                        break;
                    }
                    format_val.append(1, _pattern[pos++]);
                }
            }
            arry.push_back(std::make_tuple(format_key, format_val, 1));
            format_key.clear();
            format_val.clear();
        }
        if (sub_format_error) {
            std::cout << "{}对应出错\n";
            return false;
        }
        if (string_row.empty() == false) arry.push_back(std::make_tuple(string_row, "", 0));
        if (format_key.empty() == false) arry.push_back(std::make_tuple(format_key, format_val, 1));
        for (auto &it : arry) {
            if (std::get<2>(it) == 0) {
                FormatItem::ptr fi(new OtherFormatItem(std::get<0>(it)));
                _items.push_back(fi);
            }else {
                FormatItem::ptr fi = createFormatItem(std::get<0>(it), std::get<1>(it));
                if (fi.get() == nullptr) {
                    std::cout << "没有对应的格式化字符: %" <<  std::get<0>(it) << std::endl;
                    return false;
                }
                _items.push_back(fi);
            }
        }
        return true;
    }


};
}