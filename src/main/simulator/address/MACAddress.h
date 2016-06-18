#ifndef _MACAddress_H_
#define _MACAddress_H_

#include "TPhysicalAddress.h"
#include "common.h"

namespace simulator {
    namespace address {

/** 
 * @brief Represents a MAC address of 48 bits.
 * 
 */
class MACAddress : public TPhysicalAddress {
    public:        
        // Address used for broadcasting
        const static MACAddress BROADCAST;
        
        MACAddress();
        MACAddress(byte *addr);
        virtual string toString() const;
};

// end namespaces
}
}

#endif 
