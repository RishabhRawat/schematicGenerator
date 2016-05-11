#ifndef MODULES_H
#define MODULES_H
#include <string>
#in
#include <common.h>

class modules
{
private:
    string moduleType;
    string moduleIdentifier;
    int modulePosition[2];
    //TODO: Currently using vectors for terminals
    //need to be sure if this is appropriate,
    //it seems better than unordered set as the terminals are limited
    vector<terminal> moduleTerminals;

public:
    modules();
};

#endif // MODULES_H
