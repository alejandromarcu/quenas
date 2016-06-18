#ifndef _TCONNECTION_H
#define _TCONNECTION_H

#include "DataUnit.h"
#include "TPhysicalLayer.h"
#include "Command.h"
#include "Units.h"

namespace simulator {
    namespace layer {
        
using namespace simulator::dataUnit;
using namespace simulator::command;

/**
 * @brief Base class for connections between TPhysicalLayers.
 */        
class TConnection : public TCommandRunner {
    public:
        /**
         * @brief Virtual destructor that does nothing.
         */
        virtual ~TConnection() {}
        
        /**
         * @brief Inheriting classes must implement this method so that all the connected
         * physical layers except "from" receive the bit stream.
         */
        virtual void transport(TPhysicalLayer *from, const TBitStream &bitStream) = 0;
        
        /**
         * @brief Retrieves all the end points of this connection.
         */
        virtual vector<TPhysicalLayer *> getPoints() const = 0;
        
        /**
         * @brief Retrieves the bandwidth of this connection.
         */
        virtual Bandwidth getBandwidth() const = 0;
                
};
        
    }
}

#endif
