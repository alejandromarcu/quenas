#ifndef _TOPTIONALHEADER_H_
#define _TOPTIONALHEADER_H_

#include "Notification.h"

namespace simulator {
    namespace hypercube {
        namespace dataUnit {

using namespace simulator::notification;

/**
 * @brief Base class for all optional headers.
 */
class TOptionalHeader : public TQueryable {
    public:
        TOptionalHeader(byte type);
        virtual ~TOptionalHeader();
        
        byte getHeaderType() const;
        bool getFlag(int n) const;
        virtual void dumpTo(BackItVB it) const = 0;
        
        static byte getType(ConstItVB &it);
        static TOptionalHeader* create(ConstItVB &it);
        byte getLength() const;

        virtual QueryResult *query(const vector<string> *options = NULL) const = 0;    
    protected:
        void setFlag(int n, bool value);
        
        /// Type of header (5 bits) and flags (3 bits)
        byte typeAndFlags;
        
        /// Total length of the header
        byte length;

};

}}}


#endif
