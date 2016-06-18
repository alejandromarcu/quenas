#include "MACAddress.h"
#include "common.h"

namespace simulator {
    namespace address {

byte broadcast[6] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

const MACAddress MACAddress::BROADCAST = MACAddress(broadcast);

        
/** 
 * @brief Builds a MAC consisting of 6 zero bytes.
 */
MACAddress::MACAddress() : TPhysicalAddress(6) 
{
}

/** 
 * @brief Builds a MAC address using 6 bytes from addr
 *
 * @param addr pointer to 6 bytes representing the address
 */
MACAddress::MACAddress(byte *addr) : TPhysicalAddress(addr, 6) 
{
}

/** 
 * @brief Return the MAC address in the standard format, i.e. A0:10:00:20:FF:33
 *
 * @return the MAC address in the standard format, i.e. A0:10:00:20:FF:33
 */
string MACAddress::toString() const 
{
    char aux[20];
    sprintf(aux, "%02X:%02X:%02X:%02X:%02X:%02X", address[0], address[1],
         address[2], address[3], address[4], address[5]);
    
    return aux;
}


}
}


