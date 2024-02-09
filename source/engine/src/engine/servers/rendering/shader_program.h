#pragma once

namespace vct {

struct ProgramCreateInfo {
    std::string_view vs;
    std::string_view ps;
    std::string_view gs;
    std::string_view cs;
};

class ShaderProgram {
public:
    void bind() const;
    void unbind() const;

private:
    uint32_t m_handle = 0;

    friend class ShaderProgramManager;
};

}  // namespace vct
