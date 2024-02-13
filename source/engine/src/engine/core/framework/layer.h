#pragma once

namespace vct {

class Layer {
public:
    Layer(const std::string& name = "Layer") : m_name(name) {}

    virtual void attach() = 0;
    virtual void render() = 0;
    virtual void update(float dt) = 0;

    const std::string& get_name() const { return m_name; }

protected:
    std::string m_name;
};

}  // namespace vct