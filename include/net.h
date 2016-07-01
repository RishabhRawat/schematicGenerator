#ifndef NETS_H
#define NETS_H

#include "common.h"

struct bitNet {
	net* const baseNet;
	const int index;
	std::vector<bitTerminal*> connectedBitTerminals;
	bitNet(net* const baseNet, const int index) : baseNet(baseNet), index(index) {}
	bitNet(const bitNet&) = delete;
	bitNet& operator=(const bitNet&) = delete;
};

class net {
	friend class terminal;
	friend class splicedTerminal;
	friend class placement;
	friend class coreDesign;

public:
	const std::string netIdentifier;
	const int netWidth;

	net(const std::string& netName, const int netWidth);

	net partialNet(int index1, int index2);
	net operator[](int index);
	~net();
	net& operator=(const net&) = delete;

private:
	bitNet** internalBitNets;

	const int highIndex;
	const int lowIndex;
	const bool highToLow;

	coalescedNet* coalesced = nullptr;

	net(const net& baseNet, const int highIndex, const int lowIndex, const bool highToLow);

#ifdef WEB_COMPILATION
public:
// A get around for emscripten
#endif
	net(const net&) = default;
};

class coalescedNet {
	friend class coreDesign;
	friend class placement;
	const net* const sourceNet;
	moduleSplicedTerminalMap connectedModuleSplicedTerminalMap;
	coalescedNet(const net* const sourceNet) : sourceNet(sourceNet) {}
};

#endif  // NETS_H
