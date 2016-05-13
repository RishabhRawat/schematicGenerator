#ifndef PLACEMENT_H
#define PLACEMENT_H
#include <common.h>
#include <modules.h>
#include <nets.h>
//#include <unordered_set>

class placement
{
    typedef std::vector<modules*> box;
    typedef std::vector<box*> partition;


private:
//    std::unordered_set<modules> subModules;
//    std::unordered_set<nets> internalNets;
    std::vector<modules*> subModules;
    std::vector<nets*> internalNets;
    std::vector<terminal*> systemTerminals;

    // Do not ask me why... just gut feel...
    // Instead give me a reason saying this is wrong
    std::vector<partition*> allPartitions;

    void initializeStructures();
    void partitionFormation();
    void boxFormation();
    void modulePlacement();
    void boxPlacement();
    void partitionPlacement();
    void terminalPlacement();
    modules* selectSeed(hashlib::pool<modules*> moduleSet);
public:
    placement();
    void doPlacement();
};

#endif // PLACEMENT_H
