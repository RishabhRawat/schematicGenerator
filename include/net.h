#ifndef NETS_H
#define NETS_H
#include<common.h>
#include<terminal.h>



class net
{
    friend class placement;
private:
//    std::vector<terminal> source;
//    std::vector<terminal> sink;
    terminalCollection connectedTerminals;

    moduleTerminalMap connectedModuleTerminalMap;
    int netWidth;
    terminalCollection getConnectedTerminals() const {return connectedTerminals;}

public:
    net();
    int getNetWidth() const { return netWidth;}
};

namespace std
{
    template <>
    struct hash<net>
    {
        size_t operator()(const net& k) const
        {
            return ((size_t) &k) / sizeof(k);
        }
    };
}

#endif // NETS_H
