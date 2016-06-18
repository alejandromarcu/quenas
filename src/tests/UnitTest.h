#ifndef _UNIT_TEST_H_
#define _UNIT_TEST_H_

#include <iostream>
#include <vector>
#include "common.h"

using namespace std;

/**
 * @brief Class to run unit tests.
 */
class UnitTest {
    private:
        string name;
    
    public:
        UnitTest(string name);
    
        void areEqual(const string &expected, const string &result, string message="Bad Value");
        void areEqual(int expected, int result, string message="Bad Value");
        void areEqual(void *expected, void *result, string message="Bad Value");
        void areEqual(unsigned char *expected, int size, const vector<unsigned char> &result, string message="Bad Value");
        
        template<class T>
        void areEqual(T expected, T result, string message="Bad Value") {
            if (expected != result) {
                cout << "  FAILED. " << message << ". expected " << expected << " got " << result << endl;
            }        
        }
    
        void isTrue(bool expr, string message="Must be true");
        void isFalse(bool expr, string message="Must be false");
};

#endif
