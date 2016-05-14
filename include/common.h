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

namespace hashlib {
    template<> struct hash_ops<net*> : hash_ptr_ops {};
}


#endif // COMMON_H
