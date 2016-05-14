#ifndef PLACEMENT_H
#define PLACEMENT_H
#include <module.h>
#include <net.h>
//#include <unordered_set>

class placement
{
    typedef std::vector<module*> box;
    typedef std::vector<box*> partition;


private:
//    std::unordered_set<modules> subModules;
//    std::unordered_set<nets> internalNets;
    std::vector<module*> subModules;
    std::vector<net*> internalNets;
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
    module* selectSeed(hashlib::pool<module*> moduleSet);
public:
    placement();
    void doPlacement();
};

#endif // PLACEMENT_H
