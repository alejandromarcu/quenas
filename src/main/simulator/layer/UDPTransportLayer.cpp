#include <iostream>
#include <cmath>

#include "UDPSegment.h"
#include "UDPTransportLayer.h"
#include "TPhysicalAddress.h"
#include "DataLinkLayer.h"
#include "MACAddress.h"
#include "BitStream.h"
#include "Connection.h"
#include "Units.h"
#include "Simulator.h"
#include "Frame.h"

namespace simulator {
    namespace layer {

using namespace std;
using namespace simulator::dataUnit;

/**
 * @brief Create a new UDP transport layer.
 *
 * @param node pointer to the node containing this layer.
 * @param nl pointer to the network layer used by this layer.
 */        
UDPTransportLayer::UDPTransportLayer(TNode *node, TNetworkLayer *nl) : TTransportLayer(node, nl)
{
    // Nothing else to do here.
}

/**
 * @brief Bind an application layer to the first port available starting in 1024.
 * The onBind method in the application layer is called to notify the id used.
 *
 * @param app pointer to the application layer to be binded.
 */
void UDPTransportLayer::bind(TApplicationLayer *app)
{
    int port = 1024;
    
    // search the first port available starting from 1024
    while(applications.find(Port(port)) != applications.end()) 
        port++;

    bind(Port(port), app);
}

/**
 * @brief Bind an application layer to the specified Application ID (Port), or
 * unbind if app pointer is NULL.
 *
 * @param appId application id (port) where the application layer is binded.
 * @param app pointer to the application layer to be binded.
 */
void UDPTransportLayer::bind(const TApplicationId &appId, TApplicationLayer *app)
{
    Port port = dynamic_cast<const Port &>(appId);
    
    if (app == NULL) {
        applications.erase(applications.find(port));
    } else {
        applications[port] = app;
    }
}

/**
 * @brief Send data to the specified network address.
 *
 * @param dest destination address.
 * @param sourceAppId Port of the sending application.
 * @param destAppId Port of the destination application.
 * @param data data to send
 */
void UDPTransportLayer::send(const TNetworkAddress &dest, const TApplicationId &sourceAppId,
                  const TApplicationId &destAppId, const TData &data)
{
    UDPSegment segment(dynamic_cast<const Port&>(sourceAppId).getPort(),
                       dynamic_cast<const Port&>(destAppId).getPort(),
                       data);
    networkLayer->send(dest, TransportType(17), segment);
}

/**
 * @brief This method is called when a packet is received.
 *
 * @param from network address that sent the packet.
 * @param packet the packet received.
 */
void UDPTransportLayer::receive(const TNetworkAddress &from, const TPacket &packet)
{
    UDPSegment segment(packet);
    
    TApplicationLayer *app = getApplication(Port(segment.getDestinationPort()));

    if (app == NULL) {
        // no application binded to this port.  
        return;
    }
        
    app->receive(from, 
                 Port(segment.getSourcePort()),
                 Data(segment.getPayload()), &packet);
                  
    
}


/**
 * @brief Return a pointer to the application layer binded to the application id or null if not found.
 *
 * @brief appId application id to lookup.
 * @return a pointer to the application layer binded to the application id or null if not found.
 */
TApplicationLayer* UDPTransportLayer::getApplication(const TApplicationId &appId)
{
    return applications[dynamic_cast<const Port&>(appId)];
}
                

}
}
