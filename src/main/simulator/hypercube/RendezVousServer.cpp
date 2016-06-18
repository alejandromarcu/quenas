#include <vector>
#include <iterator>


#include "RendezVousServer.h"
#include "RendezVousPacket.h"
#include "common.h"
#include "HCPacket.h"
#include "HypercubeControlLayer.h"
#include "Message.h"
#include "Units.h"
#include "Event.h"
#include "StateMachines.h"
#include "HypercubeNode.h"
#include "CommandQueryResult.h"
#include "Exceptions.h"
#include "HypercubeParameters.h"

namespace simulator {
    namespace hypercube {

using namespace std;
using namespace simulator::hypercube::dataUnit;
using namespace simulator::message;
using namespace simulator::event;
      
const int RendezVousServer::PORT = 9902;
      
/**
 * @brief Create a RendezVousServer Application, bind it to the transport layer and
 * registers itself in the node for the required messages.
 *
 * @param tl pointer to the Transport Layer below this application.
 */           
RendezVousServer::RendezVousServer(TTransportLayer *tl) : HypercubeBaseApplication(tl), willDisconnect(false)
{
    bind(PORT);
    tl->getNode()->registerMessageListener(ConnectedMessage::TYPE, this);
    tl->getNode()->registerMessageListener(WillDisconnectMessage::TYPE, this);    
    tl->getNode()->registerMessageListener(AddressGivenMessage::TYPE, this);    
}

/**
 * @brief Unbind the application from the port.
 */
RendezVousServer::~RendezVousServer()
{
    unbind();
}

/**
 * @brief Run a Command.
 *
 * @param function the function to run.
 * @return a pointer to the next command result to execute.
 */        
TCommandResult *RendezVousServer::runCommand(const Function &function)
{
   if (function.getName() == "query")  
   {
       vector<string> params = function.getParams();
       return new CommandQueryResult(query(&params)); 
   }

   if (function.getName() == "sendRegister") 
   {
        HypercubeNode *node = dynamic_cast<HypercubeNode *>(getNode());
        int size = node->getPrimaryAddress().getBitLength();

        // Calculate the RV node for the node universal address...
        HypercubeAddress rvNode = node->getUniversalAddress().hashToHypercube(size);

        // ...and send it a Register packet.
        RendezVousRegister rvr(node->getPrimaryAddress(), node->getUniversalAddress());
        Data data(rvr.getData());
        transportLayer->send(rvNode, Port(PORT), Port(PORT), data);                
        return this;
    }
 
    
    throw command_error("RendezVousServer: Invalid function: " + function.getName());    
}
     

/**
 * @brief Query the Rendez Vous Server.
 *
 * @param options options for the query, not used.
 * @return a QueryResult with the entries in the lookup table.
 */
QueryResult *RendezVousServer::query(const vector<string> *options) const   
{
    QueryResult *qr = new QueryResult("RendezVousServer");
    TLOOKUP::const_iterator it = lookup.begin();

    if (options->size() > 0 && (*options)[0] == "size") {
       const HypercubeNode *n = dynamic_cast<const HypercubeNode *>(getNode());

       qr->insert("size", toStr(lookup.size()));
       qr->insert("nodeMask", toStr(n->getPrimaryAddressMask()));

    } else {
        for (; it != lookup.end(); it++) {
            QueryResult *entry = new QueryResult("Entry");
            entry->insert("node", it->first.toString()); 
            entry->insert("address", it->second.toString());        
            qr->insert("", entry);
            
        }
    }
    return qr;    
}
/**
 * @brief Get the object name
 *
 * @return "RendezVousServer"
 */
string RendezVousServer::getName() const
{
    return "RendezVousServer";
}

/**
 * @brief Receive data and process it.
 *
 * @param from address of the sending node
 * @param sourceAppId the Application Id (port) of the sender
 * @param data data received
 */
void RendezVousServer::receive(const TNetworkAddress &from, const TApplicationId &sourceAppId, const Data &data, const TPacket *packet)
{
    HypercubeNode *node = dynamic_cast<HypercubeNode *>(getNode());    
    TRendezVousPacket *rvp = TRendezVousPacket::create(data.getPayload());
    const DataPacket *p = dynamic_cast<const DataPacket *>(packet);

    // Got a RV Register, add it in the lookup table
    if (rvp->getType() == RendezVousRegister::TYPE) {
        RendezVousRegister *rvr = dynamic_cast<RendezVousRegister *>(rvp);
        lookup[rvr->getUniversalAddress().toString()] = rvr->getPrimaryAddress();
        QueryResult *qr = new QueryResult("client");
        qr->insert("universalAddress", rvr->getUniversalAddress().toString());
        qr->insert("primaryAddress", rvr->getPrimaryAddress().toString());     
        if (p != NULL) {
           HypercubeNetwork *hn = dynamic_cast<HypercubeNetwork *>(Simulator::getInstance()->getNetwork());
           qr->insert("distance", toStr(DataPacket::MAX_TTL - p->getTTL()));     
           qr->insert("shortestPath", toStr(hn->getShortestPath(rvr->getUniversalAddress(), node->getUniversalAddress() ,false)));     
           qr->insert("shortestPathAllConnections", toStr(hn->getShortestPath(rvr->getUniversalAddress(), node->getUniversalAddress(),true)));     
        }
        Simulator::getInstance()->notify("node.rvserver.register", qr, transportLayer->getNode());        
    }

    // Got a RV deregister, erase the entry from the lookup table
    if (rvp->getType() == RendezVousDeregister::TYPE) {
        RendezVousDeregister *rvd = dynamic_cast<RendezVousDeregister *>(rvp);
        
        string name = rvd->getUniversalAddress().toString();
        
        TLOOKUP::iterator it = lookup.find(name);
        
        if (it == lookup.end()) throw logic_error("Trying to deregister but the registered node does not exist: " + name);
        
        lookup.erase(it);
        QueryResult *qr = new QueryResult("client");
        qr->insert("universalAddress", rvd->getUniversalAddress().toString());
        qr->insert("primaryAddress", rvd->getPrimaryAddress().toString());        
        Simulator::getInstance()->notify("node.rvserver.unregister", qr, transportLayer->getNode());        
    }

    // A client asked for solving an address, so search in the lookup table and reply
    if (rvp->getType() == RendezVousAddressSolve::TYPE) {
        RendezVousAddressSolve *solve = dynamic_cast<RendezVousAddressSolve *>(rvp);
        
        // Search the address (will be empty if not found)
        HypercubeAddress addr;
        TLOOKUP::iterator it = lookup.find(solve->getUniversalAddress());
        if (it != lookup.end()) addr = it->second;

        // Reply to the client
        RendezVousAddressLookup rv(addr, solve->getUniversalAddress(), it != lookup.end());
        Data data(rv.getData());
        transportLayer->send(from, Port(PORT), Port(RendezVousClient::PORT), data);                
    }
    
    // Got a RV lookup table, so add the entries to the lookup table and acknowledge
    if (rvp->getType() == RendezVousLookupTable::TYPE) {
        RendezVousLookupTable *table = dynamic_cast<RendezVousLookupTable *>(rvp);

        // Add the entries
        for (int i = 0; i < table->getTable().size(); i++) {
            lookup[table->getTable()[i].second] = table->getTable()[i].first;
        }

        // Acknowledge that the table was received and added
        RendezVousLookupTableReceived ack(table->getId());
        Data data(ack.getData());
        transportLayer->send(from, Port(PORT), Port(PORT), data);                
    }
    
    // An ack that the sent table was received, so clear the entries of the sent table
    if (rvp->getType() == RendezVousLookupTableReceived::TYPE) {
        int id = dynamic_cast<RendezVousLookupTableReceived *>(rvp)->getId();

        // Find which packet was acknowledged
        vector<RendezVousLookupTable>::iterator it = pendingSentTables.begin();        
        while (it != pendingSentTables.end()) {
            if (it->getId() == id) break;
            it++;
        }
        
        // found the acknowledged packet?
        if (it != pendingSentTables.end()) {

            // erase all the entries of the acknowledged packet
            for (int i = 0; i < it->getTable().size(); i++) {            
                TLOOKUP::iterator entry = lookup.find(it->getTable()[i].second);
                
                if (entry != lookup.end()) lookup.erase(entry);
            }
            
            // the sent table is not pending anymore
            pendingSentTables.erase(it);
        }
        
        // if the node is willing to disconnect, put a message to indicate that
        // the RV is ready for disconnect.
        if (willDisconnect) {
            node->putMessage(new ReadyForDiscMessage(PORT));
        }
    }
        
    delete rvp;
}    
    
/**
 * @brief A message is received from the node itself, so process it.
 *
 * @param message received message.
 */    
void RendezVousServer::onMessageReceived(const TMessage *message)
{
    HypercubeNode *node = dynamic_cast<HypercubeNode *>(getNode());
    
    // The node just got connected, then register in RV
    if (message->getType() == ConnectedMessage::TYPE) {
        const ConnectedMessage *msg = dynamic_cast<const ConnectedMessage *>(message);      
        int size = msg->getPrimaryAddress().getBitLength();

        parentAddress = msg->getParentAddress();
        
        // Calculate the RV node for the node universal address...
        HypercubeAddress rvNode = node->getUniversalAddress().hashToHypercube(size);

        // ...and send it a Register packet.
        RendezVousRegister rvr(msg->getPrimaryAddress(), node->getUniversalAddress());
        Data data(rvr.getData());
        transportLayer->send(rvNode, Port(PORT), Port(PORT), data);                
    }

    if (message->getType() == WillDisconnectMessage::TYPE) {
        int size = node->getPrimaryAddress().getBitLength();

        willDisconnect = true;

        // Ask the node to wait for the RV Server to disconnect, so that
        // it can send the tables and be sure that the other end got them.
        node->putMessage(new WaitMeMessage(PORT));

          
        // Put all the lookup table in a RendezVousLookupTable packet and send it
        RendezVousLookupTable table; 
        for (TLOOKUP::iterator it = lookup.begin(); it != lookup.end(); it++) {
            HypercubeAddress rvNode = it->first.hashToHypercube(node->getPrimaryAddress().getBitLength());
            table.add(rvNode, it->first);
        }                  

        // Send the table to the parent
        Data dataTable(table.getData());
        transportLayer->send(parentAddress, Port(PORT), Port(PORT), dataTable);                
        
        
        // De register the node 
        HypercubeAddress rvNode = node->getUniversalAddress().hashToHypercube(size);

        RendezVousDeregister rvr(node->getPrimaryAddress(), node->getUniversalAddress());
        Data data(rvr.getData());
        transportLayer->send(rvNode, Port(PORT), Port(PORT), data);                
        
        // Add a timeout, in case that we don't get acknowledged the table, we disconnect anyways.
        TimeoutEvent *event = new TimeoutEvent(HypercubeParameters::RENDEZ_VOUS_LOOKUP_TABLE_RECEIVED_TIMEOUT , this, 0);
        Simulator::getInstance()->addEvent(event, true);                
    }

    // The node has a new child, so it must send the entries of the RV belonging to the child space.
    if (message->getType() == AddressGivenMessage::TYPE) {
        const AddressGivenMessage *msg = dynamic_cast<const AddressGivenMessage *>(message);            
        
        RendezVousLookupTable table; 
          
        // Iterate through the RV lookup table and find entries that should go to the new child
        for (TLOOKUP::iterator it = lookup.begin(); it != lookup.end(); it++) {
            HypercubeAddress rvNode = UniversalAddress(it->first).hashToHypercube(node->getPrimaryAddress().getBitLength());
            
            if (msg->getGivenAddress().contains(rvNode)) {
                table.add(it->second, it->first);
            }
        }                  
        
        // If there is at least one entry, send it.
        if (table.getTable().size() > 0) {
            Data data(table.getData());
            transportLayer->send(msg->getDestination(), Port(PORT), Port(PORT), data);

            // add in the list of pending tables, and when the confirmation 
            // is received, it erases the entries.
            pendingSentTables.push_back(table);
        }
    }    

}


/**
 * @brief The RendezVousLookupTableReceived packet was not received and the node
 * wants to disconnect, so disconnect anyways!
 * 
 * @param id of the timeout
 */
void RendezVousServer::onTimeout(int id) 
{
    dynamic_cast<HypercubeNode *>(getNode())->putMessage(new ReadyForDiscMessage(PORT));
}



}
}

