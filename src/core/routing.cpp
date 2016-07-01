#include "routing.h"
#include "coreDesign.h"
void routing::route() {
	addObstacleBounding();
	for (coalescedNet* internalCoalescedNet : core->internalCoalescedNets) {
		splicedTerminalSet tSet;
		for (auto&& mT_pair : internalCoalescedNet->connectedModuleSplicedTerminalMap) {
			for (splicedTerminal* t : mT_pair.second) {
				tSet.insert(t);
			}
		}

		if (internalCoalescedNet->connectedModuleSplicedTerminalMap.size() < 2)
			throw std::runtime_error("Dangling net??");

		splicedTerminal* t0 = tSet.pop();
		splicedTerminal* t1 = tSet.pop();
		initNet(t0, t1);
		while (!tSet.empty()) {
			expandNet(tSet.pop());
		}
	}
}

void routing::addObstacleBounding() {}

void routing::initNet(splicedTerminal* t0, splicedTerminal* t1) {}

void routing::expandNet(splicedTerminal* pT) {}



