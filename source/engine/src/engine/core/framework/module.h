#pragma once

namespace vct {

class Application;

class Module {
public:
    Module(std::string_view name) : m_name(name) {}
    virtual ~Module() = default;

    virtual bool initialize() = 0;
    virtual void finalize() = 0;

    std::string_view get_name() const { return m_name; }

protected:
    std::string_view m_name;
    Application* m_app;
    friend class Application;
};

}  // namespace vct
