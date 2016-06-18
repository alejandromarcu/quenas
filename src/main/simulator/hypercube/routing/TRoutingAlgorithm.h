#ifndef _TROUTING_ALGORITHM_H
#define _TROUTING_ALGORITHM_H

#include "HypercubeAddress.h"
#include "DataPacket.h"

namespace simulator {
    namespace hypercube {
        namespace routing {

using namespace std;
using namespace simulator::address;
using namespace simulator::hypercube::dataUnit;

/**
 * @brief Base class for any routing algorithm
 */
class TRoutingAlgorithm : public TCommandRunner {
    public:
        /**
         * @brief Route a packet.  It must decide the right address to send the packet and return it.
         *
         * @param packet packet to route
         * @param from address where the packet came from.
         * @return the address of the next hop.
         */
        virtual HypercubeAddress route(DataPacket *packet, const HypercubeAddress &from=HypercubeAddress()) = 0;
        
        /**
         * @brief Virtual destructor. 
         */
        virtual ~TRoutingAlgorithm() {};
};
      
      
}
}
}
#endif
