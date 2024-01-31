#include "core/io/print.h"

#include "core/os/os.h"

namespace vct {

class TestLogger : public ILogger {
public:
    virtual void print(LogLevel, std::string_view message) override { m_buffer.append(message); }
    const std::string& get_buffer() const { return m_buffer; }
    void clear_buffer() { m_buffer.clear(); }

private:
    std::string m_buffer;
};

TEST(print, print_impl) {
    OS dummy_os;
    auto logger = std::make_shared<TestLogger>();
    dummy_os.add_logger(logger);

    print_impl(LOG_LEVEL_ERROR, "{}, {}, {}", 1, 'c', "200");
    EXPECT_EQ(logger->get_buffer(), "1, c, 200");
}

}  // namespace vct
