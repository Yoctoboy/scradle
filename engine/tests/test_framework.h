#ifndef SCRADLE_TEST_FRAMEWORK_H
#define SCRADLE_TEST_FRAMEWORK_H

#include <algorithm>
#include <chrono>
#include <climits>
#include <exception>
#include <iostream>
#include <sstream>
#include <streambuf>
#include <string>

using std::cout;
using std::endl;
using std::string;

namespace test {

// ANSI color codes
namespace color {
const string RESET = "\033[0m";
const string RED = "\033[31m";
const string GREEN = "\033[32m";
const string YELLOW = "\033[33m";
const string BLUE = "\033[34m";
const string MAGENTA = "\033[35m";
const string CYAN = "\033[36m";
const string BOLD = "\033[1m";
}  // namespace color

// Test statistics
static int tests_run = 0;
static int tests_passed = 0;
static int tests_failed = 0;

// Assert that condition is true
inline void assert_true(bool condition, const string& test_name, const bool display_on_success = true) {
    tests_run++;
    if (condition) {
        tests_passed++;
        if (display_on_success) cout << color::GREEN << "  ✓ " << color::RESET << test_name << endl;
    } else {
        tests_failed++;
        cout << color::RED << "  ✗ " << test_name << " - FAILED" << color::RESET;
        cout << endl;
    }
}

inline void assert_false(bool condition, const string& test_name, const bool display_on_success = true) {
    tests_run++;
    if (!condition) {
        tests_passed++;
        if (display_on_success) cout << color::GREEN << "  ✓ " << color::RESET << test_name << endl;
    } else {
        tests_failed++;
        cout << color::RED << "  ✗ " << test_name << " - FAILED" << color::RESET;
        cout << endl;
    }
}

// Assert that two values are equal
template <typename T>
inline bool assert_equal(const T& expected, const T& actual, const string& test_name, const bool display_on_success = true) {
    tests_run++;
    if (expected == actual) {
        tests_passed++;
        if (display_on_success) cout << color::GREEN << "  ✓ " << color::RESET << test_name << endl;
        return true;
    } else {
        tests_failed++;
        cout << color::RED << "  ✗ " << test_name << " - FAILED" << color::RESET << endl;
        cout << color::YELLOW << "    Expected: " << color::RESET << expected << endl;
        cout << color::YELLOW << "    Actual:   " << color::RESET << actual << endl;
        return false;
    }
}

// Print test summary
inline void print_summary() {
    cout << "\n"
         << color::CYAN << "========================================" << color::RESET << endl;
    cout << color::BOLD << "Tests run:    " << color::RESET << tests_run << endl;
    cout << color::BOLD << "Tests passed: " << color::RESET << color::GREEN << tests_passed << color::RESET << endl;
    cout << color::BOLD << "Tests failed: " << color::RESET << (tests_failed > 0 ? color::RED : color::GREEN) << tests_failed << color::RESET << endl;
    cout << color::CYAN << "========================================" << color::RESET << endl;

    if (tests_failed == 0) {
        cout << color::GREEN << color::BOLD << "✓ All tests passed!" << color::RESET << endl;
    } else {
        cout << color::RED << color::BOLD << "✗ Some tests failed." << color::RESET << endl;
    }
}

// Return exit code (0 = success, 1 = failure)
inline int exit_code() {
    return tests_failed == 0 ? 0 : 1;
}

// Helper macro to time a test function (runs 10 times, shows min/max/avg)
#define TIME_TEST(name, test_func)                                                                                     \
    do {                                                                                                               \
        const int NUM_RUNS = 5;                                                                                        \
        long long min_time = LLONG_MAX;                                                                                \
        long long max_time = 0;                                                                                        \
        long long total_time = 0;                                                                                      \
        std::cout << endl                                                                                              \
                  << "=== Timing test: " << name << " ===" << std::endl;                                               \
        for (int run = 0; run < NUM_RUNS; run++) {                                                                     \
            auto start = std::chrono::high_resolution_clock::now();                                                    \
            {                                                                                                          \
                CoutSilencer silence;                                                                                  \
                test_func();                                                                                           \
            }                                                                                                          \
            auto end = std::chrono::high_resolution_clock::now();                                                      \
            long long duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();           \
                                                                                                                       \
            min_time = std::min(min_time, duration);                                                                   \
            max_time = std::max(max_time, duration);                                                                   \
            total_time += duration;                                                                                    \
        }                                                                                                              \
                                                                                                                       \
        long long avg_time = total_time / NUM_RUNS;                                                                    \
        cout << "  [⏱️  min: " << min_time << " ms | avg: " << avg_time << " ms | max: " << max_time << " ms]" << endl; \
    } while (0)

}  // namespace test

class CoutSilencer {
   public:
    CoutSilencer() : old_buf(std::cout.rdbuf()) {
        std::cout.rdbuf(null_stream.rdbuf());
    }

    ~CoutSilencer() {
        std::cout.rdbuf(old_buf);
    }

   private:
    std::streambuf* old_buf;
    std::ostringstream null_stream;
};

#endif  // SCRADLE_TEST_FRAMEWORK_H
