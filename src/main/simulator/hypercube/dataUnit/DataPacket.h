#ifndef _DATAPACKET_H_
#define _DATAPACKET_H_

#include <vector>
#include <iterator>

#include "DataUnit.h"
#include "RouteHeader.h"

namespace simulator {
    namespace hypercube {
        namespace dataUnit {

/**
 * @brief Class for Data packets for Hypercube Protocol
 */        
class DataPacket : public TPacket, public TQueryable {
    public:        
        const static EthernetType ETHERNET_TYPE;
        const static int16 MAX_TTL;
        
        DataPacket();
        DataPacket(const HypercubeAddress &source, const HypercubeAddress &dest,
                   TransportType transportProtocol, const TSegment &segment,
                   int16 ttl=MAX_TTL, bool returned=false);
                   
        virtual ~DataPacket();
        
        HypercubeAddress getSourceAddress() const;
        HypercubeAddress getDestinationAddress() const;
        TransportType getTransportType() const;

        int16 getTTL() const;        
        void setTTL(int16 ttl);
        
        bool isReturned() const;
        void setReturned(bool returned);

        bool isTraceRoute() const;
        void setTraceRoute(bool traceRoute);
        
        bool isRendezVous() const;
        void setRendezVous(bool rendezVous);

        bool isUnloop() const;
        void setUnloop(bool unloop);
        
        virtual void dumpTo(BackItVB it) const;
        virtual void read(const TFrame &frame);
        
        void addRouteHeader(const RouteHeader &rh);
        vector<RouteHeader> getRoute() const;
        
        virtual QueryResult *query(const vector<string> *options = NULL) const;    
    private:
        void setFlag(int n, bool value);
        bool getFlag(int n) const;
        
        /// Total length of the packet without optional headers
        int16 totalLength;
        
        /// flags
        byte flags;
        
        /// Time To Live
        int16 ttl;
                
        /// When the trace route flag is set, route is pushed here.
        vector<RouteHeader> route;
        
        /// source address of the packet
        HypercubeAddress source;
        
        /// destination address of the packet
        HypercubeAddress dest;        
        
        /// transport protocol for the packet
        TransportType protocol;
        
};



}
}
}

#endif
