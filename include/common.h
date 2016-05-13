#ifndef COMMON_H
#define COMMON_H

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

class modules;
class terminal;
class net;

#endif // COMMON_H
