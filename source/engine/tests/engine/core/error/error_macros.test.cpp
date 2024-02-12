#include "core/error/error_macros.h"

#include "core/os/os.h"

namespace vct {

class TestOS : public OS {
public:
    TestOS(std::string* buffer = nullptr) : m_buffer(buffer) {}

    void print(LogLevel, std::string_view message) override {
        if (m_buffer) {
            *m_buffer = message;
        }
    }

    inline static std::string s_buffer;

private:
    std::string* m_buffer;
};

class TestErrorHandler {
public:
    static void error_handler1(void*, std::string_view function, std::string_view file_path, int line,
                               std::string_view error) {
        s_buffer.append(function);
        s_buffer.push_back(',');
        s_buffer.append(file_path);
        s_buffer.push_back(',');
        s_buffer.push_back('a' + static_cast<char>(line));
        s_buffer.push_back(',');
        s_buffer.append(error);
        s_buffer.push_back(';');
    }

    static void error_handler2(void*, std::string_view, std::string_view, int, std::string_view) {
        s_buffer.append("?;");
    }

    static void error_handler3(void*, std::string_view function, std::string_view file_path, int line,
                               std::string_view error) {
        s_buffer.append(error);
        s_buffer.push_back(',');
        s_buffer.push_back('a' + static_cast<char>(line));
        s_buffer.push_back(',');
        s_buffer.append(file_path);
        s_buffer.push_back(',');
        s_buffer.append(function);
        s_buffer.push_back(';');
    }

    static void clear_buffer() { s_buffer.clear(); }

