#ifndef TERMINAL_H
#define TERMINAL_H
#include <string>
#include <common.h>

class terminal
{
private:
    string terminalIdentifier;
    terminalType terminalType;
    int terminalPosition[2];
    int terminalWidth;
    modules * const parentModule;
    bool systemTerminal;
    net * const attachedNet;
public:
    terminal();
};

#endif // TERMINAL_H
