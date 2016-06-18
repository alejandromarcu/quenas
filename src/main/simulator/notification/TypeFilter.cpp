#include <string>
#include <iostream>
#include <fstream>
#include <map>

#include "Command.h"
#include "Notification.h"
#include "Simulator.h"
#include "TypeFilter.h"
#include "Exceptions.h"

namespace simulator {
    namespace notification {
    
using namespace simulator::command;
using namespace simulator;
using namespace std;


/**
 * @brief Create a new TypeFilter, that will accept depending just on the type,
 * based on an accept/deny list.
 *
 * @param acceptByDefault if no entry matches, this is the value to be returned.
 */
TypeFilter::TypeFilter(bool acceptByDefault) : acceptByDefault(acceptByDefault)
{
}

/**
 * @brief Return whether this type is accepted or not.
 *
 * @param type type to check.
 * @return whether this type is accepted or not.
 */
bool TypeFilter::isAccepted(const string &type) const
{
    string t = type;
    while (true) {
        EntryMap::const_iterator it = entries.find(t);
        if (it != entries.end()) return it->second;
     
        int pos = t.find_last_of('.');
        if (pos < 0) return acceptByDefault;
        t = t.substr(0, pos);
    } 
    
}

/**
 * @brief Sets a type to be accepted.
 *
 * @type type to accept.
 */
void TypeFilter::accept(const string &type)
{
    entries[type] = true;
}

/**
 * @brief Sets a type to be denied.
 *
 * @type type to deny.
 */
void TypeFilter::deny(const string &type)
{
    entries[type] = false;
}

/**
 * @brief Sets whether a type that was not matched should be accepted or not.
 *
 * @param acceptByDefault true if no matched types should be accepted.
 */
void TypeFilter::setDefault(bool acceptByDefault)
{
    this->acceptByDefault = acceptByDefault;
}

/**
 * @brief Clear all the entries in the filter.
 */
void TypeFilter::clear()
{
    entries.clear();
}

/**
 * @brief Run a command.
 *       
 * @param function function to run.
 */
TCommandResult *TypeFilter::runCommand(const Function &function)
{
    if (function.getName() == "accept") 
    {
        for (int i = 0; i < function.getParamCount(); i++)
            accept(function.getStringParam(i));
        
        return this;
    }

    if (function.getName() == "deny") 
    {
        for (int i = 0; i < function.getParamCount(); i++)
            deny(function.getStringParam(i));
        
        return this;
    }

    if (function.getName() == "setDefault") 
    {
        setDefault(function.getBoolParam(0));
        
        return this;
    }

    throw command_error("TypeFilter - Bad Command: " + function.toString());    
}

/**
 * @brief Get the name of this object, required for a command runner.
 * 
 * @return "TypeFilter"
 */
string TypeFilter::getName() const
{
    return "TypeFilter";
}



    
}
}
