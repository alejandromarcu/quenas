#ifndef _REACTIVE_ROUTING_H
#define _REACTIVE_ROUTING_H

#include "HypercubeControlLayer.h"
#include "HCPacket.h"
#include "TRoutingAlgorithm.h"
#include "Entry.h"
#include "NeighbourMapping.h"
#include "HypercubeNode.h"

namespace simulator {
    namespace hypercube {
        namespace routing {

using namespace std;
using namespace simulator::hypercube;
using namespace simulator::hypercube::dataUnit;



/**
 * @brief Routing Table for reactive Routing algorithm.
 */
class RoutingTable : public TCommandRunner, public TQueryable, public TTimeoutTarget {
    public:
        typedef pair<Entry*, Entry* > ENTRY_PAIR;
        typedef pair<HypercubeAddress, HypercubeAddress> ADDRESS_PAIR;
        typedef map<ADDRESS_PAIR, ENTRY_PAIR> PAIR_MAP;

        RoutingTable(TNode *node);
        
        Entry* add(const Entry &entry);
        vector<Entry *> getEntries(const HypercubeAddress &dest);
        void replace(const Entry &oldEntry, const Entry &newEntry);
        void remove(Entry *entry);

        virtual TCommandResult *runCommand(const Function &function);
        virtual string getName() const;
        virtual QueryResult *query(const vector<string> *options = NULL) const;    
        
        virtual void onTimeout(int id);    
        void addTimer(bool clearEntry, Entry *e);

        PAIR_MAP pairs;
        
    private:
        ///  which entry to erase for each timeout id (bool param indicates if clear entry or just bitmap)
        map<int, pair<bool, Entry*> > timeouts;
        
        /// used to generate unique ids for timeouts.
        int nextTimeoutId;
        
        /// Routing table, easily searched by primary address        
        multimap<HypercubeAddress, Entry> entries;
        

        /// Pointer to the node holding this routing table.
        TNode *node;                
};

/**
 * @brief Implementation of the Reactive Routing algorithm
 */
class ReactiveRouting : public TRoutingAlgorithm, TMessageReceiver {
    public:
        ReactiveRouting(HypercubeNode *node);
        virtual HypercubeAddress route(DataPacket *packet, const HypercubeAddress &from=HypercubeAddress());

        virtual TCommandResult *runCommand(const Function &function);
        virtual string getName() const;

        virtual void onMessageReceived(const TMessage *message);
        
    private:
        HypercubeAddress sendToNextNeighbour(DataPacket *packet, const HypercubeAddress &from, Entry *reverseEntry);

        /// Mapping used for the visited bitmap to match neighbours.
        NeighbourMapping *mapping;

        /// Routing table
        RoutingTable routingTable;
        
        /// Pointer to the node holding this algorithm class.
        HypercubeNode *node;
        
};
      
      
}
}
}
#endif
