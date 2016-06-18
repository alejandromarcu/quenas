#ifndef _TNETWORK_ADDRESS_H_
#define _TNETWORK_ADDRESS_H_


#include "TAddress.h"
#include "common.h"

namespace simulator {
    namespace address {

/** 
 * @brief Base class for a Network Address.
 *
 * It doesn't add anything to TAddress; it just serves as a base class.
 */
class TNetworkAddress : public TAddress {
    protected:
        /**
         * @brief Empty constructor.  Does nothing.
         */        
        TNetworkAddress() {};
        
    public:       
        /**
         * @brief Creates an address of the specified length of all zeros.
         *
         * @param length length of the address
         */
        TNetworkAddress(int length) : TAddress(length) {};
        
        /**
         * @brief Creates an address of the specified length containing addr.
         *
         * @param addr pointer to at least length bytes of the address
         * @param length length of the address
         */        
        TNetworkAddress(byte *addr, int length) : TAddress(addr, length) {};
};

// end namespaces
}
}

#endif
