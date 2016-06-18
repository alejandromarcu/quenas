#ifndef _TADDRESS_H_
#define _TADDRESS_H_

#include <vector>
#include <iterator>

#include "Notification.h"
#include "common.h"

namespace simulator {
    namespace address {

using namespace std;
using namespace simulator::notification;

/**
 * @brief Abstract base class for any address class.
 *
 * An address is just a collection of bytes; how many and how will they be organized
 * depends on the specifical address type.
 *
 */
class TAddress : public TQueryable {
    protected:
        /// The address stored as a byte vector
        vector<byte> address;

        /**
         * @brief Empty constructor.  Does nothing.
         */
        TAddress() {};

    public:
        /**
         * @brief Creates an address of the specified length of all zeros.
         *
         * @param length length of the address
         */
        TAddress(int length)  : address(length, 0)
        {
        };

        /**
         * @brief Creates an address of the specified length containing addr.
         *
         * @param addr pointer to at least length bytes of the address
         * @param length length of the address
         */
        TAddress(byte *addr, int length)  : address(addr, addr + length)
        {
        };

        /**
         * @brief Virtual destructor. Does nothing.
         */
        virtual ~TAddress() { }

        /**
         * @brief Copy the address as a vector of bytes in the iterator.
         *
         * @param it the iterator where the address will be copied
         */
        template<class OutIt>void dumpTo(OutIt it) const
        {
            copy(address.begin(), address.end(), it);
        };

        /**
         * @brief Return the address in human readable format, depending on the
         * address implementation.
         *
         * @return the address in human readable format.
         */
        virtual string toString() const = 0;

        /**
         * @brief Returns true if addr is equal to this address.
         *
         * @param addr address to compare with this address.
         * @return true if addr is equal to this address.
         */
        bool operator==(const TAddress &addr) const
        {
            return this->address == addr.address;
        }

        /**
         * @brief Returns true if addr is not equal to this address.
         *
         * @param addr address to compare with this address.
         * @return true if addr is not equal to this address.
         */
        bool operator!=(const TAddress &addr) const
        {
            return this->address != addr.address;
        }
        
        /**
         * @brief Returns true if this address is smaller than addr.
         *
         * @param addr address to compare with this address.
         * @return true if this address is smaller than addr.
         */        
        bool operator<(const TAddress &addr) const {
            return this->address < addr.address;
        }

        /**
         * @brief Queries the address.
         *
         * @param options options for the query. Not used.
         * @return a QueryResult with the address in a readable format.
         */
        virtual QueryResult *query(const vector<string> *options = NULL) const 
        {
            return new QueryResult("address", "address", toString());            
        }

};

// end namespaces
}
}

#endif
