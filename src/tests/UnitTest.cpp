#include <iostream>

#include "UnitTest.h"
#include "common.h"

using namespace std;

/**
 * @brief Create an unit test.
 *
 * @param name unit test name.
 */
UnitTest::UnitTest(string name)
{
    cout << "Testing "<< name << endl;
    this->name = name;
}

/**
 * @brief Test if two strings are equal.
 *
 * @param expected expected value
 * @param result obtained
 * @param message error message in case the test fail.
 */
void UnitTest::areEqual(const string &expected, const string &result, string message)
{
    if (expected != result) {
        cout << "  FAILED. " << message << ". expected " << expected << " got " << result << endl;
    }
}

/**
 * @brief Test if two ints are equal.
 *
 * @param expected expected value
 * @param result obtained
 * @param message error message in case the test fail.
 */
void UnitTest::areEqual(int expected, int result, string message)
{
    if (expected != result) {
        cout << "  FAILED. " << message << ". expected " << expected << " got " << result << endl;
    }
}

/**
 * @brief Test if two pointers are equal.
 *
 * @param expected expected value
 * @param result obtained
 * @param message error message in case the test fail.
 */
void UnitTest::areEqual(void *expected, void *result, string message)
{
    if (expected != result) {
        cout << "  FAILED Pointer Comparisson: " << message << "." << endl;
    }
}

/**
 * @brief Test that a boolean value is true.
 *
 * @param expr value
 * @param message error message in case the test fail.
 */
void UnitTest::isTrue(bool expr, string message)
{
    if (!expr) {
        cout << "  FAILED. " << message << endl;
    }
}

/**
 * @brief Test that a boolean value is false.
 *
 * @param expr value
 * @param message error message in case the test fail.
 */
void UnitTest::isFalse(bool expr, string message)
{
    if (expr) {
        cout << "  FAILED. " << message << endl;
    }
}

/**
 * @brief Test if a charr array equals a char vector, byte by byte
 *
 * @param expected expected value
 * @param size expected size
 * @param result obtained
 * @param message error message in case the test fail.
 */
void UnitTest::areEqual(unsigned char *expected, int size, const vector<unsigned char> &result, string message)
{
    if (size != result.size()) {
        cout << "  FAILED.  " << message << ". Result expected to be " 
             << size << " bytes long but was " << result.size() << endl;
        return;
    }
    for (int i = 0 ; i < size; i++)
        if (expected[i] != result[i]) {
            cout << "  FAILED.  " << message << ". byte " << i << " expected to be " 
                 << (int) expected[i] << " but was " << (int) result [i] << endl;           
            return;
        }
}
