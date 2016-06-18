#ifndef _RENDEZVOUSPACKET_H_
#define _RENDEZVOUSPACKET_H_

#include <vector>
#include <iterator>

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
 * @brief Base Class for Rendez Vous Packets.
 */
class TRendezVousPacket  {
    public:
        TRendezVousPacket(byte typeAndFlags);
        virtual ~TRendezVousPacket() {};
         
        static TRendezVousPacket *create(const VB &data);
        virtual void dumpTo(BackItVB it) const = 0;
        
        virtual VB getData() const;
        
        byte getType() const;        
    protected:
        void setFlag(int n, bool value);
        bool getFlag(int n) const;
    
        HypercubeAddress readHypercubeAddress(ConstItVB &it) const;
        
        /// Type of header (5 bits) and flags (3 bits)
        byte typeAndFlags;
        
};
      
/**
 * @brief Rendez Vous Register packet, used when a node joins the network
 * to communicate its address.
 */
class RendezVousRegister : public TRendezVousPacket {
    public:
        /// type for this packet
        const static byte TYPE;
        
        RendezVousRegister(const VB &data);
        RendezVousRegister(const HypercubeAddress &primaryAddress, const UniversalAddress &universalAddress);
        
        HypercubeAddress getPrimaryAddress() const;
        UniversalAddress getUniversalAddress () const;

        virtual void dumpTo(BackItVB it) const;        
    private:
        /// Primary address of the node
        HypercubeAddress primaryAddress;

        /// Universal address of the node
        UniversalAddress universalAddress;               
};

/**
 * @brief Rendez Vous DeRegister packet, used when a leaves the network
 * to communicate that it must be erased from tables.
 */
class RendezVousDeregister : public TRendezVousPacket {
    public:
        // type for this packet
        const static byte TYPE;
        
        RendezVousDeregister(const VB &data);
        RendezVousDeregister(const HypercubeAddress &primaryAddress, const UniversalAddress &universalAddress);
        
        HypercubeAddress getPrimaryAddress() const;
        UniversalAddress getUniversalAddress () const;

        virtual void dumpTo(BackItVB it) const;        
    private:
        HypercubeAddress primaryAddress;
        UniversalAddress universalAddress;               
};

/**
 * @brief Rendez Vous Address Solve packet, used to ask for an address resolution.
 */
class RendezVousAddressSolve : public TRendezVousPacket {
    public:
        /// type for this packet
        const static byte TYPE;
        
        RendezVousAddressSolve(const VB &data);
        RendezVousAddressSolve(const UniversalAddress &universalAddress);
        
        UniversalAddress getUniversalAddress () const;

        virtual void dumpTo(BackItVB it) const;        
    private:
        UniversalAddress universalAddress;               
};

/**
 * @brief Rendez Vous Address Lookup packet, used to reply an address resolution request.
 */
class RendezVousAddressLookup : public TRendezVousPacket {
    public:
        /// type for this packet
        const static byte TYPE;
        
        RendezVousAddressLookup(const VB &data);
        RendezVousAddressLookup(const HypercubeAddress &primaryAddress, const UniversalAddress &universalAddress, bool solved);
        
        HypercubeAddress getPrimaryAddress() const;
        UniversalAddress getUniversalAddress () const;
        bool isSolved() const;

        virtual void dumpTo(BackItVB it) const;        

    private:
        /// Whether the lookup could be solved or not
        bool solved;

        /// Primary address of the node
        HypercubeAddress primaryAddress;

        /// Universal address of the node
        UniversalAddress universalAddress;               
};

/**
 * @brief Rendez Vous Lookup Table packet, used to send part of all of a RV table.
 */
class RendezVousLookupTable : public TRendezVousPacket {
    public:
        /// type for this packet
        const static byte TYPE;
        typedef vector<pair<HypercubeAddress, UniversalAddress> > TTABLE;
        
        RendezVousLookupTable(const VB &data);
        RendezVousLookupTable();

        void add(const HypercubeAddress &primaryAddress, const UniversalAddress &universalAddress);
        const TTABLE &getTable() const;
        int16 getId() const;
        virtual void dumpTo(BackItVB it) const;        

    private:
        /// counter to assign ids
        static int16 globalId;
        
        /// id to acknowledge the lookup table
        int16 id;
        
        /// RV table
        TTABLE table;
};

/**
 * @brief Rendez Vous Table Received packet, used to confirm that a RendezVousLookupTable was received.
 */
class RendezVousLookupTableReceived : public TRendezVousPacket {
    public:
        /// type for this packet
        const static byte TYPE;
        
        RendezVousLookupTableReceived(const VB &data);
        RendezVousLookupTableReceived(int16 id);

        int16 getId() const;
        virtual void dumpTo(BackItVB it) const;        
    private:
        /// id to acknowledge a RendezVousLookupTable
        int16 id;
};
      
}
}

#endif
