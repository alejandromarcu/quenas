#include "UnitTest.h"
#include "DataUnit.h"
#include "Data.h"
#include "UDPSegment.h"
#include "Frame.h"
#include "BitStream.h"
#include "MockDataUnit.h"

namespace simulator {
    namespace dataUnit {
      
namespace tests {

using namespace simulator::dataUnit;
    
/** 
 * @brief test the Data class.
 */
void testData() {
    UnitTest u("testData");
        
    vector<byte> vb;
    vb.push_back('h');
    vb.push_back('e');
    vb.push_back('l');    
    vb.push_back('l');
    vb.push_back('o');        
    
    vector<byte> dumped;
    
    // test the constructor from vector<byte> and dumpTo
    Data d1(vb);    
    d1.dumpTo(back_inserter(dumped));

    u.areEqual(5, dumped.size(), "bad size");    
    u.areEqual('h', dumped[0], "character 0 of dumped");
    u.areEqual('e', dumped[1], "character 1 of dumped");
    u.areEqual('l', dumped[2], "character 2 of dumped");
    u.areEqual('l', dumped[3], "character 3 of dumped");
    u.areEqual('o', dumped[4], "character 4 of dumped");                

    
    // test the constructor from a string
    Data d2("Hypercube");
    dumped.clear();
    
    d2.dumpTo(back_inserter(dumped));
    u.areEqual(9, dumped.size(), "bad size");    
    u.areEqual('H', dumped[0], "character 0 of dumped");
    u.areEqual('e', dumped[8], "character 8 of dumped");    
    
    // test getPayload
    const vector<byte> &payload = d2.getPayload();

    u.areEqual(9, payload.size(), "bad size");    
    u.areEqual('H', payload[0], "character 0 of payload");
    u.areEqual('e', payload[8], "character 8 of payload");    
    
    // test toString
    u.areEqual("Hypercube", d2.toString(), "toString representation");
}


/** 
 * @brief test the UDPSegment class, using the constructor that takes
 * the ports and data
 */
void testUDPSegment1() {
    UnitTest u("testUDPSegment1");
    
    Data d("simulator");
    UDPSegment s(256, 1279, d);

    u.areEqual(256, s.getSourcePort());
    u.areEqual(1279, s.getDestinationPort());
        
    VB dumped;
    s.dumpTo(back_inserter(dumped));
    
    byte expected[] = { 1, 0,   // source port = 256
                        4, 255, // destination port = 1279
                        0, 17,  // length
                        0, 0,   // checksum
                        's','i','m','u','l','a','t','o','r' };

    u.areEqual(17, dumped.size(), "bad size");                        
    
    for (int i = 0; i < 17; i++) 
       u.areEqual(expected[i], dumped[i], "error in byte " + toStr(i) + " of dumped");
}

/** 
 * @brief test the UDPSegment class, using the constructor that takes
 * the ports and data
 */
void testUDPSegmentFromPacket() 
{
    UnitTest u("testUDPSegmentFromPacket");
    
    // Create a segment and encapsulate it into a Mock Packet
    string str = "this is the data for testing that will be encapsulated in the segment.";
    Data d(str);
    UDPSegment s(1234, 5678, d);

    VB dumped;
    s.dumpTo(back_inserter(dumped));

    MockPacket packet(dumped);
    
    // Build a segment from the MockPacket
    UDPSegment s2(packet);
    
    u.areEqual(1234, s2.getSourcePort(), "source port");
    u.areEqual(5678, s2.getDestinationPort(), "destination port");
    u.areEqual(str, Data(s2.getPayload()).toString(), "payload");
    
}

/** 
 * @brief test the Frame class.
 */
void testFrame()
{
    UnitTest u("testFrame");
    
    byte source[] = { 0x12, 0x34, 0x56, 0x78, 0x9A, 0xBC };
    byte dest[] = { 0x00, 0x10, 0x20, 0x30, 0x40, 0x50 };
    
    MACAddress sourceMAC(source);
    MACAddress destMAC(dest);
        
    VB p;
    string s= "packet";
    copy (s.begin(), s.end(), back_inserter(p));

    MockPacket packet(p);
            
    Frame f(source, dest, EthernetType(0xABCD), packet); 
    
    // test the getters
    u.isTrue(sourceMAC.toString() == f.getSource().toString(), "source address");
    u.isTrue(destMAC.toString()== f.getDestination().toString(), "destination address");
    u.isTrue(0xABCD == f.getEthernetType().getType(), "ethernet type");
    
    // Test the dumping
    VB dumped;
    f.dumpTo(back_inserter(dumped));
    
    byte expected[] = { 0x00, 0x10, 0x20, 0x30, 0x40, 0x50, // destination addr
                        0x12, 0x34, 0x56, 0x78, 0x9A, 0xBC, // source addr
                        0xAB, 0xCD, // ethernet type
                        'p', 'a', 'c', 'k', 'e', 't' };

    u.areEqual(20, dumped.size(), "bad size");                        
    
    for (int i = 0; i < 20; i++) 
       u.areEqual(expected[i], dumped[i], "error in byte " + toStr(i) + " of dumped");

    // Test the constructor from a bit stream, retrieving a frame stored on it
    BitStream bs(f); // store the frame in the bit stream
    Frame f2(bs);    // try to retrieve the frame from the bit stream

    dumped.clear();
    f2.dumpTo(back_inserter(dumped));    

    u.areEqual(20, dumped.size(), "bad size");                        
    
    for (int i = 0; i < 20; i++) 
       u.areEqual(expected[i], dumped[i], "error in byte " + toStr(i) + " of dumped");
       
}

/** 
 * @brief test the BitStream class.
 */
void testBitStream()
{
    UnitTest u("BitStream");
    VB data;
    data.push_back(10);
    data.push_back(255);    
    data.push_back(0);    
    data.push_back(100);    
    
    MockFrame frame(data);
    
    BitStream bs(frame);
    
    VB dumped;
    bs.dumpTo(back_inserter(dumped));

    u.areEqual(4, dumped.size(), "bad size");    
    u.areEqual(10, dumped[0], "character 0 of dumped");
    u.areEqual(255, dumped[1], "character 1 of dumped");
    u.areEqual(0, dumped[2], "character 2 of dumped");
    u.areEqual(100, dumped[3], "character 3 of dumped");

}

}

using namespace simulator::dataUnit::tests;

/**
 * @brief Run all the test for dataUnit namespace
 */
void runTests() 
{
    cout << "---------------- START DATA UNIT TESTS ----------------" << endl;
    testData();
    testUDPSegment1();
    testUDPSegmentFromPacket();
    testFrame();
    testBitStream();
    cout << "---------------- END DATA UNIT TESTS ----------------" << endl;

}


}
}
