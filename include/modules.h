#ifndef MODULES_H
#define MODULES_H
#include <string>
#include <vector>
#include <common.h>
#include <terminal.h> //for terminal vector

class modules
{
private:
    std::string moduleType;
    std::string moduleIdentifier;
    int modulePosition[2];
    //TODO: Currently using vectors for terminals
    //need to be sure if this is appropriate,
    //it seems better than unordered set as the terminals are limited
    std::vector<terminal> moduleTerminals;

public:
    modules();
};

#endif // MODULES_H
