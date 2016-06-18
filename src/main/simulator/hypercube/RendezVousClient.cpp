#include <vector>
#include <iterator>


#include "RendezVousClient.h"
#include "RendezVousPacket.h"
#include "common.h"
#include "HCPacket.h"
#include "Message.h"
#include "Units.h"
#include "Event.h"
#include "StateMachines.h"
#include "HypercubeNode.h"
#include "CommandQueryResult.h"
#include "HypercubeParameters.h"

namespace simulator {
    namespace hypercube {

using namespace std;
using namespace simulator::hypercube::dataUnit;
using namespace simulator::message;
using namespace simulator::event;
      
const int RendezVousClient::PORT = 9903;
      
        
/**
 * @brief Create a RendezVousClient Application and bind it to the transport layer.
 *
 * @param tl pointer to the Transport Layer below this application.
 */        
RendezVousClient::RendezVousClient(TTransportLayer *tl) : HypercubeBaseApplication(tl), nextTimeoutId(1)
{
    bind(PORT);
}

/**
 * @brief Unbind the application from the port.
 */
RendezVousClient::~RendezVousClient()
{
    unbind();
}

/**
 * @bried Send a packet to an universal address.
 * If the address is in the lookup cache, it sends it right now, if it is not there,
 * it sends a request for solving the address and puts the packet in the wait queue
 * to send it automatically when the address is solved.
 *
 * @param dest destination address
 * @param sourcePort source port 
 * @param destPort destination port 
 * @param data data to send.
 */
void RendezVousClient::send(const UniversalAddress &dest, int sourcePort, int destPort, const Data &data)
{
    TCACHE::iterator it = cache.find(dest);
   
    if (it != cache.end()) {
        // The Universal Address is in the cache, send it right now
        transportLayer->send(it->second.first, Port(sourcePort), Port(destPort),  data);
        
        // mark as used in the perido
        it->second.second = true;
    } else {
        // The Universal Address is not in the cache...
        // Put the information to send it later in the wait queue        
        TQueueData q = {sourcePort, destPort, data, Simulator::getInstance()->getTime()};
        waitQueue.insert(make_pair(dest, q));
        
        // Calculate the RV node for solving the addres
        int size = dynamic_cast<HypercubeNode *>(getNode())->getPrimaryAddress().getBitLength();                
        HypercubeAddress rvNode = dest.hashToHypercube(size);

        // Send the request
        RendezVousAddressSolve solve(dest);
        Data d(solve.getData());
        transportLayer->send(rvNode, Port(PORT), Port(RendezVousServer::PORT), d);                        
    }
}

/**
 * @brief Receive data.
 * If it receives a reply to a lookup request, then it adds the address to the cache
 * and sends all the packets that were waiting for it.
 *
 * @param from address of the sending node
 * @param sourceAppId the Application Id (port) of the sender
 * @param data data received
 */
void RendezVousClient::receive(const TNetworkAddress &from, const TApplicationId &sourceAppId, const Data &data, const TPacket *packet)
{
    TRendezVousPacket *rvp = TRendezVousPacket::create(data.getPayload());
    
    if (rvp->getType() == RendezVousAddressLookup::TYPE) {
        RendezVousAddressLookup *al = dynamic_cast<RendezVousAddressLookup *>(rvp);        

        if (al->isSolved()) {
            addEntry(al->getUniversalAddress(), al->getPrimaryAddress());
            
            TQUEUE::iterator it = waitQueue.find(al->getUniversalAddress());

            Time minTime(Simulator::getInstance()->getTime());

            // find all the packets that can be sent           
            while ((it != waitQueue.end()) && (it->first == al->getUniversalAddress())) {
                TQueueData q = it->second;
                transportLayer->send(al->getPrimaryAddress(), Port(q.sourcePort), Port(q.destPort),  q.data);

                if (q.time < minTime) minTime = q.time;
                
                TQUEUE::iterator it2 = it++;
                waitQueue.erase(it2);
            }

            Time elapsed(Simulator::getInstance()->getTime().getValue() - minTime.getValue());
            QueryResult *qr = new QueryResult("elapsedTime", elapsed.toString(Time::SEC));
            Simulator::getInstance()->notify("node.rvclient.solved", qr, transportLayer->getNode());        

        }
    }
    delete rvp;
}

/**
 * @brief Add an entry in the lookup table, and set a timeout for cleaning it later.
 *
 * @param uaddr universal address of the node
 * @param primaryAddr primary  address of the node
 */
void RendezVousClient::addEntry(const UniversalAddress &uaddr, const HypercubeAddress &primaryAddr)
{    
    // Store the lookup in the cache.
    cache[uaddr] = make_pair(primaryAddr, false);
    
    // Ask for a timeout for this entry
    cacheTimeouts.insert(make_pair(nextTimeoutId, uaddr));
    
    TimeoutEvent *event = new TimeoutEvent(HypercubeParameters::RENDEZ_VOUS_CLIENT_CACHE_CLEANING_PERIOD, this, nextTimeoutId++); 
    Simulator::getInstance()->addEvent(event, true);        
}

/**
 * @brief Run when a timeout for an entry in the lookup table is triggered.
 * 
 * @param id of the timeout
 */
void RendezVousClient::onTimeout(int id)
{
    TTIMEOUT::iterator it = cacheTimeouts.find(id);

    if (it == cacheTimeouts.end()) return;
    
    TCACHE::iterator entry = cache.find(it->second);
    
    if (entry == cache.end()) return;
    
    // If the entry was used, re enter it so it isn't expired
    if (entry->second.second) {
       addEntry(entry->first, entry->second.first);
    }  else {
       // the entry was not used, erase it
       cache.erase(entry);
       cacheTimeouts.erase(it);
    }
}







}
}

