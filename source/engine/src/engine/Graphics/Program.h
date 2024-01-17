#pragma once
struct ProgramCreateInfo
{
    std::string vs;
    std::string ps;
    std::string gs;
    std::string cs;
};

class Program
{
public:
    void Bind() const;
    void Unbind() const;

private:
    uint32_t mHandle = 0;

    friend class ProgramManager;
};
