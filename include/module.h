#ifndef MODULES_H
#define MODULES_H
#include <common.h>
#include <terminal.h> //for terminal vector

namespace hashlib {
    template<> struct hash_ops<module*> : hash_ptr_ops {};
}


class module
{
    friend class placement;

private:
    std::string moduleType;
    const std::string moduleIdentifier;
    intPair modulePosition;

    /* First along x-axis then along y-axis*/
    intPair moduleSize = {50,100}; //Default size

    //TODO: Currently using vectors for terminals
    //need to be sure if this is appropriate,
    //it seems better than unordered set as the terminals are limited
    std::vector<terminal> moduleTerminals;

    hashlib::pool<module*> connectedModules;


public:
    module();
    std::string getIdentifier() const { return moduleIdentifier;}
    intPair getModuleSize() const{ return moduleSize; }
    std::vector<terminal> getTerminals() const{ return moduleTerminals; }


};

namespace std
{
    template <>
    struct hash<module>
    {
        size_t operator()(const module& k) const
        {
            return ((size_t) &k) / sizeof(k);
        }
    };
}
#endif // MODULES_H
