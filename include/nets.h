#ifndef NETS_H
#define NETS_H
#include<common.h>
#include<terminal.h>
#include<vector>
#include<hashlib.h>

class nets
{
    friend class placement;
private:
//    std::vector<terminal> source;
//    std::vector<terminal> sink;
    std::vector<terminal*> connections;
    hashlib::pool<modules*> connectedModules;
    int netWidth;
public:
    nets();
    std::vector<terminal*> getConnectedTerminals(){return connections;}
};

namespace std
{
    template <>
    struct hash<nets>
    {
        size_t operator()(const nets& k) const
        {
            return ((size_t) &k) / sizeof(k);
        }
    };
}

#endif // NETS_H
