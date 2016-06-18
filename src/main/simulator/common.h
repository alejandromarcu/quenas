#ifndef _COMMON_H_
#define _COMMON_H_

#include <stdexcept>
#include <string>
#include <vector>
#include <sstream>
#include <iostream>

namespace simulator {
using namespace std;

typedef unsigned char byte;
typedef unsigned short int int16;

typedef vector<byte> VB;
typedef back_insert_iterator<VB> BackItVB;
typedef VB::iterator ItVB;
typedef VB::const_iterator ConstItVB;

/**
 * @brief Convert a numeric type to a string.
 *
 * @param x the numeric type to convert.
 * @return the string representation for x.
 */
template<typename T> string toStr(T x)
{
    ostringstream s;
    s << fixed << x;
    return s.str();
}

string toHex(byte b);

void dumpInt16To(int16 value, BackItVB it);

int16 readInt16(VB::const_iterator &it);

void dumpByteTo(byte value, BackItVB it);

byte readByte(VB::const_iterator &it);

void dumpStringTo(const string &s, BackItVB it);

string readString(VB::const_iterator &it);

string trim(string const& source, char const* delims = " \t\r\n");

string toLower(const string &str);

vector<string> split(const string &str, char separator);

};



#endif
