#ifndef _TYPE_FILTER_H
#define _TYPE_FILTER_H

#include <string>
#include <iostream>
#include <vector>
#include <map>

#include "Units.h"
#include "Notification.h"
#include "Command.h"

namespace simulator {
    namespace notification {

using namespace std;
using namespace simulator;
using namespace simulator::command;

 /**
 * @brief Base class for Notificator Filters.
 * Classes implementing this will decide if a type is accepted (ie, the notification
 * will be written) or not.
 */
class TNotifFilter : public TCommandRunner {
    public:
        /**
         * @brief Virtual Destructor. Does nothing.
         */
        virtual ~TNotifFilter() {};
        
        /**
         * @brief Return whether this type is accepted or not.
         *
         * @param type type to check.
         * @return whether this type is accepted or not.
         */
        virtual bool isAccepted(const string &type) const = 0;
};


/**
 * @brief Filter by notification type.
 * 
 * A list of accepted/denied types can be entered, and if the type ends in *,
 * it matches up to this character.
 * When calling isAccepted, the list is checked in order, and if matches with some
 * of the accepted or denied entries, true or false is returned respectively.
 * If there are no matches, "acceptByDefault" is returned.
 */
class TypeFilter : public TNotifFilter {
    public:
        typedef map<string, bool> EntryMap;
        
        TypeFilter(bool acceptByDefault = false);
        virtual bool isAccepted(const string &type) const;
        void accept(const string &type);
        void deny(const string &type);        
        void setDefault(bool acceptByDefault);
        void clear();
        
        virtual TCommandResult *runCommand(const Function &function);
        virtual string getName() const;
        
    private:
        /// Map containing for each entry if it is accepted or denied
        EntryMap entries;
        
        /// If the type is not found, this value will be returned.
        bool acceptByDefault;
};

        
}
}


#endif
