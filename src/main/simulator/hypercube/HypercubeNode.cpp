#include <iostream>
#include "Simulator.h"
#include "Units.h"
#include "Command.h"
#include "common.h"
#include "Notification.h"
#include "HypercubeControlLayer.h"
#include "HCPacket.h"
#include "Message.h"
#include "HypercubeNode.h"
#include "Exceptions.h"
#include "MultiCommandRunner.h"
#include "CommandQueryResult.h"
#include "HypercubeNetwork.h"

namespace simulator {
	namespace hypercube {

using namespace std;
using namespace simulator;
using namespace simulator::command;
using namespace simulator::notification;
using namespace simulator::hypercube::dataUnit;
using namespace simulator::message;

/**
 * @brief Create a Hypercube Node.
 * It creates and connects all its layers.
 *
 * @param uaddr universal address of the node.
 */
HypercubeNode::HypercubeNode(const UniversalAddress &uaddr) : uaddr(uaddr)
{
    markedForDelete = false;

    physicalLayer = new PhysicalLayer(this, uaddr.hashToMAC());
    dataLinkLayer = new DataLinkLayer(this, physicalLayer);
    physicalLayer->setDataLinkLayer(dataLinkLayer);

    hypercubeControlLayer = new simulator::hypercube::HypercubeControlLayer(this, dataLinkLayer);
    dataLinkLayer->registerNetworkProtocol(TControlPacket::ETHERNET_TYPE, hypercubeControlLayer);

    hypercubeRoutingLayer = new HypercubeRoutingLayer(dataLinkLayer, hypercubeControlLayer);
    dataLinkLayer->registerNetworkProtocol(DataPacket::ETHERNET_TYPE, hypercubeRoutingLayer);

    transportLayer = new UDPTransportLayer(this, hypercubeRoutingLayer);
    hypercubeRoutingLayer->registerTransportProtocol(TransportType(17), transportLayer);

    registerMessageListener(DisconnectedMessage::TYPE, this);

    traceRoute = new TraceRoute(transportLayer);
    rendezVousServer = new RendezVousServer(transportLayer);
    rendezVousClient = new RendezVousClient(transportLayer);
    testApplication = new TestApplication(transportLayer);
}

/**
 * @brief Called when a message is received.
 *
 * @param message message received.
 */
void HypercubeNode::onMessageReceived(const TMessage *message)
{
    if (markedForDelete && message->getType() == DisconnectedMessage::TYPE) {
        Simulator::getInstance()->addEvent(
           new CommandRunnerEvent(0, Simulator::getInstance()->getNetwork(),
               "deleteNode(" + getId() + ")"), true);
    }
}


/**
 * @param Destroy the Hypercube Node.
 * It deletes all its components.
 */
HypercubeNode::~HypercubeNode()
{
    delete rendezVousServer;
    delete rendezVousClient;
    delete testApplication;
    delete traceRoute;
    delete hypercubeControlLayer;
    delete dataLinkLayer;
    delete physicalLayer;
}

/**
 * @brief Get the universal address of the node.
 *
 * @return the universal address of the node.
 */
const UniversalAddress &HypercubeNode::getUniversalAddress() const
{
    return uaddr;
}

/**
 * @brief Run a command.
 *
 * @param function function to run.
 */
TCommandResult *HypercubeNode::runCommand(const Function &function)
{
    if (function.getName() == "query")
    {
        QueryResult *qr = new QueryResult(getName(), getId());

        if (function.getParamCount() == 1 && function.getStringParam(0) == "neighbours") {
            MultiValue *ns= new MultiValue("neighbours");
            map<MACAddress, Neighbour>::iterator it = hypercubeControlLayer->getNeighbours().begin();
            map<MACAddress, Neighbour>::iterator end = hypercubeControlLayer->getNeighbours().end();
            for (; it != end; it++) {
                
                string id = "UNKNOWN";
                if (it->second.getType() == Neighbour::PARENT || 
                    it->second.getType() == Neighbour::CHILD || 
                    it->second.getType() == Neighbour::ADJACENT) {
                   HypercubeNode *neighNode = dynamic_cast<HypercubeNetwork*>(Simulator::getInstance()->getNetwork())->getNode(it->second.getPrimaryAddress());
                   if (neighNode != NULL) id = neighNode->getId();
                } 
                
                MultiValue *n = new MultiValue("neighbour", id);
                n->insert("primaryAddress", it->second.getPrimaryAddress().toString());
                n->insert("physicalAddress", it->second.getPhysicalAddress().toString());
                n->insert("type", it->second.getTypeName());
                n->insert("lastSeen", it->second.getLastSeen().toString(Time::SEC));
                ns->insert("", n);
            }

            qr->insert("neighbours", ns);
        } else if (function.getParamCount() == 1 && function.getStringParam(0) == "stats") {
            MultiValue *control= new MultiValue("ControlPackets");

            control->insert("sent", hypercubeControlLayer->getPacketStats(HypercubeControlLayer::PACKETS_SENT));
            control->insert("received", hypercubeControlLayer->getPacketStats(HypercubeControlLayer::PACKETS_RECEIVED));
            control->insert("sentBytes", hypercubeControlLayer->getPacketStats(HypercubeControlLayer::BYTES_SENT));
            control->insert("receivedBytes", hypercubeControlLayer->getPacketStats(HypercubeControlLayer::BYTES_RECEIVED));

            qr->insert("ControlPackets", control);

        } else {
            qr->insert("universalAddress", uaddr.toString());
    
            vector<HypercubeMaskAddress>::const_iterator it;
            bool first = true;
            for (it = hypercubeControlLayer->getAddresses().begin(); it != hypercubeControlLayer->getAddresses().end(); it++) {
                qr->insert(first? "primaryAddress" : "secondaryAddress", it->toString());
                first = false;
            }
    
            qr->insert("MACAddress", physicalLayer->getAddress().toString());
        }

        return new CommandQueryResult(qr);
    }

    if (function.getName() == "assertPrimaryAddress") {
        string expected =  function.getStringParam(0);
        string got = hypercubeControlLayer->getPrimaryAddress().toString();

        if (expected != got) throw invalid_argument("assertPrimaryAddress failed. Expected " + expected + ", got " + got);

        return this;
    }

    if (function.getName() == "assertSecondaryAddresses") {
        const vector<HypercubeMaskAddress> &addr = hypercubeControlLayer->getAddresses();

        if ((addr.size() - 1) != function.getParamCount()) {
            throw invalid_argument("assertSecondaryAddresses failed. Expected " +
                toStr(function.getParamCount()) + " addresses, got " + toStr(addr.size()-1));
        }

        vector<string> got;
        for (int i = 1; i < addr.size(); i++) {
            got.push_back(addr[i].toString());
        }
        sort(got.begin(), got.end());

        vector<string> expected =  function.getParams();
        sort(expected.begin(), expected.end());

        if (expected != got) {
            string expectedStr= "[";
            string gotStr = "[";
            for (int i = 0; i < expected.size(); i++) {
                expectedStr += expected[i] + ",";
                gotStr += got[i] + ",";
            }
            expectedStr = expectedStr.substr(0, expectedStr.length() - 1) + "]";
            gotStr = gotStr.substr(0, gotStr.length() - 1) + "]";
            throw invalid_argument("assertSecondaryAddresses failed. Expected " + expectedStr + ", got " + gotStr);
        }

        return this;
    }


    if (function.getName() == "allConnections")
    {
        vector<TCommandRunner *>runners;

        map<MACAddress, TConnection*>::iterator it;

        for (it = physicalLayer->getConnections().begin(); it!= physicalLayer->getConnections().end(); it++) {
            runners.push_back(it->second);
        }

        return new MultiCommandRunner(runners);

    }
    if (function.getName() == "joinNetwork")
    {
        putMessage(new JoinNetworkMessage());
        return this;
    }

    if (function.getName() == "leaveNetwork")
    {
        putMessage(new LeaveNetworkMessage());
        return this;
    }

    if (function.getName() == "shutDown")
    {
        putMessage(new LeaveNetworkMessage());
        markedForDelete = true;
        return this;
    }

    if (function.getName() == "traceRoute")
    {
        return traceRoute;
    }

    if (function.getName() == "rendezVousServer")
    {
        return rendezVousServer;
    }

    if (function.getName() == "testApplication")
    {
        return testApplication;
    }

    if (function.getName() == "routing")
    {
        return hypercubeRoutingLayer->getRouting();
    }

    if (function.getName() == "setHBEnabled")
    {
        hypercubeControlLayer->setHBEnabled(function.getBoolParam(0));
        return this;
    }

    throw command_error("HypercubeNode - Bad function: " + function.toString());
}

/**
 * @brief Get the name of this object.
 *
 * @return "Node"
 */
string HypercubeNode::getName() const
{
    return "Node";
}

/**
 * @brief Get an identification for the node.
 *
 * @return the node universal address
 */
string HypercubeNode::getId() const
{
    return uaddr.toString();
}


/**
 * @brief Get a pointer to the node physical layer.
 *
 * @return a pointer to the node physical layer.
 */
PhysicalLayer *HypercubeNode::getPhyiscalLayer()
{
    return physicalLayer;
}

/**
 * @brief Get a pointer to the node hypercube control layer.
 *
 * @return a pointer to the node hypercube control layer.
 */
HypercubeControlLayer *HypercubeNode::getHypercubeControlLayer()
{
    return hypercubeControlLayer;
}

/**
 * @brief Get the node physical address
 *
 * @return the node physical address
 */
MACAddress HypercubeNode::getPhysicalAddress() const
{
    return physicalLayer->getAddress();
}


/**
 * @brief Get the node primary address
 *
 * @return the node primary address
 */
HypercubeAddress HypercubeNode::getPrimaryAddress() const
{
    return hypercubeControlLayer->getPrimaryAddress();
}

/**
 * @brief Get whether the node has the specified address as either primary or secondary.
 *
 * @return whether the node has the specified address as either primary or secondary.
 */
bool HypercubeNode::hasAddress(const HypercubeAddress &addr) const
{
    const vector<HypercubeMaskAddress> &allAddr = hypercubeControlLayer->getAddresses();
    for (int i = 0; i < allAddr.size(); i++)
        if (addr == allAddr[i]) return true;

    return false;
}

/**
 * @brief Get whether the node is currently connected.
 *
 * @return whether the node is currently connected.
 */
bool HypercubeNode::isConnected() const
{
    return getPrimaryAddress().getBitLength() > 0;
}

/**
 * @brief Get the mask of the primary address.
 *
 * return the mask of the primary address.
 */
int HypercubeNode::getPrimaryAddressMask() const
{
   return hypercubeControlLayer->getPrimaryAddress().getMask();
}



}}
