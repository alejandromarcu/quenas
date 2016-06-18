#ifndef _EXCEPTIONS_H_
#define _EXCEPTIONS_H_


namespace simulator {

/**
 * @brief Exception thrown when there is an error in a command.
 */
class command_error : public logic_error {
    public:
        /**
         * @brief Create the exception.
         *
         * @param what_arg explanation of the error.
         */         
        command_error(const string& what_arg) : logic_error(what_arg) {};
};


}

#endif
