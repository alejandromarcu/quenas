#include <vector>
#include <iterator>


#include "Applications.h"
#include "common.h"
#include "HCPacket.h"
#include "HypercubeControlLayer.h"
#include "Message.h"
#include "Units.h"
#include "Event.h"
#include "StateMachines.h"
#include "HypercubeNode.h"

namespace simulator {
    namespace hypercube {
 
using namespace std;
using namespace simulator::hypercube::dataUnit;
using namespace simulator::message;
using namespace simulator::event;
      
const int TestApplication::PORT = 9920;      
      
//-------------------------------------------------------------------------
//---------------------< HypercubeBaseApplication >------------------------
//-------------------------------------------------------------------------      

/**
 * @brief Create a HypercubeBaseApplication.
 *
 * @param tl pointer to the Transport Layer below this application.
 */ 
HypercubeBaseApplication::HypercubeBaseApplication(TTransportLayer *tl) 
    : TApplicationLayer(tl->getNode(), tl)
{
}

/**
 * @brief Get the port used by this application.
 *
 * @return the port used by this application.
 */
TApplicationId HypercubeBaseApplication::getApplicationId() const
{
    return port;
}


/**
 * @brief Bind the application to the specified port.
 *
 * @param p the port to bind the application.
 */
void HypercubeBaseApplication::bind(Port p) 
{
    port = p;
    transportLayer->bind(p, this);
}

/**
 * @brief Unbind the application from the transport layer.
 */
void HypercubeBaseApplication::unbind()
{
    transportLayer->bind(port, NULL);
    
}

/**
 * @brief Send a packet.
 *
 * @param dest destination universal address
 * @param port port in the other end
 * @param data data to send
 */
void HypercubeBaseApplication::send(const UniversalAddress &dest, Port destPort, const Data &data)
{
    // get the RendezVousClient application to use it for sending.
    RendezVousClient *rvClient = dynamic_cast<RendezVousClient*>(transportLayer->getApplication(Port(RendezVousClient::PORT)));
     
    rvClient->send(dest, port.getPort(), destPort.getPort(), data);    
}


//-------------------------------------------------------------------------
//-------------------------< TestApplication >-----------------------------
//------------------------------------------------------------------------- 

/**
 * @brief Create a Test Application and bind it to the transport layer.
 *
 * @param tl pointer to the Transport Layer below this application.
 */
TestApplication::TestApplication(TTransportLayer *tl) : HypercubeBaseApplication(tl)
{
    bind(PORT);
}

/**
 * @brief Unbind the application from the port.
 */
TestApplication::~TestApplication()
{
    unbind();
}

/**
 * @brief Run a Command.
 *
 * @param function the function to run.
 * @return a pointer to the next command result to execute.
 */        
TCommandResult *TestApplication::runCommand(const Function &function)
{
   if (function.getName() == "send")  
   {
        HypercubeNode *n = dynamic_cast<HypercubeNode *>(getNode());

        VB appData;

        dumpStringTo(n->getUniversalAddress().toString(), back_inserter(appData));
        dumpStringTo(function.getStringParam(0), back_inserter(appData));
        dumpStringTo(function.getParamCount() > 1?  function.getStringParam(1): "", back_inserter(appData)); // tag 
        dumpStringTo(toStr(Simulator::getInstance()->getTime().getValue()), back_inserter(appData));  // t1
        dumpStringTo("", back_inserter(appData)); // t2
        dumpStringTo("", back_inserter(appData)); // t3
        dumpStringTo("", back_inserter(appData)); // d1
        dumpStringTo("", back_inserter(appData)); // d2

        Data data(appData);
        send(function.getStringParam(0), PORT, data);
        return this;
   }
    
    throw invalid_argument("TestApplication: Invalid function: " + function.getName());    
}

/**
 * @brief Receive data.
 *
 * @param from address of the sending node
 * @param sourceAppId the Application Id (port) of the sender
 * @param data data received
 */
void TestApplication::receive(const TNetworkAddress &from, const TApplicationId &sourceAppId, const Data &data, const TPacket *packet)
{
    VB::const_iterator it = data.getPayload().begin();
    string src = readString(it);
    string dest = readString(it);    
    string tag = readString(it);    
    string time1str = readString(it);
    string time2str = readString(it);
    string time3str = readString(it);
    string d1str = readString(it);
    string d2str = readString(it);
    UniversalAddress next("");

    const DataPacket *p = dynamic_cast<const DataPacket *>(packet);

    if (d1str == "") { // first message
       d1str = toStr(DataPacket::MAX_TTL - p->getTTL());
       time2str = toStr(Simulator::getInstance()->getTime().getValue());
       next = UniversalAddress(src);
    } else if (d2str == "") { // second message 
       d2str = toStr(DataPacket::MAX_TTL - p->getTTL());
       time3str = toStr(Simulator::getInstance()->getTime().getValue());
       next = UniversalAddress(dest);
    } else { // third and last message!
        Time time1 = Time(time1str, 1);
        Time time2 = Time(time2str, 1);
        Time time3 = Time(time3str, 1);

        HypercubeNetwork *hn = dynamic_cast<HypercubeNetwork *>(Simulator::getInstance()->getNetwork());
        HypercubeNode *n = dynamic_cast<HypercubeNode *>(getNode());
        
        QueryResult *qr = new QueryResult("Data");
        qr->insert("source", src);
        qr->insert("destination", dest); 
        qr->insert("tag", tag);        
        qr->insert("shortestPath", toStr(hn->getShortestPath(hn->getNode(UniversalAddress(src))->getUniversalAddress(), n->getUniversalAddress() ,false)));     
        qr->insert("distance1", d1str);     
        qr->insert("elapsedTime1", Time(time2.getValue()- time1.getValue()).toString(Time::SEC));    
        qr->insert("distance2", d2str);     
        qr->insert("elapsedTime2", Time(time3.getValue()- time2.getValue()).toString(Time::SEC));    
        qr->insert("distance3", toStr(DataPacket::MAX_TTL - p->getTTL()));     
        qr->insert("elapsedTime3", Time(Simulator::getInstance()->getTime().getValue()- time3.getValue()).toString(Time::SEC));    
            
        Simulator::getInstance()->notify("node.testApplication.received", qr);                

        return;
    }

    VB appData;

    dumpStringTo(src, back_inserter(appData));
    dumpStringTo(dest, back_inserter(appData));
    dumpStringTo(tag, back_inserter(appData));
    dumpStringTo(time1str, back_inserter(appData)); 
    dumpStringTo(time2str, back_inserter(appData)); 
    dumpStringTo(time3str, back_inserter(appData));
    dumpStringTo(d1str, back_inserter(appData));  
    dumpStringTo(d2str, back_inserter(appData)); 

    Data replyData(appData);
    send(next, PORT, replyData);
}

/**
 * @brief Get the object name
 *
 * @return "TestApplication"
 */
string TestApplication::getName() const
{
    return "TestApplication";
}



}
}

