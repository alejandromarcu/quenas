#ifndef _ROUTE_HEADER_H
#define _ROUTE_HEADER_H

#include <vector>
#include <iterator>

//#include "common.h"
//#include "DataUnit.h"
//#include "MACAddress.h"
#include "HypercubeAddress.h"
#include "HCPacket.h"

namespace simulator {
    namespace hypercube {
        namespace dataUnit {

using namespace std;

       
/**
 * @brief Optional header used to trace a route
 */        
class RouteHeader : public TOptionalHeader {
    public:
        /// Type of header
        const static byte HEADER_TYPE;
        
        RouteHeader();        
        RouteHeader(const HypercubeAddress &address, bool isReturned = false);                       
        RouteHeader(ConstItVB &it);
        
        HypercubeAddress getAddress() const;
        bool isReturned() const;
        
        virtual void dumpTo(BackItVB it) const;

        virtual QueryResult *query(const vector<string> *options = NULL) const;
    private:
        // hop address
        HypercubeAddress address;
        
        // time that the packet was received
        Time time;
};



}
}
}

#endif
