#ifndef NETS_H
#define NETS_H
#include<common.h>
#include<terminal.h>
#include<vector>

class nets
{
private:
//    std::vector<terminal> source;
//    std::vector<terminal> sink;
    std::vector<terminal> connections;
    int netWidth;
public:
    nets();
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
