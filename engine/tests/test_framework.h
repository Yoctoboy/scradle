#ifndef SCRADLE_TEST_FRAMEWORK_H
#define SCRADLE_TEST_FRAMEWORK_H

#include <iostream>
#include <string>
#include <sstream>
#include <exception>

using std::string;
using std::cout;
using std::endl;

namespace test {

// ANSI color codes
namespace color {
    const string RESET   = "\033[0m";
    const string RED     = "\033[31m";
    const string GREEN   = "\033[32m";
    const string YELLOW  = "\033[33m";
    const string BLUE    = "\033[34m";
    const string MAGENTA = "\033[35m";
    const string CYAN    = "\033[36m";
    const string BOLD    = "\033[1m";
}

// Test statistics
static int tests_run = 0;
static int tests_passed = 0;
static int tests_failed = 0;

// Assert that condition is true
inline void assert_true(bool condition, const string& test_name, const string& message = "") {
    tests_run++;
    if (condition) {
        tests_passed++;
        cout << color::GREEN << "  ✓ " << color::RESET << test_name << endl;
    } else {
        tests_failed++;
        cout << color::RED << "  ✗ " << test_name << " - FAILED" << color::RESET;
        if (!message.empty()) {
            cout << ": " << message;
        }
        cout << endl;
    }
}

inline void assert_false(bool condition, const string& test_name, const string& message = "") {
    tests_run++;
    if (!condition) {
        tests_passed++;
        cout << color::GREEN << "  ✓ " << color::RESET << test_name << endl;
    } else {
        tests_failed++;
        cout << color::RED << "  ✗ " << test_name << " - FAILED" << color::RESET;
        if (!message.empty()) {
            cout << ": " << message;
        }
        cout << endl;
    }
}

// Assert that two values are equal
template<typename T>
inline void assert_equal(const T& expected, const T& actual, const string& test_name) {
    tests_run++;
    if (expected == actual) {
        tests_passed++;
        cout << color::GREEN << "  ✓ " << color::RESET << test_name << endl;
    } else {
        tests_failed++;
        cout << color::RED << "  ✗ " << test_name << " - FAILED" << color::RESET << endl;
        cout << color::YELLOW << "    Expected: " << color::RESET << expected << endl;
        cout << color::YELLOW << "    Actual:   " << color::RESET << actual << endl;
    }
}

// Print test summary
inline void print_summary() {
    cout << "\n" << color::CYAN << "========================================" << color::RESET << endl;
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

} // namespace test

#endif // SCRADLE_TEST_FRAMEWORK_H
