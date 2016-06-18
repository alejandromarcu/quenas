#include "UnitTest.h"
#include "MACAddress.h"
#include "HypercubeAddress.h"
#include "UniversalAddress.h"
#include "HypercubeMaskAddress.h"
#include "AddressSpace.h"

namespace simulator {
    namespace address {
      
namespace tests {

/** 
 * @brief test the MACAddress class.
 */
void testMACAddress() {
    UnitTest u("testMACAddress");

    byte b[] = { 0x0, 0x20, 0xA0, 0xFF, 0x12, 0x99};    

    MACAddress addr(b);
    u.areEqual("00:20:A0:FF:12:99", addr.toString(), "toString conversion");
    
    vector<byte> v(6);
    addr.dumpTo(v.begin());
    
    for(int i=0; i < 6; i++) {
        u.areEqual(b[i], v[i], "byte " + toStr(i) + " of dumpTo result");
    }
}

    
/** 
 * @brief test the HypercubeAddress class.
 */
void testHypercubeAddress() {
    UnitTest u("testHypercubeAddress");

    byte b[] = { 0xF0 };
    
    // test the constructor that takes a byte array
    HypercubeAddress hc1(b, 5);    
    u.areEqual("11110", hc1.toString(), "constructor 1 or toString");
    
    u.areEqual(5, hc1.getBitLength(), "getBitLength");
    
    u.areEqual(1, hc1.getBit(0), "getBit(0)");
    u.areEqual(0, hc1.getBit(4), "getBit(4)");    
    
    hc1.setBit(0, 0);
    u.areEqual("01110", hc1.toString(), "setBit");    

    hc1.setBit(0);
    u.areEqual("11110", hc1.toString(), "setBit with default value");    
    
    hc1.flipBit(0);
    u.areEqual("01110", hc1.toString(), "flipBit");    

    hc1.flipBit(0);
    u.areEqual("11110", hc1.toString(), "flipBit");    
    
    // distance test
    HypercubeAddress a1("10100010");
    HypercubeAddress a2("00100110");    
    u.areEqual(2, a1.distance(a2), "a1.distance(a2)");
}

/** 
 * @brief test the HypercubeMaskAddress class.
 */
void testHypercubeMaskAddress() {
    UnitTest u("testHypercubeMaskAddress");

    byte b[] = { 0xF0, 0x00 };    
    HypercubeAddress hc1(b, 7);        

    // Test the constructor from an HypercubeAddress
    HypercubeMaskAddress hma1(hc1, 4);    
    u.areEqual("1111000/4", hma1.toString(), "constructor 1");
    
    // Test the constructor from a byte array
    HypercubeMaskAddress hma2(b, 10, 5);    
    u.areEqual("1111000000/5", hma2.toString(), "constructor 2");
    
    // Test getMask
    u.areEqual(4, hma1.getMask(), "getMask");
    u.areEqual(5, hma2.getMask(), "getMask");    
    
    // Test contains taking an hypercube address
    u.isTrue(hma1.contains(hc1));

    hc1.setBit(5); 
    u.isTrue(hma1.contains(hc1));

    hc1.setBit(4); 
    u.isTrue(hma1.contains(hc1));

    hc1.setBit(3, 0); 
    u.isFalse(hma1.contains(hc1));

    // Test contains taking an hypercube mask address
    HypercubeMaskAddress hma3(hma2);
    u.isTrue(hma2.contains(hma3));

    hma3.setMask(6);
    u.isTrue(hma2.contains(hma3));
    
    hma3.setMask(4);
    u.isFalse(hma2.contains(hma3));    
    
    hma3.setMask(8);
    hma3.setBit(7);
    u.isTrue(hma2.contains(hma3));

    // distance test
    HypercubeAddress a1("10100010");
    HypercubeMaskAddress a2("00010110", 3);        
    HypercubeMaskAddress a3("01100010", 4);        
    HypercubeMaskAddress a4("10101000", 6);        
    u.areEqual(2, a2.distanceWithMask(a3), "a2.distanceWithMask(a3)");
    u.areEqual(2, a3.distanceWithMask(a2), "a3.distanceWithMask(a2)");
    u.areEqual(2, a2.distanceWithMask(a4), "a2.distanceWithMask(a4)");
    u.areEqual(2, a3.distanceWithMask(a4), "a3.distanceWithMask(a4)");
    u.areEqual(2, a2.distanceWithMask(a1), "a2.distanceWithMask(a1)");
    u.areEqual(4, a2.distance(a3), "a2.distance(a3)");

}


/** 
 * @brief test the UniversalAddress class.
 */
void testUniversalAddress() {
    UnitTest u("testUniversalAddress");
    UniversalAddress addr("nodex");

    u.areEqual("nodex", addr.toString(), "toString conversion");
}


/** 
 * @brief test the AddressSpace class.
 */
void testAddressSpace() {
    UnitTest u("testAddressSpace");
    AddressSpace as;
    
    as.add(HypercubeMaskAddress("1010",3));
    as.add(HypercubeMaskAddress("1100",2));    
    as.add(HypercubeMaskAddress("1000",3));    

    vector<HypercubeMaskAddress> v = as.getBase();

    u.areEqual(1, v.size(), "bad number of base addresses");
    u.areEqual("1000/1", v[0].toString(), "bad base addresses");
}

}
  
using namespace simulator::address::tests;        

/**
 * @brief Run all the test for address namespace
 */
void runTests() 
{
    cout << "---------------- START ADDRESS TESTS ----------------" << endl;
    testMACAddress();    
    testHypercubeAddress();
    testHypercubeMaskAddress();
    testUniversalAddress();
    testAddressSpace();
    cout << "---------------- END ADDRESS TESTS ----------------" << endl;    
}
        
       
    }
}
