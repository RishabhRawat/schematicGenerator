#ifndef NETS_H
#define NETS_H

#include "common.h"


struct bitNet {
	net * const baseNet;
	const int index;
	std::vector<bitTerminal*> connectedBitTerminals;
	bitNet(net *const baseNet, const int index) : baseNet(baseNet), index(index) { }
};


class net {
	friend class splicedTerminal;
	friend class schematicGenerator;

public:
	const std::string netIdentifier;
	const int netWidth;

	net(const std::string &netName, const int netWidth);

	net partialNet(int index1, int index2);
	~net();

private:
	bitNet ** internalBitNets;

	const int highIndex;
	const int lowIndex;
	const bool highToLow;

	coalescedNet * coalesced;

	net(const net &baseNet, const int highIndex, const int lowIndex, const bool highToLow);
};

class coalescedNet {
	friend class schematicGenerator;
	const net * const sourceNet;
	moduleSplicedTerminalMap connectedModuleSplicedTerminalMap;
	coalescedNet(const net *const sourceNet) : sourceNet(sourceNet) { }
};

#endif // NETS_H
