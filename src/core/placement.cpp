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
    for(auto & n: internalNets) {
        for(auto const& t: n->getConnectedTerminals()) {
            //PUT A SETTER
            n->connectedModules.insert(t->getParent());
        }
    }

    //Setting up module connectivity
    for(auto const& m: subModules) {
        for(auto const& t: m->getTerminals()) {
              for(auto & otherModules: (t.getNet())->getConnectedModules()) {
                if(otherModules != m)
                    //PUT A SETTER
                    m->connectedModules.insert(otherModules);
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
