#ifndef MODULES_H
#define MODULES_H
#include <common.h>
#include <terminal.h> //for terminal vector

class module
{
    friend class placement;

private:
    std::string moduleType;
    const std::string moduleIdentifier;
    intPair modulePosition;

    /* First along x-axis then along y-axis*/
    intPair moduleSize = {50,100}; //Default size

    terminalCollection moduleTerminals;

    moduleNetMap connectedModuleNetMap;


public:
    module();
    std::string getIdentifier() const { return moduleIdentifier;}
    intPair getModuleSize() const{ return moduleSize; }
    std::vector<terminal*> getTerminals() const{ return moduleTerminals; }


};
#endif // MODULES_H
