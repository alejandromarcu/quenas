#ifndef _MOCKDATAUNIT_H_
#define _MOCKDATAUNIT_H_

#include "UnitTest.h"
#include "DataUnit.h"
#include "Data.h"
#include "UDPSegment.h"
#include "Frame.h"
#include "BitStream.h"

namespace simulator {
    namespace dataUnit {
      
namespace tests {

/**
 * @brief Mock Packet for testing.
 */    
class MockPacket : public TPacket {
    public:
        MockPacket(const VB& data) : TPacket(data) {};        
        virtual void dumpTo(BackItVB it) const 
        {
            copy(data.begin(), data.end(), it);
        };        
        
};

/**
 * @brief Mock Frame for testing.
 */    
class MockFrame : public TFrame {
    public:
        MockFrame(const VB& data) : TFrame(data) {};        
        virtual void dumpTo(BackItVB it) const 
        {
            copy(data.begin(), data.end(), it);
        };        
        
};

/**
 * @brief Mock Bit Stream for testing.
 */    
class MockBitStream : public BitStream {
    public:
        MockBitStream(int length) : BitStream(MockFrame(VB(length))) {};
        MockBitStream(const VB& data) : BitStream(MockFrame(data)) {};        
        virtual void dumpTo(BackItVB it) const 
        {
            copy(data.begin(), data.end(), it);
        };        
        
};

}
}
}

#endif
