#include <cmath>
#include <iostream>
#include <fstream>

#include "UnitTest.h"
#include "Units.h"
#include "Simulator.h"
#include "Message.h"
#include "Command.h"

namespace simulator {
      
namespace tests {

using namespace simulator::message;    
using namespace simulator::command;

/**
 * @brief Node used for testing.
 */
class MockNode : public TNode 
{
    public:
        virtual TCommandResult *runCommand(const Function &function) {};        
        string getName() const { return ""; };
};

/**
 * @brief Message receiver for testing.
 */
class MockMessageReceiver : public TMessageReceiver {
    private:
        TMessage *message;
    public:
        MockMessageReceiver() : message(NULL) {};
        virtual void onMessageReceived(const TMessage *message) { this->message = message->clone(); }
        TMessage* getMessage() const { return message; }        
        
        void clear() { message = NULL; }

};

/** 
 * @brief Test the Time class
 */
void testTime() 
{
    UnitTest u("testTime");   
    Time t1;
    u.areEqual(0, t1.getValue(), "t1: Default value should be 0");
    
    Time t2(1234);
    u.areEqual(1234, t2.getValue(), "t2: Bad initialization from long constructor");
    
    Time t3("1");
    u.areEqual(Time::SEC, t3.getValue(), "t3: Bad interpretation from string constructor");

    Time t4("145ms");
    u.areEqual(Time::MILISEC * 145, t4.getValue(), "t4: Bad interpretation from string constructor");

    Time t5("13   ms");
    u.areEqual(Time::MILISEC * 13, t5.getValue(), "t5: Bad interpretation from string constructor");

    Time t6("13.26   ms");
    u.areEqual(Time::MICROSEC * 13260, t6.getValue(), "t6: Bad interpretation from string constructor");

    Time t7("6h");
    u.areEqual(Time::HOUR * 6, t7.getValue(), "t7: Bad interpretation from string constructor");

    Time t8("5 m");
    u.areEqual(Time::MIN * 5, t8.getValue(), "t8: Bad interpretation from string constructor");

    Time t9("4 us");
    u.areEqual(Time::MICROSEC * 4, t9.getValue(), "t9: Bad interpretation from string constructor");

    Time t10("3 ns");
    u.areEqual(Time::NANOSEC * 3, t10.getValue(), "t10: Bad interpretation from string constructor");

    Time t11("1", Time::HOUR);
    u.areEqual(Time::HOUR, t11.getValue(), "t11: Bad interpretation from string constructor");


    Time u1(999 * Time::NANOSEC);
    u.areEqual("999.000000 ns", u1.toString(), "u1: Bad toString conversion");

    Time u2(1234 * Time::NANOSEC);
    u.areEqual("1.234000 us", u2.toString(), "u2: Bad toString conversion");

    Time u3(7 * Time::MILISEC);
    u.areEqual("7.000000 ms", u3.toString(), "u3: Bad toString conversion");

    Time u4(7500 * Time::MILISEC);
    u.areEqual("7.500000 s", u4.toString(), "u4: Bad toString conversion");

    Time u5(3 * Time::MIN);
    u.areEqual("3.000000 m", u5.toString(), "u5: Bad toString conversion");

    Time u6(500 * Time::HOUR);
    u.areEqual("500.000000 h", u6.toString(), "u6: Bad toString conversion");

    u.isTrue(Time(6) > Time(4), "1. Bad Time comparisson");
    u.isTrue(Time(10) < Time(600), "2. Bad Time comparisson");    
    u.isTrue(Time(-1) < Time(5), "3. Bad Time comparisson");    
    u.isTrue(Time(5) >= Time(5), "4. Bad Time comparisson");    
    u.isTrue(Time(7) >= Time(5), "5. Bad Time comparisson");    
    u.isTrue(Time(100) == Time(100), "6. Bad Time comparisson");    
    u.isTrue(Time(200) != Time(5), "7. Bad Time comparisson");    

}

/**
 * @brief Test the TNode class through MockNode, since TNode is abstract.
 */
void testTNode()
{
    UnitTest u("testTNode");   
    Simulator *sim = Simulator::getInstance();    
    sim->reset();
    sim->getNotificator().setFilename("temp.xml");

    MockNode n;
    MockMessageReceiver mr1, mr2, mr3;
    JoinNetworkMessage *m1 = new JoinNetworkMessage();
    WaitMeMessage *m2 = new WaitMeMessage(25);

    n.registerMessageListener(m1->TYPE, &mr1);
    n.registerMessageListener(m1->TYPE, &mr2);    
    n.registerMessageListener(m2->TYPE, &mr2);    
    n.registerMessageListener(m2->TYPE, &mr3);    
    
    n.putMessage(m1->clone()); // use clone so that m1 is not deleted
    while (sim->simulateStep());
    u.isTrue(NULL != dynamic_cast<JoinNetworkMessage*>(mr1.getMessage()), "mr1 expected to receive m1");
    u.isTrue(NULL != dynamic_cast<JoinNetworkMessage*>(mr2.getMessage()), "mr2 expected to receive m1");
    u.isTrue(NULL == mr3.getMessage(), "mr3 expected to be null");
    
    mr1.clear();  mr2.clear(); mr3.clear();

    n.putMessage(m2);
    while (sim->simulateStep());
    u.isTrue(NULL == mr1.getMessage(), "mr1 expected to be null");
    u.isTrue(NULL != dynamic_cast<WaitMeMessage*>(mr2.getMessage()), "mr2 expected to receive m2");
    u.isTrue(NULL != dynamic_cast<WaitMeMessage*>(mr3.getMessage()), "mr3 expected to receive m2");
   
    n.unregisterMessageListener(m1->TYPE, &mr2);    

    mr1.clear();  mr2.clear(); mr3.clear();
    
    
    n.putMessage(m1);
    while (sim->simulateStep());
    u.isTrue(NULL != dynamic_cast<JoinNetworkMessage*>(mr1.getMessage()), "mr1 expected to receive m1");
    u.isTrue(NULL == mr2.getMessage(), "mr2 expected to be null");
    u.isTrue(NULL == mr3.getMessage(), "mr3 expected to be null");

    
}

/**
 * @brief Test some simulations.
 *
 * In order to add more simulations, add the file to test_files/simulatios and
 * add its name to filelist.txt
 */
void testSimulations()
{
    UnitTest u("testSimulations");   
        
    ifstream fileList;    
    fileList.open("test_files/simulations/filelist.txt");

    u.isTrue(fileList, "Missing file: test_files/simulations/filelist.txt");
    string fname;
    while (fileList >> fname) {
        Simulator::getInstance()->destroy();
        Simulator *sim = Simulator::getInstance();        
        sim->getNotificator().setFilename("temp.xml");
        
        cout << "    Simulating " << fname << ": ";        
        sim->loadFile("test_files/simulations/" + fname);
        try {
            sim->simulate();
            cout << "OK" << endl;
        } catch (exception &e) {
            u.isTrue(false, e.what());
        }
        
    }

    fileList.close();
    
}
    
} // end tests namespace
  
  
using namespace simulator::tests;        

/**
 * @brief Run all the test for simulator namespace
 */
void runTests() 
{
    cout << "---------------- START SIMULATOR TESTS ----------------" << endl;
    testTime(); 
    testTNode();
    testSimulations();
    cout << "---------------- END SIMULATOR TESTS ----------------" << endl;
}
        
       
}
