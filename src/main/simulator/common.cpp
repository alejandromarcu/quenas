#include "common.h"

namespace simulator {
using namespace std;

/**
 * @brief Get the hexadecimal representation for a byte.
 *
 * @param b byte to get its hexadecimal representation
 * @return a 2 char string with b's hexadecimal representation.
 */
string toHex(byte b)
{
    char a[5];
    sprintf(a,"%02X",b);
    return string(a);
}

/**
 * @brief Dump an int16 value to an iterator
 *
 * @param value value to dump.
 * @param it iterator where the value must be dumped.
 */
void dumpInt16To(int16 value, BackItVB it)
{
    *it = (value >> 8);
    it++;
    *it = value & 0xFF;
}


/**
 * @brief Read an int16 from an iterator, advancing it.
 *
 * @param it iterator to read from (will advance).
 * @return the read value.
 */int16 readInt16(VB::const_iterator &it)
{
    int16 value;
    value = (*it) << 8;
    it++;
    value += *it;
    it++;
    return value;
}

/**
 * @brief Dump a byte value to an iterator
 *
 * @param value value to dump.
 * @param it iterator where the value must be dumped.
 */
void dumpByteTo(byte value, BackItVB it)
{
    *it = value;
    it++;
}

/**
 * @brief Read a byte from an iterator, advancing it.
 *
 * @param it iterator to read from (will advance).
 * @return the read value.
 */
byte readByte(VB::const_iterator &it)
{
    return *(it++);
}

/**
 * @brief Dump a string to an iterator
 *
 * @param value value to dump.
 * @param it iterator where the value must be dumped.
 */
void dumpStringTo(const string &s, BackItVB it)
{
    *it = s.length();
    it++;

    for (int i=0; i < s.length(); i++, it++) *it = s[i];
}

/**
 * @brief Read a string from an iterator, advancing it.
 *
 * @param it iterator to read from (will advance).
 * @return the read value.
 */
string readString(VB::const_iterator &it)
{
    byte length = readByte(it);
    string s(length, ' ');
    for (int i =0; i < length; i++) s[i] = *(it++);
    return s;
}


/**
 * @brief Erases blank spaces at the beggining and end of a string.
 *
 * @param source string to trim
 * @param delims characters that must be cleaned
 * @return the trimmed string.
 */
string trim(string const& source, char const* delims) {
  string result(source);
  string::size_type index = result.find_last_not_of(delims);
  if(index != string::npos)
    result.erase(++index);

  index = result.find_first_not_of(delims);
  if(index != string::npos)
    result.erase(0, index);
  else
    result.erase();
  return result;
}


/**
 * @brief Get a lowercase representation of a string.
 *
 * @param str string to lowercase.
 * @return str in lowercase.
 */
string toLower(const string &str) {
    string lower;
    for (string::const_iterator it = str.begin(); it != str.end(); it++)
        lower += tolower(*it);

    return lower;
}


/**
 * @brief Split a string taking into account parenthesis and simple quotes.
 *
 * @brief str the string to split.
 * @brief separator the character that splits.
 * @return a vector of strings consisting in str splited by the separator.
 */
vector<string> split(const string &str, char separator)
{
    // level of parenthesis
    int pthLevel = 0;
    bool quoted = false;
    vector<string> result;
    string value = "";

    for (int i = 0; i < str.length(); i++) {
        char ch = str[i];

        // if a quote is found, reverse the quoted state and push the quote
        if (ch == '\'') {
            quoted = !quoted;
            value += '\'';
            continue;
        }

        // if we're inside a quote, just push the character without checking
        // if it is a separator or a parenthesis.
        if (quoted) {
            value += ch;
            continue;
        }

        // increase or decrease parenthesis level
        if (ch == '(') pthLevel++;
        if (ch == ')') {
            if (pthLevel == 0) throw invalid_argument("')' not expected at position " + toStr(i));
            pthLevel--;
        }

        // if we aren't inside a parenthesis and the character is a separator,
        // add the value to the result collection and clear it.
        if (pthLevel == 0 && ch == separator)   {
            result.push_back(value);
            value = "";
        } else {
            value += ch;
        }
    }
    result.push_back(value);
    if (pthLevel != 0) throw invalid_argument("')' expected");

    return result;

}

};
