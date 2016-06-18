#ifndef _TCOMMANDRESULT_H_
#define _TCOMMANDRESULT_H_

namespace simulator {
    namespace command {


/**
 * @brief Base class for any class representing the result of a command.
 */
class TCommandResult {
    public:
        /**
         * @brief Virtual destructor required for polymorphism.
         */       
        virtual ~TCommandResult() {};    
};

}}

#endif
