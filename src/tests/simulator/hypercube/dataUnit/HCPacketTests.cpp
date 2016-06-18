#include "HCPacket.h"
#include "UnitTest.h"
#include "MockDataUnit.h"

namespace simulator {
    namespace hypercube {
        namespace dataUnit {
      
namespace tests {

using namespace simulator::hypercube::dataUnit;
using namespace simulator::dataUnit::tests;

byte b1[] = { 0x0, 0x20, 0xA0, 0xFF, 0x12, 0x99};    
byte b2[] = {0x33, 0x30 };
byte b3[] = {0x34, 0x50 };
byte b4[] = {0x35, 0x70 };

MACAddress mac(b1);
HypercubeMaskAddress pa1(b2, 12, 1);
HypercubeMaskAddress pa2(b3, 12, 2);
HypercubeMaskAddress pa3(b4, 12, 3);
    
/** 
 * @brief Test the PARPacket.
 */
void testPARPacket() {
    UnitTest u("testPARPacket");
    PARPacket p(mac);
    
    
    vector<byte> vb;
    p.dumpTo(back_inserter(vb));
    
    byte expected[] = { 1,  // type and flags
                        11,    // total length
                        0x0, 0x20, 0xA0, 0xFF, 0x12, 0x99,    // Physical address
                        0, 0, // primary addres bit length & mask
                        0 };// header type

    u.areEqual(expected, 11, vb, "PARPacket has wrong format");                        
    
    MockFrame mf(vb);
    
    PARPacket *p2 = dynamic_cast<PARPacket*>(TControlPacket::create(mf));
    u.isTrue(p2 != NULL, "create method returned an unexpected object type.");

    u.isTrue(p2->getPhysicalAddress() == mac, "Bad mac address");
}

/** 
 * @brief Test the PAPPacket with address exhausted set and without headers
 */
void testPAPPacket1() {
    UnitTest u("testPAPPacket1");

    PAPPacket p(mac, pa1);
        
    vector<byte> vb;
    p.dumpTo(back_inserter(vb));
    
    byte expected[] = { 0x82,  // type and flags (address exhausted = true)
                        13,    // total length
                        0x0, 0x20, 0xA0, 0xFF, 0x12, 0x99,    // Physical address
                        12,  // primary addres bit length
                        0x33, 0x30, 1, // primary address & mask
                        0 };// header type

    u.areEqual(expected, 13, vb, "PAPPacket has wrong format");                        
    
    MockFrame mf(vb);
    
    PAPPacket *p2 = dynamic_cast<PAPPacket*>(TControlPacket::create(mf));
    u.isTrue(p2 != NULL, "create method returned an unexpected object type.");

    u.isTrue(p2->getPhysicalAddress() == mac, "Bad mac address");
    u.isTrue(p2->getPrimaryAddress() == pa1, "Bad primary address");
    u.isTrue(p2->isAddressExhausted(), "Address exhausted flag should be set.");
}

/** 
 * @brief Test the PAPPacket with optional headers
 */
void testPAPPacket2() {
    UnitTest u("testPAPPacket2");
    
    AdditionalAddress ap(pa2, 2, 5);

    AdditionalAddress ra1(pa2, 3);
    AdditionalAddress ra2(pa3, 4);

    vector<AdditionalAddress> v;
    v.push_back(ra1);
    v.push_back(ra2);


    PAPPacket p(mac, pa1, ap, v);
        
    vector<byte> vb;
    p.dumpTo(back_inserter(vb));
    
    byte expected[] = { 0x02,  // type and flags 
                        32,    // total length
                        0x0, 0x20, 0xA0, 0xFF, 0x12, 0x99,    // Physical address
                        12,  // primary addres bit length
                        0x33, 0x30, 1, // primary address & mask
                        0xC1, 7,         // address proposal header type -length
                        12, 0x34, 0x50, 2, 5, // address proposal OH
                        0x81, 6,         // reconnect address header type -length
                        12, 0x34, 0x50, 3, // reconnect address OH
                        0x81, 6,          // reconnect address header type -length
                        12, 0x35, 0x70, 4, // reconnect address OH
                        0 };// header type

    u.areEqual(expected, 32, vb, "PAPPacket has wrong format");                        
    
    MockFrame mf(vb);
    
    PAPPacket *p2 = dynamic_cast<PAPPacket*>(TControlPacket::create(mf));
    u.isTrue(p2 != NULL, "create method returned an unexpected object type.");

    u.isTrue(p2->getPhysicalAddress() == mac, "Bad mac address");
    u.isTrue(p2->getPrimaryAddress() == pa1, "Bad primary address");
    u.isFalse(p2->isAddressExhausted(), "Address exhausted flag should not be set.");
    
    u.isTrue(p2->getProposed().getAddress() == ap.getAddress(), "proposed not correctly retrieved");
    u.isTrue(p2->getReconnect()[0].getAddress() == ra1.getAddress(), "reconnect 0 not correctly retrieved");
    u.isTrue(p2->getReconnect()[1].getAddress() == ra2.getAddress(), "reconnect 1 not correctly retrieved");    
}

/** 
 * @brief Test the PANPacket.
 */
void testPANPacket() {
    UnitTest u("testPANPacket");

    PANPacket p(mac, pa1);
        
    vector<byte> vb;
    p.dumpTo(back_inserter(vb));
    
    byte expected[] = { 3,  // type and flags
                        13,    // total length
                        0x0, 0x20, 0xA0, 0xFF, 0x12, 0x99,    // Physical address
                        12,  // primary addres bit length
                        0x33, 0x30, 1, // primary address & mask
                        0 };// header type

    u.areEqual(expected, 13, vb, "PANPacket has wrong format");                        
    
    MockFrame mf(vb);
    
    PANPacket *p2 = dynamic_cast<PANPacket*>(TControlPacket::create(mf));
    u.isTrue(p2 != NULL, "create method returned an unexpected object type.");

    u.isTrue(p2->getPhysicalAddress() == mac, "Bad mac address");
    u.isTrue(p2->getPrimaryAddress() == pa1, "Bad primary address");
}

/** 
 * @brief Test the PANCPacket.
 */
void testPANCPacket() {
    UnitTest u("testPANCPacket");

    PANCPacket p(mac, pa1);
        
    vector<byte> vb;
    p.dumpTo(back_inserter(vb));
    
    byte expected[] = { 4,  // type and flags
                        13,    // total length
                        0x0, 0x20, 0xA0, 0xFF, 0x12, 0x99,    // Physical address
                        12,  // primary addres bit length
                        0x33, 0x30, 1, // primary address & mask
                        0 };// header type

    u.areEqual(expected, 13, vb, "PANCPacket has wrong format");                        
    
    MockFrame mf(vb);
    
    PANCPacket *p2 = dynamic_cast<PANCPacket*>(TControlPacket::create(mf));
    u.isTrue(p2 != NULL, "create method returned an unexpected object type.");

    u.isTrue(p2->getPhysicalAddress() == mac, "Bad mac address");
    u.isTrue(p2->getPrimaryAddress() == pa1, "Bad primary address");
}

/** 
 * @brief Test the DiscPacket.
 */
void testDISCPacket() {
    UnitTest u("testDISCPacket");

    DiscPacket p(mac, pa3 , false);
        
    vector<byte> vb;
    p.dumpTo(back_inserter(vb));
    
    byte expected[] = { 5,  // type and flags
                        13,    // total length
                        0x0, 0x20, 0xA0, 0xFF, 0x12, 0x99,    // Physical address
                        12,  // primary addres bit length
                        0x35, 0x70, 3, // primary address & mask
                        0 };// header type

    u.areEqual(expected, 13, vb, "DISCPacket has wrong format");                        
    
    MockFrame mf(vb);
    
    DiscPacket *p2 = dynamic_cast<DiscPacket*>(TControlPacket::create(mf));
    u.isTrue(p2 != NULL, "create method returned an unexpected object type.");

    u.isTrue(p2->getPhysicalAddress() == mac, "Bad mac address");
    u.isTrue(p2->getPrimaryAddress() == pa3, "Bad primary address");
}

/** 
 * @brief Test the HBPacket.
 */
void testHBPacket() {
    UnitTest u("testHBPacket");

    HBPacket p(mac, pa1);
        
    vector<byte> vb;
    p.dumpTo(back_inserter(vb));
    
    byte expected[] = { 6,  // type and flags
                        13,    // total length
                        0x0, 0x20, 0xA0, 0xFF, 0x12, 0x99,    // Physical address
                        12,  // primary addres bit length
                        0x33, 0x30, 1, // primary address & mask
                        0 };// header type

    u.areEqual(expected, 13, vb, "HBPacket has wrong format");                        
    
    MockFrame mf(vb);
    
    HBPacket *p2 = dynamic_cast<HBPacket*>(TControlPacket::create(mf));
    u.isTrue(p2 != NULL, "create method returned an unexpected object type.");

    u.isTrue(p2->getPhysicalAddress() == mac, "Bad mac address");
    u.isTrue(p2->getPrimaryAddress() == pa1, "Bad primary address");
}

/** 
 * @brief Test the SAPPacket.
 */
void testSAPPacket() {
    UnitTest u("testSAPPacket");
    
    AdditionalAddress ap(pa2, 2, 5);
    SAPPacket p(mac, pa1, ap);
        
    vector<byte> vb;
    p.dumpTo(back_inserter(vb));
    
    byte expected[] = { 0x07,  // type and flags 
                        20,    // total length
                        0x0, 0x20, 0xA0, 0xFF, 0x12, 0x99,    // Physical address
                        12,  // primary addres bit length
                        0x33, 0x30, 1, // primary address & mask
                        0xC1, 7,         // address proposal header type -length
                        12, 0x34, 0x50, 2, 5, // address proposal OH
                        0 };// header type

    u.areEqual(expected, 20, vb, "SAPPacket has wrong format");                        
    
    MockFrame mf(vb);
    
    SAPPacket *p2 = dynamic_cast<SAPPacket*>(TControlPacket::create(mf));
    u.isTrue(p2 != NULL, "create method returned an unexpected object type.");

    u.isTrue(p2->getPhysicalAddress() == mac, "Bad mac address");
    u.isTrue(p2->getPrimaryAddress() == pa1, "Bad primary address");   
    u.isTrue(p2->getProposed().getAddress() == ap.getAddress(), "proposed not correctly retrieved");
}

/** 
 * @brief Test the SANPacket.
 */
void testSANPacket() {
    UnitTest u("testSANPacket");
    
    AdditionalAddress ap(pa2, 2, 5);
    SANPacket p(mac, pa1, true, ap);
        
    vector<byte> vb;
    p.dumpTo(back_inserter(vb));
    
    byte expected[] = { 0x88,  // type and flags  (accepted = true)
                        20,    // total length
                        0x0, 0x20, 0xA0, 0xFF, 0x12, 0x99,    // Physical address
                        12,  // primary addres bit length
                        0x33, 0x30, 1,// primary address & mask
                        0xC1, 7,         // address proposal header type -length
                        12, 0x34, 0x50, 2, 5, // address proposal OH
                        0 };// header type

    u.areEqual(expected, 20, vb, "SANPacket has wrong format");                        
    
    MockFrame mf(vb);
    
    SANPacket *p2 = dynamic_cast<SANPacket*>(TControlPacket::create(mf));
    u.isTrue(p2 != NULL, "create method returned an unexpected object type.");

    u.isTrue(p2->getPhysicalAddress() == mac, "Bad mac address");
    u.isTrue(p2->getPrimaryAddress() == pa1, "Bad primary address");   
    u.isTrue(p2->getProposed().getAddress() == ap.getAddress(), "proposed not correctly retrieved");
    u.isTrue(p2->isAccepted(), "accepted should be true");
}

}

using namespace simulator::hypercube::dataUnit::tests;

/**
 * @brief Run all the test for hypercube::dataUnit namespace
 */
void runTests() 
{
    cout << "---------------- START HYPERCUBE DATA UNIT TESTS ----------------" << endl;
    testPARPacket();
    testPAPPacket1();
    testPAPPacket2();
    testPANPacket();
    testPANCPacket();    
    testDISCPacket();    
    testHBPacket();    
    testSAPPacket();    
    testSANPacket();    
    cout << "---------------- END HYPERCUBE DATA UNIT TESTS ----------------" << endl;

}


}
}
}
