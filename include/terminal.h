#ifndef TERMINAL_H
#define TERMINAL_H
#include <string>
#include <common.h>

class terminal
{
private:
    std::string terminalIdentifier;
    tType terminalType;
    int terminalPosition[2];
    int terminalWidth;
    modules * const parentModule;
    bool systemTerminal;
    net * attachedNet;
public:
    terminal(std::string name, modules *parentPointer):terminalIdentifier(name),parentModule(parentPointer) {

    }
};

#endif // TERMINAL_H
