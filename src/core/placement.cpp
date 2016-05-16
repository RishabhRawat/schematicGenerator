#include "placement.h"

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

//module* selectSeed(hashlib::pool<module*> moduleSet) {
//    int maxConnections = 0;
//    for(auto const& m: moduleSet) {
//        int connections = 0;
//        for(auto const& t: m->getTerminals()) {
//            for(auto const& value: t.) {

//        }
//    }
//}
