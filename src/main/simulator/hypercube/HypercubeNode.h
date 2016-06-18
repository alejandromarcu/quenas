#ifndef _HYPERCUBE_NODE_H
#define _HYPERCUBE_NODE_H

#include <queue>
#include <map>

#include "Units.h"
#include "common.h"
#include "Event.h"
#include "Notification.h"
#include "Command.h"
#include "UniversalAddress.h"
#include "PhysicalLayer.h"
#include "DataLinkLayer.h"
#include "Message.h"
#include "TypeFilter.h"
#include "TraceRoute.h"
#include "Simulator.h"
#include "UDPTransportLayer.h"
#include "HypercubeRoutingLayer.h"
#include "HypercubeControlLayer.h"
#include "RendezVousServer.h"
#include "RendezVousClient.h"

namespace simulator {
	namespace hypercube {

using namespace simulator::event;
using namespace simulator::notification;
using namespace simulator::command;
using namespace simulator::address;
using namespace simulator::layer;
using namespace simulator::message;
using namespace simulator;

using namespace std;

/**
 * @brief Class for a Node working with Hypercube protocol.
 */
class HypercubeNode : public TNode, TMessageReceiver {
    public:
        HypercubeNode(const UniversalAddress &uaddr);
        virtual ~HypercubeNode();
        virtual void onMessageReceived(const TMessage *message);

        const UniversalAddress &getUniversalAddress() const;
        HypercubeAddress getPrimaryAddress() const;
        MACAddress getPhysicalAddress() const;
        int getPrimaryAddressMask() const;

        bool hasAddress(const HypercubeAddress &addr) const;

        virtual TCommandResult *runCommand(const Function &function);
        virtual string getName() const;
        virtual string getId() const;

        PhysicalLayer *getPhyiscalLayer();
        HypercubeControlLayer *getHypercubeControlLayer();
        
        bool isConnected() const;

    private:
        ///the address of the node
        UniversalAddress uaddr;

    protected:
        /// Physical Layer of the node
        PhysicalLayer *physicalLayer;

        /// Data Link Layer of the node
        DataLinkLayer *dataLinkLayer;

        /// Network Layer of the node for connection and disconnection of nodes
        HypercubeControlLayer *hypercubeControlLayer;

        /// Network Layer of the node for routing
        HypercubeRoutingLayer *hypercubeRoutingLayer;

        /// Transport layer of the node (UDP simplified)
        UDPTransportLayer *transportLayer;

        /// Trace Route application of the node
        TraceRoute *traceRoute;

        /// Rendez Vous Server application of the node
        RendezVousServer *rendezVousServer;

        /// Rendez Vous Client application of the node
        RendezVousClient *rendezVousClient;

        /// Test Application of the node
        TestApplication *testApplication;

        /// Whether the node should be deleted after disconnection
        bool markedForDelete;
};


} }

#endif
