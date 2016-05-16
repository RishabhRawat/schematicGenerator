#include "placement.h"
#include "partition.h"
#include "box.h"
#include <iostream>

placement::placement()
{

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
    for(net* n: internalNets) {
        for(terminal * t: n->getConnectedTerminals()) {

            moduleTerminalMap::iterator mapIterator = n->connectedModuleTerminalMap.find(t->getParent());
            if(mapIterator == n->connectedModuleTerminalMap.end()) {
                terminalCollection *newCollection = new terminalCollection();
                newCollection->push_back(t);
                n->connectedModuleTerminalMap.insert({t->getParent(),*newCollection});
            }
            else {
                mapIterator->second.push_back(t);
            }
        }
    }

    //Setting up module connectivity
    for(module* m: subModules) {
        for(terminal* t: m->getTerminals()) {
            for(moduleTerminalPair pair: (t->getNet())->connectedModuleTerminalMap) {
                if(pair.first != m){
                    moduleNetMap::iterator mapIterator = m->connectedModuleNetMap.find(pair.first);
                    if(mapIterator == m->connectedModuleNetMap.end()) {
                        netCollection *newCollection = new netCollection();
                        newCollection->push_back(t->getNet());
                        m->connectedModuleNetMap.insert({pair.first,*newCollection});
                    }
                    else {
                        mapIterator->second.push_back(t->getNet());
                    }
                }
            }
        }
    }
}

void placement::partitionFormation() {


}

module* placement::selectSeed(hashlib::pool<module*> moduleSet) {
    int maxConnections = 0;
    module * seed;
    for(module* m: moduleSet) {
        int connections = 0;
        for(moduleNetPair pair: m->connectedModuleNetMap) {
            for(net* n:pair.second) {
                connections+=n->getNetWidth();
            }
        }
        if(connections>maxConnections) {
            maxConnections = connections;
            seed = m;
        }
        else if(connections == maxConnections) {
            //TODO: Implement Tie Breaker
            std::cerr<<"Uninmplemented Tie Breaker"<<std::endl;
        }
    }
    if(!seed)
        throw "Bad moduleSet in placement::selectSeed";
    return seed;
}

void placement::createPartition(hashlib::pool<module*> moduleSet, module * seed) {
    partition * newPartition = new partition();
    box * rootBox = new box();
    newPartition->add(rootBox);
    int connections = 0;
    int partitionEntries = 0;
    while(!moduleSet.empty() && (partitionEntries < maxPartitionSize) && (connections < maxPartitionConnections)){
        module * selectedModule;
        //TODO: algo to select module



        moduleSet.erase(selectedModule);
        rootBox->add(selectedModule);
        for(moduleNetPair pair:selectedModule->connectedModuleNetMap){
            if(pair.first->parentBox != rootBox){
                for(net* n:pair.second){
                    connections+=n->getNetWidth();
                }
            }
            else{
                for(net* n:pair.second){
                    connections-=n->getNetWidth();
                }
            }
        }
    }
    allPartitions.insert(newPartition);
}

//int placement::connectionsToExistingPartitions(module *m) {}
