#ifndef _FUNCTION_H_
#define _FUNCTION_H_

#include <string>
#include <vector>

#include "Units.h"
#include "common.h"

namespace simulator {
    namespace command {

using namespace std;
using namespace simulator;


/**
 * @brief Represents one Function of a command.
 *
 * A function has a name and optionally any number of arguments. 
 * Example of functions: "query()", "node(1)", "setDelay(2)".
 */
class Function {
    public:
        Function(const string &function);
        string getName() const;
        int getParamCount() const;
        bool getBoolParam(int n) const;
        int getIntParam(int n) const;
        long getLongParam(int n) const;        
        string getStringParam(int n) const;
        Time getTimeParam(int n) const;                
        
        vector<string> getParams() const;
        
        string toString() const;
    private:
        void setParams(const vector<string> &p);
   
        /// Name of the function             
        string name;
        
        /// Sting used to build the function
        string originalString;
        
        /// All the function parameters stored as strings
        vector<string> params;
};

}}

#endif
