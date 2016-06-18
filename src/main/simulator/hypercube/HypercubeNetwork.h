#ifndef _HYPERCUBE_NETWORK_H_
#define _HYPERCUBE_NETWORK_H_

//#include <queue>
#include <map>


#include "UniversalAddress.h"
#include "HypercubeAddress.h"
#include "HypercubeNode.h"

namespace simulator {
	namespace hypercube {

using namespace std;

/**
 * @brief Represents a Network that works with Hypercube protocol.
 */
class HypercubeNetwork : public TNetwork {
    public:
        typedef map<UniversalAddress, HypercubeNode*>::iterator NodesIterator;

        HypercubeNetwork(int addressLength = 8);
        virtual void addNode(TNode *node);

        void setAddressLength(int addressLength);
        int getAddressLength() const;

        HypercubeNode* getNode(const UniversalAddress &addr);
        HypercubeNode* getNode(const HypercubeAddress &addr);

        void exportConnections(const string &filename);

        int getShortestPath(const UniversalAddress &source, const UniversalAddress &dest, bool allconnections);
        virtual TCommandResult *runCommand(const Function &f);
        virtual string getName() const;

    private:
        NodesIterator getNodeIterator(const UniversalAddress &addr);
        
        /// Nodes in the network
        map<UniversalAddress, HypercubeNode*> nodes;

        // Cache to get the universal address from the node's primary address        
        map<HypercubeAddress, UniversalAddress> addressCache;

        /// Address length used in the network.
        int addressLength;
};



} }

#endif
