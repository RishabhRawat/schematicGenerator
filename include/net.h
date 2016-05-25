#ifndef NETS_H
#define NETS_H
#include "common.h"
#include "terminal.h"



class net
{
    friend class placement;
private:

	const std::string netIdentifier;
	terminalCollection connectedTerminals;
    moduleTerminalMap connectedModuleTerminalMap;
    int netWidth;

	terminalCollection getConnectedTerminals() const {
        return connectedTerminals;
    }

public:

	net(const std::string &netName) : netIdentifier(netName) { }

	int getNetWidth() const {
	    return netWidth;
    }
};

#endif // NETS_H
