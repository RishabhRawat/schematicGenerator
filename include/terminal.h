#ifndef TERMINAL_H
#define TERMINAL_H
#include <string>
#include <common.h>

class terminal
{
private:
    std::string terminalIdentifier;
    terminalType type;
    terminalSide side;
    intPair terminalPosition;
    int terminalWidth;
    modules * const parentModule;
    bool systemTerminal;
    net * attachedNet;

    void updateTerminalSide();

public:
    terminal(std::string name, modules *parentPointer):terminalIdentifier(name),parentModule(parentPointer) {
    }

    /* sets position relative to the module which owns the terminal,
     * relative to bottom left corner, must be set after the module size is set
     * has no meaning for systemTerminals
     */
    void setRelativePosition(int x, int y);
    bool isSystemTerminal(){
        return systemTerminal;
    }

};

#endif // TERMINAL_H
