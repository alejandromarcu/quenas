#ifndef _CONNECTION_H
#define _CONNECTION_H

#include "TConnection.h"
#include "DataUnit.h"
#include "TPhysicalLayer.h"
#include "common.h"
#include "Units.h"
#include "Event.h"
#include "Simulator.h"
#include "Command.h"

namespace simulator {
    namespace layer {
        
using namespace simulator;        
using namespace simulator::command;        
using namespace simulator::dataUnit;
using namespace simulator::event;
        
/**
 * @brief Represents a connection between 2 nodes.
 *
 * The connection can also have a delay.
 */        
class Connection : public TConnection {
    public:
        Connection(TPhysicalLayer *point1, TPhysicalLayer *point2, 
                   Bandwidth bandwidth = 0, Time delay = 0);
        virtual ~Connection();
        
        virtual void transport(TPhysicalLayer *from, const TBitStream &bitStream);

        virtual vector<TPhysicalLayer *> getPoints() const;
        virtual Bandwidth getBandwidth() const;
        virtual Time getDelay() const;

        virtual TCommandResult *runCommand(const Function &f);
        
        virtual string getName() const;
        virtual string getId() const;
             
        virtual void setBandwidth(Bandwidth bw);
        virtual void setDelay(Time d);
    private:
        /// one endpoint of this connection
        TPhysicalLayer *point1;

        /// the other endpoint of this connection        
        TPhysicalLayer *point2;
        
        /// delay for the connection.        
        Time delay;
        
        /// bandwidth of the connection.
        Bandwidth bandwidth;
    
};
        
    }
}

#endif
