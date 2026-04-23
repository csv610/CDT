#ifndef TEST_MACROS_H
#define TEST_MACROS_H

#include <iostream>
#include <string>
#include <vector>
#include <source_location>

struct TestCase {
    std::string name;
    void (*func)();
};

inline std::vector<TestCase>& getTestCases() {
    static std::vector<TestCase> testCases;
    return testCases;
}

#define TEST(name) \
    void name(); \
    static bool name##_registered = []() { \
        getTestCases().push_back({#name, name}); \
        return true; \
    }(); \
    void name()

inline void assert_true(bool condition, const char* cond_str, std::source_location loc = std::source_location::current()) {
    if (!condition) {
        std::cerr << "Assertion failed: " << cond_str << " at " << loc.file_name() << ":" << loc.line() << " in " << loc.function_name() << std::endl;
        exit(1);
    }
}

#define ASSERT_TRUE(condition) assert_true(condition, #condition)

#define ASSERT_FALSE(condition) assert_true(!(condition), "!(" #condition ")")

template<typename T, typename U>
inline void assert_eq(const T& val1, const U& val2, const char* val1_str, const char* val2_str, std::source_location loc = std::source_location::current()) {
    if (!(val1 == val2)) {
        std::cerr << "Assertion failed: " << val1_str << " == " << val2_str << " (" << val1 << " != " << val2 << ") at " << loc.file_name() << ":" << loc.line() << " in " << loc.function_name() << std::endl;
        exit(1);
    }
}

#define ASSERT_EQ(val1, val2) assert_eq(val1, val2, #val1, #val2)

inline void assert_near(double val1, double val2, double epsilon, const char* val1_str, const char* val2_str, std::source_location loc = std::source_location::current()) {
    if (std::abs(val1 - val2) > epsilon) {
        std::cerr << "Assertion failed: " << val1_str << " near " << val2_str << " (diff: " << std::abs(val1 - val2) << " > " << epsilon << ") at " << loc.file_name() << ":" << loc.line() << " in " << loc.function_name() << std::endl;
        exit(1);
    }
}

#define ASSERT_NEAR(val1, val2, epsilon) assert_near(val1, val2, epsilon, #val1, #val2)

#endif
