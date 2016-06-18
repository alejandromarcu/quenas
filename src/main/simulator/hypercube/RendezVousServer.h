#ifndef _RENDEZVOUSSERVER_H_
#define _RENDEZVOUSSERVER_H_

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
#include "RendezVousPacket.h"

namespace simulator {
    namespace hypercube {


/**
 * @brief Rendez Vous server application.
 * It takes care of managing rendez vous addresses.
 */
class RendezVousServer : public HypercubeBaseApplication, public TCommandRunner,
                         public TQueryable, public TMessageReceiver, public TTimeoutTarget {
    private:
        /// Type for the lookup table
        typedef map<UniversalAddress, HypercubeAddress> TLOOKUP;

    public:
        /// Port used for RendezVous Server Application        
        const static int PORT;
        
        RendezVousServer(TTransportLayer *tl);
        virtual ~RendezVousServer();
        virtual TCommandResult *runCommand(const Function &function);
        virtual QueryResult *query(const vector<string> *options = NULL) const;    
        
        virtual string getName() const;                
        
        virtual void onMessageReceived(const TMessage *message);        
        
        virtual void receive(const TNetworkAddress &from, const TApplicationId &sourceAppId, const Data &data, const TPacket *packet);        
        
        virtual void onTimeout(int id); 
        
    private:
        /// Lookup table
        TLOOKUP lookup;
        
        /// True if the node is going to disconnect
        bool willDisconnect;
        
        /// Lookup tables that were sent but still aren't confirmed.
        vector<RendezVousLookupTable> pendingSentTables;
        
        /// Address of the parent node
        HypercubeAddress parentAddress;
};
      
      
}
}

#endif
