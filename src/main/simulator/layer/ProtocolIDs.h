#ifndef _PROTOCOLIDS_H_
#define _PROTOCOLIDS_H_

#include "common.h"

namespace simulator {
    namespace layer {


/**
 * @brief Base clase for classes identifying network protocols. 
 *
 * It doesn't do anything, just serve as a base class.
 */
class TNetworkProtocolId {
    public:
        virtual ~TNetworkProtocolId() {};
};

/** 
 * @brief Represents the network protocol as in Ethernet II.
 *
 * see http://www.answers.com/main/ntquery?method=4&dsid=2222&dekey=EtherType&gwp=8&curtab=2222_1
 */
class EthernetType : public TNetworkProtocolId {
    public:
        EthernetType(int16 type) : type(type) {};
        EthernetType() : type(0) {};
        
        int16 getType() const { return type; }

        bool operator<(const EthernetType &type) const {
            return this->getType() < type.getType();
        }
            
    private:
        int16 type;
};


/**
 * @brief Base clase for classes identifying transport protocols. 
 *
 * It doesn't do anything, just serve as a base class.
 */
class TTransportProtocolId {
    public:
        virtual ~TTransportProtocolId() {};
};

/**
 * @brief Represents the transport protocol as stated in RFC 1700.
 * 
 * see http://www.faqs.org/rfcs/rfc1700.html
 */
class TransportType : public TTransportProtocolId {
    public:
        TransportType(byte type) : type(type) {};
        TransportType() : type(0) {};
        virtual ~TransportType() {};
        
        byte getType() const { return type; }

        bool operator<(const TransportType &type) const {
            return this->getType() < type.getType();
        }
            
    private:
        byte type;
};

/**
 * @brief Base clase for classes identifying applications
 *
 * It doesn't do anything, just serve as a base class.
 */
class TApplicationId {
    public:
        virtual ~TApplicationId() {};        
};


/** 
 * @brief Represents a port in TCP or UDP for identifying applications.
 */
class Port : public TApplicationId {
    public:
        Port(int16 type) : type(type) {};
        Port() : type(0) {};
        
        int16 getPort() const { return type; }

        bool operator<(const Port &port) const {
            return this->getPort() < port.getPort();
        }
            
    private:
        int16 type;
};

}
}
#endif
