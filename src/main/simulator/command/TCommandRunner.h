#ifndef _TCOMMANDRUNNER_H_
#define _TCOMMANDRUNNER_H_

#include <string>
#include "TCommandResult.h"
#include "Function.h"

namespace simulator {
    namespace command {

/**
 * @Brief Base class for all classes that can run a command.
 */
class TCommandRunner : public TCommandResult {
    public:
        /**
         * @brief Inheriting classes must implement this method so that it runs the specified
         * function and returns the result encapsulated into a TCommandResult.
         *
         * @param function The function to execute
         * @return the result of the execution encapsulated into a TCommandResult.
         */
        virtual TCommandResult *runCommand(const Function &function) = 0;
        
        /**
         * @brief Inheriting classes must implement this method to return the name
         * of the objects represented by that class.
         *
         * @return The name of the object represented by that class.
         */
        virtual string getName() const = 0;
        
        /**
         * @brief Inheriting classes can implement this method to return an 
         * identification of the object.
         *
         * @return The id of the object.  Empty by default.
         */         
        virtual string getId() const { return ""; };        
};

}}

#endif
