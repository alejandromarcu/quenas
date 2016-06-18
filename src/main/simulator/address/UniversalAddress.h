#ifndef _UNIVERSALADDRESS_H_
#define _UNIVERSALADDRESS_H_

#include "TApplicationAddress.h"
#include "common.h"
#include "MACAddress.h"
#include "HypercubeAddress.h"

namespace simulator {
    namespace address {

/** 
 * @brief Represents an universal address.
 * 
 */
class UniversalAddress : public TApplicationAddress {
    public:
        UniversalAddress(byte *addr);
        UniversalAddress(const string &str);
        
        virtual string toString() const;
        MACAddress hashToMAC() const;
        HypercubeAddress hashToHypercube(int bitLength) const;
};

// end namespaces
}
}

#endif
