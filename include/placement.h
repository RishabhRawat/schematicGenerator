#ifndef PLACEMENT_H
#define PLACEMENT_H
#include <module.h>
#include <net.h>
//#include <unordered_set>

class placement
{


private:
//    std::unordered_set<modules> subModules;
//    std::unordered_set<nets> internalNets;
    std::vector<module*> subModules;
    std::vector<net*> internalNets;
    std::vector<terminal*> systemTerminals;

    // Do not ask me why... just gut feel...
    // Instead give me a reason saying this is wrong
    hashlib::pool<partition*> allPartitions;

    //Important Layout Parameters
    int maxPartitionSize;
    int maxPartitionConnections;


    void initializeStructures();

    void partitionFormation();
    module* selectSeed(hashlib::pool<module*> moduleSet);
    int connectionsToExistingPartitions(module *m);
    partition *createPartition(hashlib::pool<module *> &moduleSet, module *seed);

    void boxFormation();

    void modulePlacement();
    void boxPlacement();
    void partitionPlacement();
    void terminalPlacement();



public:
    placement();
    void doPlacement();
};

#endif // PLACEMENT_H
