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
    for(auto const& n: internalNets) {
        for(auto const& t: n->getConnectedTerminals()) {
            n->connectedModules.insert(t->getParent());
        }
    }

    //Setting up module connectivity
    for(auto const& m: subModules) {
        for(auto const& t: m->getTerminals()) {
//            for(auto const& otherModules: t->) {

//            }
        }
    }
}

void placement::partitionFormation() {


}

modules* selectSeed(hashlib::pool<modules*> moduleSet) {
    int maxConnections = 0;
    for(auto const& m: moduleSet) {
        int connections = 0;
        for(auto const& t: m->getTerminals()) {
//            for(auto const& value: t.) {

        }
    }
}
