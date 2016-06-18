#ifndef _APPLICATIONS_H_
#define _APPLICATIONS_H_


#include "common.h"
#include "HCPacket.h"
#include "Message.h"
#include "Units.h"
#include "Event.h"
#include "Simulator.h"
#include "HypercubeMaskAddress.h"
#include "MACAddress.h"

namespace simulator {
    namespace hypercube {

using namespace std;
using namespace simulator::hypercube::dataUnit;
using namespace simulator::message;
using namespace simulator::event;
using namespace simulator;
using namespace simulator::address;

/**
 * @brief Base class for all Hypercube Applications.
 * It provides the send helper method to send resolving an universal address,
 * as well as some helper methods.
 */
class HypercubeBaseApplication : public TApplicationLayer {
    public:
        HypercubeBaseApplication(TTransportLayer *tl);
        virtual ~HypercubeBaseApplication() {};
        virtual TApplicationId getApplicationId() const;
        
    protected:
        void bind(Port p);
        void unbind();
        void send(const UniversalAddress &dest, Port port, const Data &data);
        
    private:
        /// Port where the application resides.
        Port port;
        
};

/**
 * @brief Test Application for the Hypercube Protocol.
 * It can send packet to a Test Application in another node.
 *
 */
class TestApplication : public HypercubeBaseApplication, public TCommandRunner {
    public:
        /// Port used for Test Application
        const static int PORT;
        
        TestApplication(TTransportLayer *tl);
        virtual ~TestApplication();
        
        virtual void receive(const TNetworkAddress &from, const TApplicationId &sourceAppId, const Data &data, const TPacket *packet);

        TCommandResult *runCommand(const Function &function);
        
        virtual string getName() const;                        
        
};

      
}
}

#endif
