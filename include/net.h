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
    std::vector<terminal*> connections;
    hashlib::pool<module*> connectedModules;
    int netWidth;
    std::vector<terminal*> getConnectedTerminals() const {return connections;}

    //I need to attach const& here (which I should), but somehow it doesnt compile without
    hashlib::pool<module*> const& getConnectedModules() const {return connectedModules;}
public:
    net();

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
