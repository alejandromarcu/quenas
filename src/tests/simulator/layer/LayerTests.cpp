#include <cmath>

#include "UnitTest.h"
#include "MockLayer.h"
#include "common.h"
#include "Connection.h"
#include "MockDataUnit.h"
#include "PhysicalLayer.h"
#include "DataLinkLayer.h"
#include "UDPTransportLayer.h"

namespace simulator {
    namespace layer {
      
namespace tests {

using namespace simulator::dataUnit::tests;

/** 
 * @brief test the Connection class.
 */
void testConnection() 
{
    UnitTest u("testConnection");
    Simulator *sim = Simulator::getInstance();
    sim->reset();
    
    // Build 2 mock physical layers
    byte b[] = {1,2,3,4,5,6,7,8};
    MACAddress mac1(b);
    MACAddress mac2(b+2);    
    MockPhysicalLayer p1(NULL, mac1);
    MockPhysicalLayer p2(NULL, mac2);    
    
    // the connection that will be tested, using 15 as the delay
    Connection conn(&p1, &p2, 10000, 15);
    
    // test getDelay method
    u.areEqual(15, conn.getDelay().getValue(), "delay");

    // test getBandwidth method
    u.areEqual(10000, conn.getBandwidth().bpsValue(), "bandwidth");

    // test getPoints
    u.areEqual(&p1, conn.getPoints()[0]);
    u.areEqual(&p2, conn.getPoints()[1]);
    
    // Create a Bit Stream to send
    MockBitStream bs(1000);
    
    // send the frame from p1, so it should arrive to p2
    conn.transport(&p1, bs);
    
    Simulator::getInstance()->simulateStep();
    Simulator::getInstance()->simulateStep();    
    
    u.areEqual(15, p2.getTimeReceived().getValue(), "bad reception time");
    u.areEqual(1000, p2.getReceived()->getLength(), "bad length of data received");    
}

/**
 * @brief test PhysicalLayer class.
 *
 * It tests connecting PhysicalLayers and sending frames from one to another and
 * assert that the frame arrived to the expected PL and with the expected delay.
 */
void testPhysicalLayer1()
{
    UnitTest u("testPhysicalLayer1");
    Simulator *sim = Simulator::getInstance();
    sim->reset();

    // Create 3 Physical layers to be tested (pl1, pl2 and pl3) with mock link layers
    // that should receive the sent frames
    
    byte b[] = {1,2,3,4,5,6,7,8,9,10};
    MACAddress mac1(b);
    MACAddress mac2(b+2);    
    MACAddress mac3(b+4);        

    PhysicalLayer pl1(NULL, mac1);
    PhysicalLayer pl2(NULL, mac2);    
    PhysicalLayer pl3(NULL, mac3);        
    
    MockDataLinkLayer dll1(NULL, &pl1);
    MockDataLinkLayer dll2(NULL, &pl2);
    MockDataLinkLayer dll3(NULL, &pl3);
    
    pl1.setDataLinkLayer(&dll1);
    pl2.setDataLinkLayer(&dll2);
    pl3.setDataLinkLayer(&dll3);    
        
    Connection conn1(&pl1, &pl2, Bandwidth(1024*1024 * 8), Time::MILISEC * 30);
    Connection conn2(&pl1, &pl3, Bandwidth(512*1024 * 8), Time::MILISEC * 40);


    VB data1(2000, 26);
    VB data2(10000, 26);
    VB data3(5555, 78);
    
    // Send from pl1 one frame to pl2 and one to pl3.
    pl1.send(mac2, MockFrame(data1));
    pl1.send(mac3, MockFrame(data2));
    
    sim->simulateStep(); // should run SendBitSream  for first frame
    sim->simulateStep(); // should run SendBitSream  for second frame
    sim->simulateStep(); // should run ReceiveBitSream  for first frame

    // check that the frame to pl2 arrived correctly
    long long expectedTime =  (long long) round(Time::MILISEC * 30 + 2000.0 * Time::SEC / (1024.0 * 1024));
    u.areEqual(expectedTime, dll2.getTimeReceived().getValue(), "1.bad reception time");
    u.areEqual(2000, dll2.getReceived()->getLength(), "bad length of data received");        
    u.isTrue(dll3.getReceived() == NULL, "data link layer 3 must not have received anything!");

    sim->simulateStep(); // should run ReceiveBitSream  for first frame

    // check that the frame to pl3 arrived correctly
    expectedTime +=  (long long) round(Time::MILISEC * 10 + 10000.0 * Time::SEC / (512.0 * 1024));
    u.areEqual(expectedTime, dll3.getTimeReceived().getValue(), "2.bad reception time");
    u.areEqual(10000, dll3.getReceived()->getLength(), "bad length of data received");        
 
    long long et2 = expectedTime + (long long) round(Time::MILISEC * 30 + 5555.0 * Time::SEC / (512.0 * 1024));
    long long et3 = expectedTime + (long long) round(Time::MILISEC * 40 + 5555.0 * Time::SEC / (512.0 * 1024));

    // Test Broadcast
    pl1.send(MACAddress::BROADCAST, MockFrame(data3));
    while (sim->simulateStep(et3));    
    
    u.areEqual(5555, dll2.getReceived()->getLength(), "bad length of data received for broadcast in 2");        
    u.areEqual(5555, dll3.getReceived()->getLength(), "bad length of data received for broadcast in 3");        
    u.areEqual(et2, dll2.getTimeReceived().getValue(), "3.bad reception time");
    u.areEqual(et3, dll3.getTimeReceived().getValue(), "4.bad reception time");

}

/**
 * @brief test DataLinkLayer class.
 *
 * It tests that the Data Link layer correctly sends and receives packets
 * and that it correctly passes them to the network layer depending
 * on the Ethernet Type.
 */
void testDataLinkLayer1()
{
    UnitTest u("testDataLinkLayer1");
    Simulator *sim = Simulator::getInstance();
    sim->reset();

    byte b[] = {1,2,3,4,5,6,7,8,9,10};
    MACAddress mac1(b);
    MACAddress mac2(b+2);    

    PhysicalLayer pl1(NULL, mac1);
    PhysicalLayer pl2(NULL, mac2);    
    
    DataLinkLayer dll1(NULL, &pl1);
    DataLinkLayer dll2(NULL, &pl2);
    
    pl1.setDataLinkLayer(&dll1);
    pl2.setDataLinkLayer(&dll2);
    
    // Attach 2 networks layers to data link layer 2           
    MockNetworkLayer mnl1 (NULL, &dll2);
    MockNetworkLayer mnl2 (NULL, &dll2);
    
    EthernetType type1 = EthernetType(100);
    EthernetType type2 = EthernetType(200);
        
    dll2.registerNetworkProtocol(type1, &mnl1);
    dll2.registerNetworkProtocol(type2, &mnl2);    
    
    Connection conn1(&pl1, &pl2, 512*1024);

    VB data(2000, 26);
    MockPacket packet(data);

    // send a packet of type1 to data link layer 2.
    dll1.send(mac2, type1, packet);

    while (sim->simulateStep(Time::MIN)); 

    u.isTrue(mnl2.getReceived() == NULL, "Network layer with type2 must not have received anything!");    
    u.isTrue(mnl1.getReceived()->getPayload() == data, "bad data received");        
    
    mnl1.reset();    
    mnl2.reset();    

    // send a packet of type2 to data link layer 2.
    dll1.send(mac2, type2, packet);

    while (sim->simulateStep(2* Time::MIN)); 

    u.isTrue(mnl1.getReceived() == NULL, "Network layer with type1 must not have received anything!");    
    u.isTrue(mnl2.getReceived()->getPayload() == data, "bad data received");        

    // unregister a protocol type and try to send a packet, it should throw an exception
    dll2.registerNetworkProtocol(type2, NULL);
    dll1.send(mac2, type2, packet);
    
    try {
        while (sim->simulateStep(3 * Time::MIN)); 
        u.isTrue(false, "An exception must be thrown when sending to a non registered protocol");
    } catch (...) {
    }
}


} // end tests namespace
  
using namespace simulator::layer::tests;        

/**
 * @brief Run all the test for layer namespace
 */
void runTests() 
{
    cout << "---------------- START LAYER TESTS ----------------" << endl;
    testConnection(); 
    testPhysicalLayer1();   
    testDataLinkLayer1();
    cout << "---------------- END LAYER TESTS ----------------" << endl;
}
        
       
    }
}
