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
//Same with this function
inline int countNets(linkCollection lC) {
    int connections = 0;
    connections = lC.size();
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
    for(net& n: internalNets) {
        for(terminal * t: n.getConnectedTerminals()) {

            moduleTerminalMap::iterator mapIterator = n.connectedModuleTerminalMap.find(t->getParent());
            if(mapIterator == n.connectedModuleTerminalMap.end()) {
                terminalCollection newCollection;
                newCollection.push_back(t);
                //NOTE: I presume that in insert we have a copy, so even when the lifetime
                // of newCollection ends we wont have a problem
                // need a better solution to this maybe
                n.connectedModuleTerminalMap.insert({t->getParent(),newCollection});
            }
            else {
                mapIterator->second.push_back(t);
            }
        }
    }

    //Setting up module connectivity
    for(module& m: subModules) {
        for(terminal& t: m.getTerminals()) {
            for(moduleTerminalPair& pair: (t.getNet())->connectedModuleTerminalMap) {
                if(pair.first != &m){
                    moduleLinkMap::iterator mapIterator = m.connectedModuleLinkMap.find(pair.first);

                    ulink * newLink;
                    newLink->linkNet = t.getNet();
                    newLink->linksource = &t;
                    newLink->linksink = &pair.second;

                    if(mapIterator == m.connectedModuleLinkMap.end()) {
                        linkCollection *newCollection = new linkCollection();
                        newCollection->push_back(newLink);
                        m.connectedModuleLinkMap.insert({pair.first,*newCollection});
                    }
                    else {
                        mapIterator->second.push_back(newLink);
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

    for(module& m: subModules)
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
        for(moduleLinkPair& pair: m->connectedModuleLinkMap) {
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
            for(moduleLinkPair& pair: m->connectedModuleLinkMap) {
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

moduleCollection placement::selectRoots(partition *p) {
    if(p->partitionBoxes.size()>1) throw "Invalid Partition Error: placment::selectRoots";

    box * b = p->partitionBoxes.pop();
    moduleCollection roots;
    for(module *m:b->boxModules) {
        bool seed = false;
        for(moduleLinkPair pair: m->connectedModuleLinkMap) {
            if(b->boxModules.find(pair.first) == b->boxModules.end() && pair.first != &systemModule){
                seed = true;
            }
            else if(pair.first == &systemModule) {
                for(ulink *l: pair.second){
                    for(terminal* t: *(l->linksink)) {
                        if (t->type == in || t->type == inout){
                            seed = true;
                            break;
                        }
                    }
                    if(seed)
                        break;
                }
            }
            if(seed)
                break;
        }

        int outGoingNets = 0;
        for(terminal& t:m->getTerminals()) {
            if(t.type == out)
                outGoingNets++;
            if(outGoingNets > 1)
                break;
        }
        if(seed)
            roots.push_back(m);
    }
    return roots;
}

