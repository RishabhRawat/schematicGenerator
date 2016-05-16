#ifndef COMMON_H
#define COMMON_H
#include <hashlib.h>
#include <vector>
#include <string>

enum terminalType {
    in, out, inout
};

enum terminalSide {
    left, right, top, bottom, none
};

struct intPair {
    int x;
    int y;
};



class module;
class terminal;
class net;
class box;
class partition;

namespace hashlib {
template<> struct hash_ops<net*> : hash_ptr_ops {};
template<> struct hash_ops<module*> : hash_ptr_ops {};
template<> struct hash_ops<box*> : hash_ptr_ops {};
template<> struct hash_ops<partition*> : hash_ptr_ops {};
}


typedef std::vector<terminal*> terminalCollection;
typedef std::vector<net*> netCollection;

typedef hashlib::dict<module*,std::vector<net*>> moduleNetMap;
typedef hashlib::dict<module*,std::vector<terminal*>> moduleTerminalMap;

typedef std::pair<module*,std::vector<net*>> moduleNetPair;
typedef std::pair<module*,std::vector<terminal*>> moduleTerminalPair;


#endif // COMMON_H
