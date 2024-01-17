#pragma once
#include <universal/containers/fixed_string.h>

class Archive
{
public:
    ~Archive()
    {
        Close();
    }

    bool OpenRead(const std::string& path);
    bool OpenWrite(const std::string& path);
    void Close();
    bool IsWriteMode() const;

    template<typename T, class = typename std::enable_if<std::is_pod<T>::value>::type>
    inline Archive& operator<<(const std::vector<T>& vec)
    {
        size_t size = vec.size();
        Write(size);
        for (const T& ele : vec)
        {
            Write(ele);
        }
        return *this;
    }

    template<typename T, class = typename std::enable_if<std::is_pod<T>::value>::type>
    inline Archive& operator>>(std::vector<T>& vec)
    {
        size_t size;
        Read(size);
        vec.resize(size);
        for (size_t i = 0; i < size; ++i)
        {
            Read(&vec[i], sizeof(T));
        }
        return *this;
    }

    inline Archive& operator<<(const std::string& value)
    {
        size_t length = value.length();
        Write(length);
        Write(value.data(), length);
        return *this;
    }

    inline Archive& operator>>(std::string& value)
    {
        size_t stringLength = 0;
        Read(stringLength);
        value.resize(stringLength);
        Read(value.data(), stringLength);
        return *this;
    }

    template<size_t N>
    inline Archive& operator<<(const fixed_string<N>& value)
    {
        size_t length = value.length();
        Write(length);
        Write(value.data(), length);
        return *this;
    }

    template<size_t N>
    inline Archive& operator>>(fixed_string<N>& value)
    {
        size_t length = 0;
        Read(length);
        assert(length + 1 < value.capacity());
        char buffer[N] = { 0 };
        Read(buffer, length);
        value = fixed_string<N>(length, buffer);
        return *this;
    }

    template<typename T, class = typename std::enable_if<std::is_pod<T>::value>::type>
    inline Archive& operator<<(const T& value)
    {
        Write(&value, sizeof(T));
        return *this;
    }

    template<typename T, class = typename std::enable_if<std::is_pod<T>::value>::type>
    inline Archive& operator>>(T& value)
    {
        Read(&value, sizeof(T));
        return *this;
    }

    template<typename T, class = typename std::enable_if<std::is_pod<T>::value>::type>
    bool Write(const T& value)
    {
        return Write(&value, sizeof(value));
    }

    template<typename T, class = typename std::enable_if<std::is_pod<T>::value>::type>
    bool Read(T& value)
    {
        return Read(&value, sizeof(value));
    }

    bool Write(const void* data, size_t bytes);
    bool Read(void* data, size_t bytes);

private:
    enum class EMode : uint8_t
    {
        None,
        Read,
        Write,
    } mMode = EMode::None;

    FILE* mFile = nullptr;
};
