#ifndef PLACEMENT_H
#define PLACEMENT_H
#include <common.h>
#include <modules.h>
#include <nets.h>
#include <unordered_set>

class placement
{
private:
    std::unordered_set<modules> subModules;
    std::unordered_set<nets> internalNets;
    std::vector<terminal> systemTerminals;
public:
    placement();
};

#endif // PLACEMENT_H
