#include "placement.h"
#include "partition.h"
#include "box.h"
#include <iostream>

inline int countNets(netCollection nC) {
    int connections = 0;
    //If you want give weigtaget to net width
    //    for(net* n:nC) {
    //        connections+=n->getNetWidth();
    //    }

    //If you just want to count nets
    connections = nC.size();
    return connections;
}


placement::placement()
{

}

placement::~placement() {
    for(partition *p: allPartitions) {
        for(box *b: p->partitionBoxes){
            delete b;
        }
        delete p;
    }
}

void placement::doPlacement() {
    initializeStructures();
    partitionFormation();
    boxFormation();
    modulePlacement();
    boxPlacement();
    partitionPlacement();
    terminalPlacement();
}

void placement::initializeStructures() {
    //Setting up net connectivity
    for(net n: internalNets) {
        for(terminal * t: n.getConnectedTerminals()) {

            moduleTerminalMap::iterator mapIterator = n.connectedModuleTerminalMap.find(t->getParent());
            if(mapIterator == n.connectedModuleTerminalMap.end()) {
                terminalCollection *newCollection = new terminalCollection();
                newCollection->push_back(t);
                n.connectedModuleTerminalMap.insert({t->getParent(),*newCollection});
            }
            else {
                mapIterator->second.push_back(t);
            }
        }
    }

    //Setting up module connectivity
    for(module m: subModules) {
        for(terminal t: m.getTerminals()) {
            for(moduleTerminalPair pair: (t.getNet())->connectedModuleTerminalMap) {
                if(pair.first != &m){
                    moduleNetMap::iterator mapIterator = m.connectedModuleNetMap.find(pair.first);
                    if(mapIterator == m.connectedModuleNetMap.end()) {
                        netCollection *newCollection = new netCollection();
                        newCollection->push_back(t.getNet());
                        m.connectedModuleNetMap.insert({pair.first,*newCollection});
                    }
                    else {
                        mapIterator->second.push_back(t.getNet());
                    }
                }
            }
        }
    }
}

void placement::partitionFormation() {
    module * seed;
    partition * newPartition;
    hashlib::pool<module*> moduleSet;

    for(module m: subModules)
        moduleSet.insert(&m);

    while(!moduleSet.empty()){
        seed = selectSeed(moduleSet);
        moduleSet.erase(seed);
        newPartition = createPartition(moduleSet,seed);
        allPartitions.insert(newPartition);
    }
}

module* placement::selectSeed(hashlib::pool<module*> moduleSet) {
    int maxConnectionsInFreeSet = -1;
    int minConnectionsOutFreeSet = INT32_MAX;
    module * seed;
    for(module* m: moduleSet) {
        int connectionsInFreeSet = 0;
        int connectionsOutFreeSet = 0;
        // The choice is either to iterate over all modules connected to m
        // or to iterate over all elements in moduleSet and find those connected to m
        for(moduleNetPair pair: m->connectedModuleNetMap) {
            if(moduleSet.find(pair.first) != moduleSet.end())
                connectionsInFreeSet += countNets(pair.second);
            else
                connectionsOutFreeSet += countNets(pair.second);
        }

        if((connectionsInFreeSet > maxConnectionsInFreeSet) ||
                ((connectionsInFreeSet == maxConnectionsInFreeSet) &&
                 (connectionsOutFreeSet <= minConnectionsOutFreeSet))){
            seed = m;
            maxConnectionsInFreeSet = connectionsInFreeSet;
            minConnectionsOutFreeSet = connectionsOutFreeSet;
        }
    }
    if(!seed || maxConnectionsInFreeSet < 0 ||
            minConnectionsOutFreeSet == INT32_MAX)
        throw "Bad moduleSet in placement::selectSeed";
    return seed;
}

partition * placement::createPartition(hashlib::pool<module*>& moduleSet, module * seed) {
    box * rootBox = new box();
    rootBox->add(seed);
    partition * newPartition = new partition();
    newPartition->add(rootBox);
    int connections = 0;
    int partitionEntries = 0;
    while(!moduleSet.empty() && (partitionEntries < maxPartitionSize)
          && (connections < maxPartitionConnections)) {

        module * selectedModule;
        int maxConnectionsInPartition = -1;
        int minConnectionsOutPartion = INT32_MAX;

        //Algo to select module
        for(module* m: moduleSet) {
            int connectionsInPartition = 0;
            int connectionsOutPartition = 0;
            for(moduleNetPair pair: m->connectedModuleNetMap) {
                if(pair.first->parentBox == rootBox)
                    connectionsInPartition += countNets(pair.second);
                else
                    connectionsOutPartition += countNets(pair.second);
            }

            if((connectionsInPartition > maxConnectionsInPartition) ||
                    ((connectionsInPartition == maxConnectionsInPartition) &&
                     (connectionsOutPartition <= minConnectionsOutPartion))) {
                selectedModule = m;
                maxConnectionsInPartition = connectionsInPartition;
                minConnectionsOutPartion = connectionsOutPartition;
            }
        }

        if(!selectedModule || maxConnectionsInPartition < 0 ||
                minConnectionsOutPartion == INT32_MAX)
            throw "Bad selectedModule in placement::createPartition";


        moduleSet.erase(selectedModule);
        rootBox->add(selectedModule);
        connections -= maxConnectionsInPartition;
        connections +=minConnectionsOutPartion;
    }
    return newPartition;
}