    static const std::string& get_buffer() { return s_buffer; }

private:
    static std::string s_buffer;
};

void assert_handler(void*, std::string_view, std::string_view, int, std::string_view) { exit(99); }

std::string TestErrorHandler::s_buffer;

TEST(error_macros, add_error_handler) {
    TestOS e;

    ErrorHandler handler1;
    handler1.data.error_func = TestErrorHandler::error_handler1;

    add_error_handler(&handler1);
    report_error_impl("a", "b", 2, "d");

    EXPECT_EQ(TestErrorHandler::get_buffer(), "a,b,c,d;");
    remove_error_handler(&handler1);

    TestErrorHandler::clear_buffer();
}

TEST(error_macros, remove_error_handler) {
    TestOS e;

    ErrorHandler handler1;
    handler1.data.error_func = TestErrorHandler::error_handler1;

    ErrorHandler handler2;
    handler2.data.error_func = TestErrorHandler::error_handler2;

    ErrorHandler handler3;
    handler3.data.error_func = TestErrorHandler::error_handler3;

    add_error_handler(&handler1);
    add_error_handler(&handler2);
    add_error_handler(&handler3);

    report_error_impl("a", "b", 2, "d");
    EXPECT_EQ(TestErrorHandler::get_buffer(), "d,c,b,a;?;a,b,c,d;");
    TestErrorHandler::clear_buffer();

    remove_error_handler(&handler2);

    report_error_impl("b", "c", 3, "e");
    EXPECT_EQ(TestErrorHandler::get_buffer(), "e,d,c,b;b,c,d,e;");
    TestErrorHandler::clear_buffer();

    remove_error_handler(&handler1);
    remove_error_handler(&handler3);
}

TEST(error_macros, ERR_FAIL) {
    std::string buffer;
    TestOS e{ &buffer };
    auto func = []() {
        ERR_FAIL();
        return;
    };
    func();

    EXPECT_TRUE(buffer.starts_with(
        R"(ERROR: Method/function failed.
    at)"))
        << buffer;
}

TEST(error_macros, ERR_FAIL_MSG) {
    std::string buffer;
    TestOS e{ &buffer };
    auto func = []() { ERR_FAIL_MSG("Bluh!"); };
    func();

    EXPECT_TRUE(buffer.starts_with(
        R"(ERROR: Method/function failed.
Detail: Bluh!
    at)"))
        << buffer;
}

TEST(error_macros, ERR_FAIL_V) {
    std::string buffer;
    TestOS e{ &buffer };
    auto func = []() { ERR_FAIL_V(10); };
    func();

    EXPECT_TRUE(buffer.starts_with(
        R"(ERROR: Method/function failed. Returning: 10
    at)"))
        << buffer;
}

TEST(error_macros, ERR_FAIL_V_MSG) {
    std::string buffer;
    TestOS e{ &buffer };
    auto func = []() { ERR_FAIL_V_MSG(nullptr, "Bruh..."); };
    func();

    EXPECT_TRUE(buffer.starts_with(
        R"(ERROR: Method/function failed. Returning: nullptr
Detail: Bruh...
    at)"))
        << buffer;
}

TEST(error_macros, ERR_FAIL_COND) {
    std::string buffer;
    TestOS e{ &buffer };
    auto func = [&]() { ERR_FAIL_COND(buffer.size() >= 0); };
    func();

    EXPECT_TRUE(buffer.starts_with(
        R"(ERROR: Condition "buffer.size() >= 0" is true.
    at)"))
        << buffer;
}

TEST(error_macros, ERR_FAIL_COND_MSG) {
    std::string buffer;
    TestOS e{ &buffer };
    auto func = [&]() { ERR_FAIL_COND_MSG(buffer.size() >= 0, "invalid buffer size"); };
    func();

    EXPECT_TRUE(buffer.starts_with(
        R"(ERROR: Condition "buffer.size() >= 0" is true.
Detail: invalid buffer size
    at)"))
        << buffer;
}

TEST(error_macros, ERR_FAIL_COND_V) {
    std::string buffer;
    TestOS e{ &buffer };
    auto func = [&]() {
        ERR_FAIL_COND_V(buffer.size() >= 0, buffer.size());
        return buffer.size();
    };
    func();

    EXPECT_TRUE(buffer.starts_with(
        R"(ERROR: Condition "buffer.size() >= 0" is true. Returning: buffer.size()
    at)"))
        << buffer;
}

TEST(error_macros, ERR_FAIL_COND_V_MSG) {
    std::string buffer;
    TestOS e{ &buffer };
    auto func = [&]() {
        ERR_FAIL_COND_V_MSG(buffer.size() >= 0, buffer.size(), "???");
        return buffer.size();
    };
    func();

    EXPECT_TRUE(buffer.starts_with(
        R"(ERROR: Condition "buffer.size() >= 0" is true. Returning: buffer.size()
Detail: ???
    at)"))
        << buffer;
}

TEST(error_macros, ERR_FAIL_INDEX) {
    std::string buffer;
    TestOS e{ &buffer };
    auto func = [&]() {
        int a = 1;
        int b = 1;
        ERR_FAIL_INDEX(a, b);
    };
    func();

    EXPECT_TRUE(buffer.starts_with(
        R"(ERROR: Index a = 1 is out of bounds (b = 1).
    at)"))
        << buffer;
}

TEST(error_macros, ERR_FAIL_INDEX_MSG) {
    std::string buffer;
    TestOS e{ &buffer };
    auto func = [&]() {
        int a = 1;
        int b = 1;
        ERR_FAIL_INDEX_MSG(a, b, "?????");
    };
    func();

    EXPECT_TRUE(buffer.starts_with(
        R"(ERROR: Index a = 1 is out of bounds (b = 1).
Detail: ?????
    at)"))
        << buffer;
}

TEST(error_macros, ERR_FAIL_INDEX_V) {
    std::string buffer;
    TestOS e{ &buffer };
    auto func = [&]() {
        int a = 1;
        int b = 1;
        ERR_FAIL_INDEX_V(a, b, 10);
        return 20;
    };
    func();

    EXPECT_TRUE(buffer.starts_with(
        R"(ERROR: Index a = 1 is out of bounds (b = 1).
    at)"))
        << buffer;
}

TEST(error_macros, ERR_FAIL_INDEX_V_MSG) {
    std::string buffer;
    TestOS e{ &buffer };
    auto func = [&]() {
        int a = 1;
        int b = 1;
        ERR_FAIL_INDEX_V_MSG(a, b, 10, "????");
        return 20;
    };
    func();

    EXPECT_TRUE(buffer.starts_with(
        R"(ERROR: Index a = 1 is out of bounds (b = 1).
Detail: ????
    at)"))
        << buffer;
}

TEST(error_macros, DEV_ASSERT) {
    ErrorHandler handler;
    handler.data.error_func = assert_handler;

    add_error_handler(&handler);

    EXPECT_EXIT(
        {
            int a = 200;
            int b = 200;
            DEV_ASSERT(a - b);
        },
        testing::ExitedWithCode(99), "FATAL: DEV_ASSERT failed \\(a - b\\)");

    remove_error_handler(&handler);
}

TEST(error_macros, DEV_ASSERT_INDEX) {
    ErrorHandler handler;
    handler.data.error_func = assert_handler;

    add_error_handler(&handler);

    EXPECT_EXIT(
        {
            int a = 201;
            int b = 200;
            DEV_ASSERT_INDEX(a, b);
        },
        testing::ExitedWithCode(99), "FATAL: DEV_ASSERT_INDEX failed Index a = 201 is out of bounds \\(b = 200\\).");

    remove_error_handler(&handler);
}

}  // namespace vct
