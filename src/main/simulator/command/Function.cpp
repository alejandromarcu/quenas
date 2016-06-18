#include <string>
#include <iostream>

#include "Function.h"
#include "Exceptions.h"

namespace simulator {
    namespace command {

using namespace std;

/**
 * @brief Build a function from a string.
 *
 * @param function the string that will be parsed to build the command.
 */
Function::Function(const string &function) : originalString(function)
{
    string t = trim(function);

    name = "";
    int i=0;
   
    // up to the first parenthesis is the function name
    while (t[i]!= '(' && i < t.length()) {
        name += t[i];
        i++;
    }
    // there was no parenthesis, so the command ends here
    if (i == t.length()) return;

    // erase spaces around name
    name = trim(name);
        
    // last character must be a parenthesis
    if (t[t.length() -1] != ')') throw invalid_argument("')' expected");

    // read the parameters, they're between the first and last parenthesis.
    string p = t.substr(i + 1, t.length() - i - 2);
    
    // split the parameters by ',' and set them, doing any necessary formatting
    if (trim(p).length() > 0) setParams(split(p, ','));
}


/**
 * @brief Clean and checks the parameters and stores them in params member.
 *
 * @param p collection of parameters.
 */
void Function::setParams(const vector<string> &p)
{
    for(int i = 0; i < p.size(); i++) {
        string str = trim(p[i]);
        
        if (str[0] == '\'' && str[str.length() - 1] == '\'') {
             str = str.substr(1, str.length()-2);
             if (str.find('\'') != string::npos) throw invalid_argument("more quotes than expected in " + str);
        }
        
        if ((str[0] == '\'') != (str[str.length() - 1] == '\'')) throw invalid_argument("error in quoted string: " + str);
        
        params.push_back(str);
    }
}

/**
 * @brief Get the function name.
 *
 * @return the function name.
 */
string Function::getName() const
{
    return name;
}

/**
 * @brief Get the original string for that function.
 *
 * @return the original string for that function.
 */
string Function::toString() const
{
    return originalString;
}

/**
 * @brief Get the number of parameters in the function.
 *
 * @return the number of parameters in the function.
 */
int Function::getParamCount() const
{
    return params.size();
}

/**
 * @brief Get a parameter as a boolean value.
 * The string value must be either false or true, and if not an invalid_argument
 * exception is thrown.
 *
 * @param n index of parameter to get.
 * @return the boolean value for parameter n.
 */
bool Function::getBoolParam(int n) const
{
    if (trim(getStringParam(n)) == "true") return true;
    if (trim(getStringParam(n)) == "false") return false;    
    throw invalid_argument(getStringParam(n) + " is not a valid boolean value for parameter " + toStr(n));
}

/**
 * @brief Get a parameter as an int value.
 *
 * @param n index of parameter to get.
 * @return the int value for parameter n.
 */
int Function::getIntParam(int n) const
{
    return atoi(getStringParam(n).c_str());
}

/**
 * @brief Get a parameter as a long value.
 *
 * @param n index of parameter to get.
 * @return the int value for parameter n.
 */
long Function::getLongParam(int n) const
{
    return atol(getStringParam(n).c_str());
}

/**
 * @brief Get a parameter as a string value.
 *
 * @param n index of parameter to get.
 * @return the string value for parameter n.
 */
string Function::getStringParam(int n) const
{
    if (n >= params.size()) throw command_error("Parameter #" + toStr(n+1) + " expected for function " + name);
    return params[n];
}

/**
 * @brief Get a parameter as a Time value.
 *
 * @param n index of parameter to get.
 * @return the Time value for parameter n.
 */
Time Function::getTimeParam(int n) const
{
    return Time(getStringParam(n));
}

/**
 * @brief Get all the parameters as string.
 *
 * @return all the parameters as string
 */
vector<string> Function::getParams() const
{
    return params;
}

}}
