#ifndef _TRACEROUTE_H_
#define _TRACEROUTE_H_

#include <vector>
#include <iterator>

#include "common.h"
#include "HCPacket.h"
#include "Message.h"
#include "Units.h"
#include "Event.h"
#include "Simulator.h"
#include "HypercubeMaskAddress.h"
#include "MACAddress.h"
#include "Applications.h"

namespace simulator {
    namespace hypercube {

/** 
 * @brief Hypercube Application to trace routes between nodes.
 */
class TraceRoute : public HypercubeBaseApplication, public TCommandRunner {
    public:
        /// Port used for Trace Route Application
        const static int PORT;
        
        TraceRoute(TTransportLayer *tl);
        virtual ~TraceRoute();
        virtual TCommandResult *runCommand(const Function &function);
        virtual string getName() const;                
        
        virtual void receive(const TNetworkAddress &from, const TApplicationId &sourceAppId, const Data &data, const TPacket *packet);        
};
      
      
}
}

#endif
