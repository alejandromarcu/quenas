#include <vector>
#include <iterator>

#include "ReactiveRouting.h"
#include "HCPacket.h"
#include "HypercubeControlLayer.h"
#include "MultiCommandRunner.h"
#include "CommandQueryResult.h"
#include "HypercubeNode.h"
#include "HypercubeParameters.h"

namespace simulator {
    namespace hypercube {
        namespace routing {

using namespace std;
using namespace simulator::hypercube;
using namespace simulator::hypercube::dataUnit;
      
//-------------------------------------------------------------------------
//--------------------------< ReactiveRouting >----------------------------
//-------------------------------------------------------------------------      
/**
 * @brief Constructor for a Reactive routing algorithm
 *
 * @param node node where this algorithm runs.
 */
ReactiveRouting::ReactiveRouting(HypercubeNode *node)
     : node(node), routingTable(node)

{    
    node->registerMessageListener(ConnectedMessage::TYPE, this);
    node->registerMessageListener(NewRouteMessage::TYPE, this);
    node->registerMessageListener(LostRouteMessage::TYPE, this);
    node->registerMessageListener(RouteChangedMaskMessage::TYPE, this);    
    mapping = new NeighbourMapping();
}
   
/**
 * @brief Route a packet.
 *
 * @param packet packet to route.
 * @param from where the packet came from.
 */    
HypercubeAddress ReactiveRouting::route(DataPacket *packet, const HypercubeAddress &from)
{
    // if the packet came as returned, try to send it to another neighbour
    if (packet->isReturned()) {
        packet->setReturned(false);

        return sendToNextNeighbour(packet, from, NULL);
    }
   
    // If the packet came back to the source node, return it.
    if (packet->getSourceAddress() == node->getPrimaryAddress() && from.getBitLength() > 0) {
        packet->setReturned(true);
        packet->setTTL(packet->getTTL() + 1);                                   
        return from;
    }


    Entry *reverseEntry = NULL;

    bool needsJoin = false;

    // insert or update the reverse entry, if there is a from address (i.e. is not the starting node)
    if (from.getBitLength() > 0) {
        int distance = DataPacket::MAX_TTL - packet->getTTL(); 

        RoutingTable::PAIR_MAP::iterator it = routingTable.pairs.find(make_pair(packet->getSourceAddress(), packet->getDestinationAddress()));
    
        if (it != routingTable.pairs.end()) { 
            if (distance > it->second.first->getDistance()) {
                packet->setReturned(true);
                packet->setTTL(packet->getTTL() + 1);                                   
                return from;
            }               
            reverseEntry = it->second.first;
            reverseEntry->setNextHop(from);
        } else {
    
             needsJoin = true;

            // find entries whose destination is the source address
            vector<Entry *> entries = routingTable.getEntries(packet->getSourceAddress());
    
            for (int i = 0; i < entries.size(); i++) {
                if (entries[i]->getNextHop() == from) {
                    reverseEntry = entries[i];
                    break;
                }
            } 

            if (reverseEntry == NULL) {
                // no entries found, add it
                Entry entry(packet->getSourceAddress(), from, distance, mapping);

                reverseEntry = routingTable.add(entry);
            } 
    
        }
        reverseEntry->setDistance(distance);
    }        

    // if the destination address is the current node, it arrived.
    if (packet->getDestinationAddress() == node->getPrimaryAddress()) {
        return packet->getDestinationAddress();
    }

    // find if there are entries to the packet destination
    vector<Entry *> entries = routingTable.getEntries(packet->getDestinationAddress());

    if (entries.size() == 0) {
        // if there aren't, it will find the first neighbour to send.
        return sendToNextNeighbour(packet, from, reverseEntry);
    }
    
    // there are already entries for the destination, so find the one with shortest path
    HypercubeAddress next;
    int minDist = DataPacket::MAX_TTL + 1;
    int entryIdx = -1;

    for (int i = 0; i < entries.size(); i++) {
        if (entries[0]->getDistance() < minDist) {
            next = entries[i]->getNextHop();
            minDist = entries[i]->getDistance();
            entryIdx = i;
        }
    }
    
    // if the entry is empty, there is no route to host from this node, so send it back
    if (next.getBitLength() == 0) {
        return next;
    }


    // if the next hop is the address where the packet came from, send it back as returned
    if (next == from) {
        packet->setReturned(true);
        packet->setTTL(packet->getTTL() + 1);                                   
       return from;
    }

    // if the next hop is not available anymore, send to the next neighbour.    
    int index = mapping->findIndex(next);
    if (index >= 0 && !mapping->isAvailable(index)) {
        return sendToNextNeighbour(packet, from, reverseEntry);
    }

    if (needsJoin) {   
        routingTable.pairs.insert(make_pair(make_pair(packet->getSourceAddress(), packet->getDestinationAddress()), 
                                            make_pair(reverseEntry, entries[entryIdx])));
    }

    // all the verifications passed, so the packet can be sent to the next hop stored in the route entry
    packet->setTTL(packet->getTTL()-1);
    return next;
}


/**
 * @brief Find the next neighbour to send the packet.
 * 
 * @param packet packet to send
 * @param from where the packet came from
 */
HypercubeAddress ReactiveRouting::sendToNextNeighbour(DataPacket *packet, const HypercubeAddress &from, Entry *reverseEntry)
{
    vector<Entry *> entries = routingTable.getEntries(packet->getDestinationAddress());
    Entry *entry;
    HypercubeAddress dest = packet->getDestinationAddress();
        
    if (entries.size() == 0) {
        // if there are no entries for the destination, create a new one
        routingTable.add(Entry(dest, mapping));
        entries = routingTable.getEntries(dest);
        entry = entries[0];
        
    }  else {
        // find the entry with the shortest distance.
        int minDist = DataPacket::MAX_TTL + 1;
        int best = -1;
        for (int i = 0; i < entries.size(); i++) {
            if (entries[0]->getDistance() < minDist) {
                best = i;
                minDist = entries[i]->getDistance();
            }
        }
        entry = entries[best];        
    }

    if (reverseEntry != NULL) {
        routingTable.pairs.insert(make_pair(make_pair(packet->getSourceAddress(), packet->getDestinationAddress()), 
                                            make_pair(reverseEntry, entry)));
    }
    
    VisitedBitmap *visited = entry->getVisitedBitmap();
    NeighbourMapping *nmap = visited->getMapping();

    //if the bitmap is new, add a timer to clear it
    if (visited->visitedCount() == 0) {
        routingTable.addTimer(false, entry);
    }
    
    // mark as visited the node where the packet came from
    for (int i = 0; i < nmap->size(); i++ ) {
        if (nmap->getAddress(i) == from) {
            visited->setVisited(i);
            break;
        }
    }

    // If all the neighbours are visited  
    bool allVisited = true;   
    for (int i = 0; i < nmap->size(); i++) {
        if (nmap->isAvailable(i) && !visited->isVisited(i)) allVisited = false;
    }
  
    if (allVisited) {     
        RoutingTable::PAIR_MAP::iterator it = routingTable.pairs.find(make_pair(packet->getSourceAddress(), packet->getDestinationAddress()));

        HypercubeAddress nh;

        if (it != routingTable.pairs.end()) { 
           packet->setReturned(true);
           packet->setTTL(packet->getTTL() + 1);                                   
           nh = it->second.first->getNextHop();
        }

        // Mark that there is no route for the destination
        for (int i = 0; i < entries.size(); i++) {
            entries[i]->setNextHop(nh);
        }
        
        return nh;
    }
    
    int nextHopIdx = -1;
    
    if (visited->visitedCount() <= HypercubeParameters::NEIGHBOURS_BEFORE_PARENT) {
        // find the neighbour that is closest to destination
        int bestDist = 10000;
        int bestMask = 10000;
        for (int i = 0; i < nmap->size(); i++) {            
            if (!visited->isVisited(i) && nmap->isAvailable(i)) {
                int d =  packet->isRendezVous() ? nmap->getAddress(i).distanceWithMask(dest) :
                                                  nmap->getAddress(i).distance(dest);
                if (d < bestDist || (d == bestDist && nmap->getAddress(i).getMask() < bestMask)) {
                    nextHopIdx = i;
                    bestDist = d;
                    bestMask = nmap->getAddress(i).getMask();
                }
            }
        }
    } else {           
        if (!visited->isVisited(0) && nmap->isAvailable(0)) {
            nextHopIdx = 0;
        } else {
            // closest neighbour and parent were visited, so find the next available
            for (int i = 0; i < nmap->size(); i++) {
                if (!visited->isVisited(i) && nmap->isAvailable(i)) {   
                    nextHopIdx = i;
                    break;                 
                }            
            }
        }
    }
    
    if (nextHopIdx < 0) throw logic_error("expected to find a next hop");

    entry->setNextHop(nmap->getAddress(nextHopIdx));
            
    visited->setVisited(nextHopIdx);
    packet->setTTL(packet->getTTL() - 1);
    return nmap->getAddress(nextHopIdx);
    
}

/**
 * @brief Run a command.
 * 
 * @param function fucntion to run.
 */
TCommandResult *ReactiveRouting::runCommand(const Function &function)
{
   if (function.getName() == "table")  
    {
        return &routingTable;
    }
     
   throw new invalid_argument("ReactiveRouting: unknown command " + function.getName());
}

/**
 * @brief Called when a node message is received.
 * It receives messages about the changes in neighours and passes this information to the mapping.
 * 
 * @param message received message
 */
void ReactiveRouting::onMessageReceived(const TMessage *message)
{    
    if (message->getType() == ConnectedMessage::TYPE) {
        const ConnectedMessage *cm = dynamic_cast<const ConnectedMessage *>(message);
        if (cm->getParentAddress().getBitLength() > 0)   mapping->setParent(cm->getParentAddress());
        
    } else if (message->getType() == NewRouteMessage::TYPE) {
        const NewRouteMessage *nrm = dynamic_cast<const NewRouteMessage *>(message);
        mapping->add(nrm->getRoute());

    } else if (message->getType() == LostRouteMessage::TYPE) {
        const LostRouteMessage *nrm = dynamic_cast<const LostRouteMessage *>(message);
        int n = mapping->findIndex(nrm->getRoute());
        if (n >= 0) mapping->setAvailable(n, false);

    } else if (message->getType() == RouteChangedMaskMessage::TYPE) {
        const RouteChangedMaskMessage *m = dynamic_cast<const RouteChangedMaskMessage *>(message);
        mapping->changeMask(m->getRoute());
    }
}

/**
 * @brief Get this object name.
 *
 * @return "ReactiveRouting"
 */
string ReactiveRouting::getName() const 
{ 
    return "ReactiveRouting"; 
}

//-------------------------------------------------------------------------
//----------------------------< RoutingTable >-----------------------------
//-------------------------------------------------------------------------      
/**
 * @brief Create a routing table.
 *
 * @brief node node where the routing table is.
 */
RoutingTable::RoutingTable(TNode *node) : node(node), nextTimeoutId(1) 
{
}

/**
 * @brief Add an entry to the routing table.
 *
 * @param entry entry to add.
 */
Entry* RoutingTable::add(const Entry &entry)
{
    multimap<HypercubeAddress, Entry>::iterator it = entries.insert(make_pair(entry.getDestination(), entry));
    Simulator::getInstance()->notify("node.routing.table.added", &entry, NULL, node);        

    addTimer(true, &(it->second));
    addTimer(false, &(it->second));    
        
    return &(it->second);
}

/**
 * @brief Get all the entries with a specific destination.
 *
 * @param dest destination to search
 * @return all the entries with the specificied destination.
 */
vector<Entry *> RoutingTable::getEntries(const HypercubeAddress &dest)
{
    vector<Entry *> result;
    multimap<HypercubeAddress, Entry>::iterator it = entries.find(dest);
    while ((it != entries.end()) && (it->first == dest)) {
        result.push_back(&(it->second));
        it++;
    }
    return result;
}

/**
 * @brief Replace an entry in the routing table by another.
 *
 * @param oldEntry entry to replace.
 * @param newEntry new entry to put in place.
 */
void RoutingTable::replace(const Entry &oldEntry, const Entry &newEntry)
{
    multimap<HypercubeAddress, Entry>::iterator it = entries.find(oldEntry.getDestination());

    while ((it != entries.end()) && (it->first == oldEntry.getDestination())) {
        if (it->second == oldEntry) it->second = newEntry;
        it++;
    }
    Simulator::getInstance()->notify("node.routing.table.replaced", &newEntry, NULL, node);        
}

/**
 * @brief Remove an entry from the routing table.
 *
 * @param entry entry to remove.
 */
void RoutingTable::remove(Entry *entry)
{
    Simulator::getInstance()->notify("node.routing.table.removed", entry, NULL, node);        

    multimap<HypercubeAddress, Entry>::iterator it = entries.find(entry->getDestination());

    while ((it != entries.end()) && (it->first == entry->getDestination())) {
        if (it->second == *entry) {
            entries.erase(it);         
            return; 
        }
        it++;
    }
}

/**
 * @param Run a command in the routing table.
 *
 * @param function function to run.
 */
TCommandResult *RoutingTable::runCommand(const Function &function)
{
   if (function.getName() == "query")  
    {     
        vector<string> params = function.getParams();
        return new CommandQueryResult(query(&params)); 
    }
     
}

/**
 * @brief Get information about the routing table, composed by its entries.
 *
 * @param options options for the query, not used.
 */
QueryResult *RoutingTable::query(const vector<string> *options) const   
{
    QueryResult *qr = new QueryResult("RoutingTable");

    if (options->size() > 0 && (*options)[0] == "size") {

       qr->insert("size", toStr(entries.size()));

    } else {    
        multimap<HypercubeAddress, Entry>::const_iterator it = entries.begin();
        for (; it != entries.end(); it++) {
            qr->insert("Entry", it->second.query());        
        }
    }
    return qr;    
}

/**
 * @brief This method is called when a timeout expires. It is used to erase
 * entries or their bitmaps.
 *
 * @param id id of the timeout that has expired.
 */
void RoutingTable::onTimeout(int id)
{
    map<int, pair<bool, Entry*> >::iterator tit = timeouts.find(id);

    if (tit == timeouts.end()) {
        return;
    }
    
    bool clearEntry = tit->second.first;
    Entry *entry = tit->second.second;
    
    timeouts.erase(tit);

    // Find the entry, needed either to erase from multimap, or confirm that it exist.
    multimap<HypercubeAddress, Entry>::iterator it = entries.begin();
    while (it != entries.end()) {
        if (&(it->second) == entry) {
            break;
        }
        it++;
    }    

    // entry not found, it was erased.
    if (it == entries.end()) return;
    
    if (clearEntry) {
        RoutingTable::PAIR_MAP::iterator itp = pairs.begin();
        while (itp != pairs.end()) {
            if (itp->second.first == entry || itp->second.second == entry) {
               PAIR_MAP::iterator itp2 = itp++;
               pairs.erase(itp2);
            } else {
               itp++;
            }
        }

        // Find the entry and clear it
        entries.erase(it); 
    } else {
        // just clear the bitmap
        entry->getVisitedBitmap()->clear();
    }
}

/**
 * @brief helper method to add a timer.
 *
 * @param clearEntry whether to clear the entry (if false, clears the bitmap)
 * @param e entry to clear (or holding the bitmap to clear)
 */
void RoutingTable::addTimer(bool clearEntry, Entry *e)
{
    nextTimeoutId++;
    timeouts[nextTimeoutId] = make_pair(clearEntry, e);
    
    TimeoutEvent *event = new TimeoutEvent(clearEntry? 
        HypercubeParameters::ROUTING_TABLE_ENTRY_CLEAR_PERIOD :
        HypercubeParameters::ROUTING_TABLE_BITMAP_CLEAR_PERIOD, this, nextTimeoutId); 
    Simulator::getInstance()->addEvent(event, true);    
}

/**
 * @brief Get this object name.
 *
 * @return "RoutingTable"
 */ 
string RoutingTable::getName() const 
{ 
    return "RoutingTable"; 
}



}}}

