#include <string>
#include "net.h"

net::net(const std::string &netName, const int netWidth) : netIdentifier(netName), netWidth(netWidth),
                                                           highIndex(netWidth - 1), lowIndex(0), highToLow(true) {
	internalBitNets = new bitNet*[netWidth];
	for(int i = 0; i < netWidth; ++i) {
		internalBitNets[i] = new bitNet(this,i);
	}
}

net::net(const net &baseNet, const int highIndex, const int lowIndex, const bool highToLow) :
		netIdentifier(baseNet.netIdentifier+"["+std::to_string(highIndex)+":"+std::to_string(lowIndex)+"]"),
		netWidth(highIndex - lowIndex + 1), highIndex(highIndex), lowIndex(lowIndex), highToLow(highToLow) {
	internalBitNets = new bitNet*[netWidth];
	for(int i = 0; i < netWidth; ++i) {
		internalBitNets[i] = baseNet.internalBitNets[lowIndex+i];
	}
}

net::~net() {
	if(internalBitNets[lowIndex]->baseNet == this) {
		for (int i = 0; i < netWidth; ++i) {
			delete internalBitNets[i];
		}
	}
	delete[] internalBitNets;

}

net net::partialNet(int index1, int index2) {
	if(index1 < lowIndex || index2 < lowIndex || index1 > highIndex || index2 > highIndex)
		throw std::invalid_argument("Index value out of range");
	return net(*this,index1,index2,index1>index2);
}

