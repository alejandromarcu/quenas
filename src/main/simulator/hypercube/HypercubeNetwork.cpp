#include <fstream>
#include <iostream>
#include "Simulator.h"
#include "Units.h"
#include "Command.h"
#include "common.h"
#include "Notification.h"
#include "UniversalAddress.h"
#include "PhysicalLayer.h"
#include "Connection.h"
#include "HypercubeNode.h"
#include "Exceptions.h"
#include "MultiCommandRunner.h"
#include "CommandQueryResult.h"
#include "HypercubeNetwork.h"
#include "AddressSpace.h"

namespace simulator {
	namespace hypercube {

using namespace std;
using namespace simulator::command;
using namespace simulator::notification;
using namespace simulator::address;
using namespace simulator::layer;

/**
 * @brief Create an hypercube network.
 *
 * @param addressLength length of the addresss in bits.
 */
HypercubeNetwork::HypercubeNetwork(int addressLength) : addressLength(addressLength)
{
}

/**
 * @brief Run a command.
 *
 * @param function function to run.
 */
TCommandResult *HypercubeNetwork::runCommand(const Function &function)
{
    if (function.getName() == "simulator")
    {
        return Simulator::getInstance();
    }

    if (function.getName() == "query")
    {
        QueryResult *qr = new QueryResult("network");
        qr->insert("addressLength", toStr(addressLength));
        qr->insert("nodeCount", toStr(nodes.size()));

        int biggestMask = 0;        
        for (map<UniversalAddress, HypercubeNode*>::iterator it = nodes.begin(); it!= nodes.end(); it++) {
            if (it->second->getPrimaryAddressMask() > biggestMask) biggestMask = it->second->getPrimaryAddressMask();
        }
        qr->insert("biggestMask", toStr(biggestMask));

        return new CommandQueryResult(qr);
    }

    if (function.getName() == "setAddressLength")
    {
        addressLength = function.getIntParam(0);
        return this;
    }

    if (function.getName() == "newNode")
    {
        HypercubeNode *node = new HypercubeNode(UniversalAddress(function.getStringParam(0)));
        addNode(node);
        return node;
    }

    if (function.getName() == "node") {
       return getNode(UniversalAddress(function.getStringParam(0)));
    }

    if (function.getName() == "allNodes") {
        vector<TCommandRunner *>runners;
        map<UniversalAddress, HypercubeNode*>::iterator it;

        for (it = nodes.begin(); it!= nodes.end(); it++) {
            runners.push_back(it->second);
        }

        return new MultiCommandRunner(runners);
    }

    if (function.getName() == "deleteNode") {
        NodesIterator it = getNodeIterator(UniversalAddress(function.getStringParam(0)));

        delete it->second;
        nodes.erase(it);
        return this;
    }


    if (function.getName() == "newConnection") {
        HypercubeNode *node1 = getNode(UniversalAddress(function.getStringParam(0)));
        HypercubeNode *node2 = getNode(UniversalAddress(function.getStringParam(1)));

        Connection *conn;

        conn = new Connection(node1->getPhyiscalLayer(), node2->getPhyiscalLayer());
        if (function.getParamCount() >= 3) conn->setBandwidth(Bandwidth(function.getStringParam(2)));
        if (function.getParamCount() >= 4) conn->setDelay(function.getTimeParam(3));

        return conn;
    }
    if (function.getName() == "connection") {
        HypercubeNode *node1 = getNode(UniversalAddress(function.getStringParam(0)));
        HypercubeNode *node2 = getNode(UniversalAddress(function.getStringParam(1)));

        TConnection *conn = node1->getPhyiscalLayer()->getConnections()[node2->getPhysicalAddress()];
        if (conn == NULL)
            throw command_error("Connection not found: (" + function.getStringParam(0) + ", " + function.getStringParam(0) + ")");

        return conn;
    }

    if (function.getName() == "deleteConnection") {
        HypercubeNode *node1 = getNode(UniversalAddress(function.getStringParam(0)));
        HypercubeNode *node2 = getNode(UniversalAddress(function.getStringParam(1)));

        map<MACAddress, TConnection*> &conn = node1->getPhyiscalLayer()->getConnections();
        map<MACAddress, TConnection*>::iterator it = conn.find(node2->getPhysicalAddress());
        if (it == conn.end())
             throw invalid_argument("Connection not found: (" + function.getStringParam(0) + ", " + function.getStringParam(0) + ")");

        delete it->second;
        return this;
    }

    if (function.getName() == "assertCompleteAddressSpace") {
        AddressSpace as;
        map<UniversalAddress, HypercubeNode*>::iterator it;

        for (it = nodes.begin(); it!= nodes.end(); it++) {
            const vector<HypercubeMaskAddress> addr = it->second->getHypercubeControlLayer()->getAddresses();
            for (int i = 0; i < addr.size(); i++) {
                if (!as.add(addr[i])) {
                    vector<HypercubeMaskAddress> v = as.getBase();                                      
                    string s;                                      
                    for (int k = 0; k < v.size(); k++) {
                        s += v[k].toString() + "\n";
                    }
                    throw invalid_argument("Address space has non-disjoint addresses! Address: " + addr[i].toString() + "\nSpace: \n" + s) ;                                      
                }
            }
        }
        
        vector<HypercubeMaskAddress> v = as.getBase();
        if (v[0].getMask() != 0) {
            string s, m;                                      
            for (int i = 0; i < v.size(); i++) {
                s += v[i].toString() + "\n";
            }
            
            while (v[0].getMask() != 0) {
                HypercubeMaskAddress missing = v[v.size() - 1];
                missing.flipBit(v[v.size() - 1].getMask() - 1);
                m += missing.toString() + " \n";
                as.add(missing);
                v = as.getBase();
            } 
            
            throw invalid_argument("Address space is not complete! Composed by: \n" + s + "\n\nMissing: \n" + m) ;
        }
        return this;
    }

    if (function.getName() == "exportConnections") {
        exportConnections(function.getStringParam(0));
        return this;
    }



    throw invalid_argument("Bad function: " + function.toString());
}


void HypercubeNetwork::exportConnections(const string &filename)
{
    ofstream ofile(filename.c_str());

    map<UniversalAddress, HypercubeNode*>::iterator it;
    
    for (it = nodes.begin(); it!= nodes.end(); it++) {
        map<MACAddress, Neighbour>::iterator itConn = it->second->getHypercubeControlLayer()->getNeighbours().begin();
        map<MACAddress, Neighbour>::iterator end = it->second->getHypercubeControlLayer()->getNeighbours().end();
        for (; itConn != end; itConn++) {            
            if (itConn->second.getType() == Neighbour::PARENT || 
                    itConn->second.getType() == Neighbour::CHILD || 
                    itConn->second.getType() == Neighbour::ADJACENT) {
                
               HypercubeNode *neighNode = dynamic_cast<HypercubeNetwork*>(Simulator::getInstance()->getNetwork())->getNode(itConn->second.getPrimaryAddress());
               if (it->second->getId() > neighNode->getId()) {
                   ofile << it->second->getId() << "," << neighNode->getId() << endl;
               }
            } 
        }
    }
}

/**
 * @brief Get the name of this object.
 *
 * @return "Network"
 */
string HypercubeNetwork::getName() const
{
    return "Network";
}


/**
 * @brief Add a node to the network.
 *
 * @param node node to add.
 */
void HypercubeNetwork::addNode(TNode *node)
{
    HypercubeNode *hcn = dynamic_cast<HypercubeNode*>(node);
    nodes[hcn->getUniversalAddress()] = hcn;
}


/**
 * @brief Set the working address length in bits.
 *
 * @param addressLength the working address length in bits.
 */
void HypercubeNetwork::setAddressLength(int addressLength)
{
    this->addressLength = addressLength;
}

/**
 * @brief Get the working address length in bits.
 *
 * @return the working address length in bits.
 */
int HypercubeNetwork::getAddressLength() const
{
    return addressLength;;
}

/**
 * @brief Get a pointer to the node with the specified address. It throws an exception if not found.
 *
 * @param addr universal address of the node to find.
 * @return a pointer to the node with the specified address.
 */
HypercubeNode* HypercubeNetwork::getNode(const UniversalAddress &addr)
{
    return getNodeIterator(addr)->second;
}

/**
 * @brief Get a pointer to the node with the specified address. It returns null if not found.
 *
 * @param addr primary address of the node to find.
 * @return a pointer to the node with the specified address.
 */
HypercubeNode* HypercubeNetwork::getNode(const HypercubeAddress &addr)
{
    HypercubeNode *node;
    
    // try to find it in the cache
    map<HypercubeAddress, UniversalAddress>::iterator it = addressCache.find(addr);
    if (it != addressCache.end()) {
       node = getNode(it->second);

       // if found and ok, return it, if not erase and find it again
       if (node->getPrimaryAddress() == addr) return node;
       else addressCache.erase(it);
    }                       

    // Sequential Search
    map<UniversalAddress, HypercubeNode*>::iterator it2 = nodes.begin();
    while (it2 != nodes.end()) {
        if (it2->second->getPrimaryAddress() == addr) {
            addressCache.insert(make_pair(addr, it2->second->getUniversalAddress()));
            return it2->second;
        }
        it2++;
    }

    return NULL;

}

/**
 * @brief Get an iterator pointing to the node with the specified address. It throws an exception if not found.
 *
 * @param addr Address to find
 * @return an iterator pointing to the node with the specified address. 
 */
HypercubeNetwork::NodesIterator HypercubeNetwork::getNodeIterator(const UniversalAddress &addr) 
{
    map<UniversalAddress, HypercubeNode*>::iterator it = nodes.find(addr);
    if (it == nodes.end()) throw invalid_argument("Node not found: " + addr.toString());
    return it;
}
/*
int HypercubeNetwork::getShortestPath(const UniversalAddress &source, const UniversalAddress &dest, bool allconnections)  
{
    if (source == dest) return 0;

    HypercubeNetwork *network = dynamic_cast<HypercubeNetwork*>(Simulator::getInstance()->getNetwork());
    vector<string> visiting, toVisit;
    set<string> visited;
    int d = 1;

    string destination = dest.toString();
    visiting.push_back(source.toString());

    while (!visiting.empty()) {   
        for (int i = 0; i < visiting.size();  i++) {
            visited.insert(visiting[i]);

            multimap<string, string>::iterator it = connections.find(visiting[i]);
            multimap<string, string>::iterator end = connections.end();
        
            for (; it != end && it->first == visiting[i]; it++) {
               if (it->second == destination) return d;
               if (visited.find(it->second) == visited.end()) {
                  toVisit.push_back(it->second);
            visited.insert(it->second);
               }
            }
        }
        visiting = toVisit;
//        cout << "visiting size: " << visiting.size() << endl;
        toVisit.clear();
        d++;
    }

    return -1;
}
*/

int HypercubeNetwork::getShortestPath(const UniversalAddress &source, const UniversalAddress &dest, bool allconnections)  
{
    if (source == dest) return 0;

    HypercubeNetwork *network = dynamic_cast<HypercubeNetwork*>(Simulator::getInstance()->getNetwork());
    vector<UniversalAddress> visiting, toVisit;
    set<UniversalAddress> visited;
    int d = 1;

    visiting.push_back(source);

    while (!visiting.empty()) {   
        for (int i = 0; i < visiting.size();  i++) {
            visited.insert(visiting[i]);

            NodesIterator it = getNodeIterator(visiting[i]);
            map<MACAddress, Neighbour>::iterator itConn = it->second->getHypercubeControlLayer()->getNeighbours().begin();
            map<MACAddress, Neighbour>::iterator end = it->second->getHypercubeControlLayer()->getNeighbours().end();
            for (; itConn != end; itConn++) {            
                if (allconnections || itConn->second.getType() == Neighbour::PARENT || 
                        itConn->second.getType() == Neighbour::CHILD || 
                        itConn->second.getType() == Neighbour::ADJACENT) {
                    
                   HypercubeNode *neighNode = network->getNode(itConn->second.getPrimaryAddress());
                   UniversalAddress a = neighNode->getUniversalAddress();
                   if (a == dest) return d;
                   if (visited.find(a) == visited.end()) {
                      toVisit.push_back(a);
                      visited.insert(a);
  
                   }
                } 
            }
        }
        visiting = toVisit;
        toVisit.clear();
        d++;
    }

    return -1;
}

}}
