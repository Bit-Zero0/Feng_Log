#pragma once

#include <vector>
#include <iostream>
#include <cassert>

namespace FengLog{

#define BUFFER_DEFAULT_SIZE (10 * 1024 * 1024)
#define BUFFER_THRESHOLD (80 * 1024 * 1024)
#define BUFFER_INCREMENT_SIZE (10 * 1024 * 1024)

class Buffer{

private:
    size_t _reader_index;
    size_t _writer_index;
    std::vector<char> _buffer;

public:
    Buffer()
        :_reader_index(0)
        ,_writer_index(0)
        ,_buffer(BUFFER_DEFAULT_SIZE)
    {}

    bool empty() const {
        return _buffer.empty() || _reader_index == _writer_index;
    }

    size_t readable_size() const{
        return _writer_index - _reader_index;
    }

    size_t writable_size() const{
        return _buffer.size() - _writer_index;
    }

    void reset()
    {
        _reader_index = 0;
        _writer_index = 0;
    }

    const char* begin()
    {
        return &_buffer[_reader_index];
    }

    void swap(Buffer& buffer)
    {
        _buffer.swap(buffer._buffer);
        std::swap(_reader_index, buffer._reader_index);
        std::swap(_writer_index, buffer._writer_index);
    }

    void push(const char* data , size_t len)
    {
        assert(len <= writable_size());
        has_enough_space(len);

        std::copy(data , data + len , _buffer.begin() + _writer_index);
        _writer_index += len;
    }

    void pop(size_t len)
    {
        _reader_index += len;
        assert(_reader_index <= _writer_index);
    }

    void move_read_index(size_t len)
    {
        _reader_index += len;
        assert(_reader_index <= _writer_index);
    }

    void move_write_index(size_t len)
    {
        assert(_writer_index + len <= _buffer.size());
        _writer_index += len;
    }

private:
    void has_enough_space(size_t len)
    {
        if(len <= writable_size())        
            return;
        
        size_t new_capacity;
        if(_buffer.size() < BUFFER_THRESHOLD)
            new_capacity = _buffer.size() * 2 + len;
        else
            new_capacity = _buffer.size() + BUFFER_INCREMENT_SIZE + len;
        
        _buffer.resize(new_capacity);
    }



};


}