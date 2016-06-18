#ifndef _RENDEZVOUSCLIENT_H_
#define _RENDEZVOUSCLIENT_H_

#include <vector>
#include <iterator>
#include <map>

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
 * @brief Client for Rendez Vous.
 * It sends request to RV Server for looking up address, and takes care of queueing
 * the packet that need to be send in order to send them when the address is solved.
 */
class RendezVousClient : public HypercubeBaseApplication, public TTimeoutTarget {
    private:
        /// Structure to hold an element in the queue.
        typedef struct { 
            int sourcePort;
            int destPort;
            Data data;
            Time time;
            } TQueueData;

        typedef map<UniversalAddress, pair<HypercubeAddress, bool> > TCACHE;
        typedef multimap<UniversalAddress, TQueueData > TQUEUE;
        typedef map<int, UniversalAddress> TTIMEOUT;
        
    public:
        /// Port used for RendezVous Client Application
        const static int PORT;
        
        RendezVousClient(TTransportLayer *tl);
        virtual ~RendezVousClient();
                
        void send(const UniversalAddress &dest, int sourcePort, int destPort, const Data &data);
        virtual void receive(const TNetworkAddress &from, const TApplicationId &sourceAppId, const Data &data, const TPacket *packet);
        
        virtual void onTimeout(int id); 
        void addEntry(const UniversalAddress &uaddr, const HypercubeAddress &primaryAddr);
    private:            
        /// Cache of lookup addresses
        TCACHE cache;
        
        /// Queue of packets waiting for an address resolution to be sent.
        TQUEUE waitQueue;
    
        /// Which entries must be cleared for each timeout id
        TTIMEOUT cacheTimeouts;
        
        /// id for the timeout of a cache entry
        int nextTimeoutId;
};
      
      
}
}

#endif
